#pragma once

#include <sstream>

#include <DirectXTex/DirectXTex.h>

#include "ReflectionCapture.hpp"

#include "utils/Logger/Logger.hpp"

namespace engn {
	namespace rend {
		void ReflectionCapture::init(const std::vector<std::string>& skyTexturePaths)
		{
			initCubeBuffers();
			initPipelines();
			initDiffuseIrradianceCubeMap();
			initPreFilteredSpecularCubeMap();

			m_worldToClipBuffer.init();
			m_textureMapPaths = skyTexturePaths;
		}
		void ReflectionCapture::initCubeBuffers()
		{
			std::vector vertices =
			{
				VertexPos{{1.0f, 1.0f, -1.0f}}, // top-right-front
				VertexPos{{-1.0f, 1.0f, -1.0f}}, // top-left-front
				VertexPos{{1.0f, -1.0f, -1.0f}}, // bottom-right-front
				VertexPos{{-1.0f, -1.0f, -1.0f}}, // bottom-left-front

				VertexPos{{1.0f, 1.0f, 1.0f}}, // top-right-back
				VertexPos{{-1.0f, 1.0f, 1.0f}}, // top-left-back
				VertexPos{{1.0f, -1.0f, 1.0f}}, // bottom-right-back
				VertexPos{{-1.0f, -1.0f, 1.0f}}, // bottom-left-back
			};
			std::vector<DWORD> indices =
			{
				2, 0, 1, // front right
				3, 2, 1, // front left

				4, 0, 2, // right left
				6, 4, 2, // right right

				1, 5, 3, // left right
				5, 7, 3, // left left

				7, 5, 6, // back right
				5, 4, 6, // back left

				4, 5, 1, // top right
				0, 4, 1, // top left

				6, 2, 3, // back right
				7, 6, 3, // back left
			};

			m_cubeVertexBuffer.init(vertices);
			m_cubeIndexBuffer.init(indices);
		}
		void ReflectionCapture::initPipelines()
		{
			//! Input layout
			D3D11_INPUT_ELEMENT_DESC layout[] = { "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };

			//! Depth stencil state
			D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc{};
			depthStencilStateDesc.DepthEnable = true;
			depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
			//! Rasterizer state
			D3D11_RASTERIZER_DESC rasterizerDesc{};
			rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
			rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

			// Init Pipelines
			const std::wstring exeDirW = util::getExeDirW();

			// Diffuse Irradiance
			PipelineData diffIrradiancePipelineData{
				layout,
				ARRAYSIZE(layout),
				D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				exeDirW + DI_AND_PFS_VS_NAME,
				L"", L"", L"",
				exeDirW + DI_PS_NAME,
				rasterizerDesc,
				depthStencilStateDesc
			};

			initPipeline(m_diffuseIrradiancePipeline, diffIrradiancePipelineData);

			// Pre filtered Specular
			PipelineData preFiltSpecPipelineData{
				layout,
				ARRAYSIZE(layout),
				D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				exeDirW + DI_AND_PFS_VS_NAME,
				L"", L"", L"",
				exeDirW + PFS_PS_NAME,
				rasterizerDesc,
				depthStencilStateDesc
			};

			initPipeline(m_preFilteredSpecularPipeline, preFiltSpecPipelineData);

			// BRDF Integration Specular
			PipelineData integrBRDFPipelineData{
				nullptr,
				0,
				D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				exeDirW + BRDF_VS_NAME,
				L"", L"", L"",
				exeDirW + BRDF_PS_NAME,
				rasterizerDesc,
				depthStencilStateDesc
			};

			initPipeline(m_BRDFIntegrationPipeline, integrBRDFPipelineData);

		}
		void ReflectionCapture::initDiffuseIrradianceCubeMap()
		{
			D3D11_TEXTURE2D_DESC cubemapTextureDesc;
			cubemapTextureDesc.Width = DI_TEXTURE_DIMENSION;
			cubemapTextureDesc.Height = DI_TEXTURE_DIMENSION;
			cubemapTextureDesc.MipLevels = 1;
			cubemapTextureDesc.ArraySize = 6;
			cubemapTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			cubemapTextureDesc.SampleDesc.Count = 1;
			cubemapTextureDesc.SampleDesc.Quality = 0;
			cubemapTextureDesc.Usage = D3D11_USAGE_DEFAULT;
			cubemapTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			cubemapTextureDesc.CPUAccessFlags = 0;
			cubemapTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

			HRESULT hr = d3d::s_device->CreateTexture2D(&cubemapTextureDesc, nullptr, m_diffuseIrradianceCubemap.GetAddressOf());
			if (FAILED(hr)) {
				Logger::instance().logErr("ReflectionCapture::initDiffuseIrradianceRTVs: Failed at cubemap creation");
				return;
			}

			// Init the RTV, 1 field here will be overriden at each 2D texture in the cubemap write
			m_RTVDIDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			m_RTVDIDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			m_RTVDIDesc.Texture2DArray.MipSlice = 0;
			m_RTVDIDesc.Texture2DArray.ArraySize = 1;
		}
		void ReflectionCapture::initPreFilteredSpecularCubeMap()
		{
			D3D11_TEXTURE2D_DESC cubemapTextureDesc;
			cubemapTextureDesc.Width = PFS_TEXTURE_DIMENSION;
			cubemapTextureDesc.Height = PFS_TEXTURE_DIMENSION;
			cubemapTextureDesc.MipLevels = PFS_TEXTURE_MIPS;
			cubemapTextureDesc.ArraySize = 6;
			cubemapTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			cubemapTextureDesc.SampleDesc.Count = 1;
			cubemapTextureDesc.SampleDesc.Quality = 0;
			cubemapTextureDesc.Usage = D3D11_USAGE_DEFAULT;
			cubemapTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			cubemapTextureDesc.CPUAccessFlags = 0;
			cubemapTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;

			HRESULT hr = d3d::s_device->CreateTexture2D(&cubemapTextureDesc, nullptr, m_preFilteredSpecularCubemap.GetAddressOf());
			if (FAILED(hr)) {
				Logger::instance().logErr("ReflectionCapture::initPreFilteredSpecularCubeMap: Failed at cubemap creation");
				return;
			}

			// Init the RTV, 1 field here will be overriden at each 2D texture in the cubemap write
			m_RTVPFSDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			m_RTVPFSDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			m_RTVPFSDesc.Texture2DArray.MipSlice = 0;
			m_RTVPFSDesc.Texture2DArray.ArraySize = 1;

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = cubemapTextureDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.TextureCube.MipLevels = PFS_TEXTURE_MIPS;
			srvDesc.TextureCube.MostDetailedMip = 0;

			hr = d3d::s_device->CreateShaderResourceView(m_preFilteredSpecularCubemap.Get(), &srvDesc, m_preFilteredSpecularSRV.GetAddressOf());
			if (FAILED(hr)) {
				Logger::instance().logErr("ReflectionCapture::initPreFilteredSpecularCubeMap: Failed at cubemap SRV creation");
				return;
			}
		}
		void ReflectionCapture::initAndBindViewPort(uint32_t dimension)
		{
			D3D11_VIEWPORT viewPort;
			viewPort.TopLeftX = 0;
			viewPort.TopLeftY = 0;
			viewPort.Width = dimension;
			viewPort.Height = dimension;
			// It is set this way, despite the reversed depth matrix
			viewPort.MinDepth = 0.0f;
			viewPort.MaxDepth = 1.0f;

			d3d::s_devcon->RSSetViewports(1, &viewPort);
		}
		void ReflectionCapture::renderCube()
		{
			m_cubeVertexBuffer.bind();
			m_cubeIndexBuffer.bind();

			d3d::s_devcon->DrawIndexed(36, 0, 0);
		}
		void ReflectionCapture::generateDiffuseIrradianceCubemap()
		{
			initAndBindViewPort(DI_TEXTURE_DIMENSION);

			for (const auto& mapPath : m_textureMapPaths) {

				std::array<DirectX::ScratchImage, 6> scratchImages;
				std::array<DirectX::Image, 6> images;
				for (uint32_t i = 0; i < 6; ++i) {
					// ----- Create RTV -----
					m_RTVDIDesc.Texture2DArray.FirstArraySlice = i;
					HRESULT hr = d3d::s_device->CreateRenderTargetView(m_diffuseIrradianceCubemap.Get(), &m_RTVDIDesc, m_currentRTVs[i].ReleaseAndGetAddressOf());
					if (FAILED(hr)) {
						Logger::instance().logErr("ReflectionCapture::generateDiffuseIrradianceCubemap: Failed at RTV creation");
					}

					// ----- Set RTV ------
					d3d::s_devcon->OMSetRenderTargets(1, m_currentRTVs[i].GetAddressOf(), nullptr);
					float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
					d3d::s_devcon->ClearRenderTargetView(m_currentRTVs[i].Get(), clearColor);

					// ----- Render -----
					bindPipeline(m_diffuseIrradiancePipeline);

					m_worldToClipBuffer.getData().worldToClip = XMMatrixTranspose(CAMERA_CAPTURE_VIEWS[i] * PROJECTION);
					m_worldToClipBuffer.fill();

					d3d::s_devcon->VSSetConstantBuffers(0, 1, m_worldToClipBuffer.getBufferAddress());
					d3d::s_devcon->PSSetShaderResources(0, 1, tex::TextureManager::getInstance().getTexture(mapPath)->textureView.GetAddressOf());

					renderCube();

					// ----- Capture Result -----
					Microsoft::WRL::ComPtr<ID3D11Resource> rtvResource;
					m_currentRTVs[i]->GetResource(rtvResource.ReleaseAndGetAddressOf());
					DirectX::CaptureTexture(d3d::s_device, d3d::s_devcon, rtvResource.Get(), scratchImages[i]);
					images[i] = scratchImages[i].GetImages()[i];
				}
				// ----- Write to cubemap -----
				DirectX::ScratchImage diffuseIrradiaceCubeImage;
				diffuseIrradiaceCubeImage.InitializeCubeFromImages(images.data(), images.size());
				std::wstring filepath = util::stringToWstring(util::removeFileExt(mapPath)) + L"DI.dds";

				// TODO: Replace with proper saving later
				DirectX::SaveToDDSFile(
					diffuseIrradiaceCubeImage.GetImages(),
					diffuseIrradiaceCubeImage.GetImageCount(),
					diffuseIrradiaceCubeImage.GetMetadata(),
					DirectX::DDS_FLAGS_NONE,
					filepath.c_str()
				);
			}
		}
		void ReflectionCapture::generatePreFilteredSpecularCubemap()
		{
			initAndBindViewPort(PFS_TEXTURE_DIMENSION);

			for (const auto& mapPath : m_textureMapPaths) {

				std::array<DirectX::ScratchImage, 6> scratchImages;
				std::array<DirectX::Image, 6> images;
				for (uint32_t i = 0; i < 6; ++i) {
					// ----- Create RTV -----
					m_RTVPFSDesc.Texture2DArray.FirstArraySlice = i;

					for (int mipLevel = PFS_TEXTURE_MIPS - 1; mipLevel >= 0 ; --mipLevel) {

						initAndBindViewPort(PFS_TEXTURE_DIMENSION >> mipLevel);
						m_RTVPFSDesc.Texture2DArray.MipSlice = mipLevel;

						HRESULT hr = d3d::s_device->CreateRenderTargetView(m_preFilteredSpecularCubemap.Get(), &m_RTVPFSDesc, m_currentRTVs[i].ReleaseAndGetAddressOf());
						if (FAILED(hr)) {
							Logger::instance().logErr("ReflectionCapture::generatePreFilteredSpecularCubemap: Failed at RTV creation");
						}

						// ----- Set RTV ------
						d3d::s_devcon->OMSetRenderTargets(1, m_currentRTVs[i].GetAddressOf(), nullptr);
						float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
						d3d::s_devcon->ClearRenderTargetView(m_currentRTVs[i].Get(), clearColor);

						// ----- Render -----
						bindPipeline(m_preFilteredSpecularPipeline);

						m_worldToClipBuffer.getData().worldToClip = XMMatrixTranspose(CAMERA_CAPTURE_VIEWS[i] * PROJECTION);
						m_worldToClipBuffer.fill();

						d3d::s_devcon->VSSetConstantBuffers(0, 1, m_worldToClipBuffer.getBufferAddress());
						d3d::s_devcon->PSSetShaderResources(0, 1, tex::TextureManager::getInstance().getTexture(mapPath)->textureView.GetAddressOf());

						renderCube();
					}
					// ----- Capture Result -----
					Microsoft::WRL::ComPtr<ID3D11Resource> rtvResource;
					m_currentRTVs[i]->GetResource(rtvResource.ReleaseAndGetAddressOf());
					DirectX::CaptureTexture(d3d::s_device, d3d::s_devcon, rtvResource.Get(), scratchImages[i]);
					images[i] = scratchImages[i].GetImages()[0];
				}
				// ----- Write to cubemap -----
				DirectX::ScratchImage preFIlteredSpeculareCubeImage;
				preFIlteredSpeculareCubeImage.InitializeCubeFromImages(images.data(), images.size());
				std::wstring filepath = util::stringToWstring(util::removeFileExt(mapPath)) + L"PFS.dds";

				DirectX::ScratchImage mipchain;
				DirectX::GenerateMipMaps(preFIlteredSpeculareCubeImage.GetImages(), preFIlteredSpeculareCubeImage.GetImageCount(), preFIlteredSpeculareCubeImage.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 5, mipchain);

				// TODO: Replace with proper saving later
				HRESULT hr = DirectX::SaveToDDSFile(
					mipchain.GetImages(),
					mipchain.GetImageCount(),
					mipchain.GetMetadata(),
					DirectX::DDS_FLAGS_NONE,
					filepath.c_str()
				);

				if (FAILED(hr)) {
					Logger::instance().logErr("ReflectionCapture::generatePreFilteredSpecularCubemap: Failed at saving .dds" + std::system_category().message(hr));
				}
			}
		}
		void ReflectionCapture::generateBRDFIntegrationTexture()
		{
		}
	} // rend
} // engn