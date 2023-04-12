#pragma once

#include <array>

#include <DirectXTex/DirectXTex.h>

#include "ReflectionCapture.hpp"

#include "utils/Logger/Logger.hpp"

namespace engn {
	namespace rend {
		void ReflectionCapture::init(const std::string& skyTexturePath)
		{
			initCubeBuffers();
			initPipelines();

			m_worldToClipBuffer.init();
			m_skyBoxTexture = tex::TextureManager::getInstance().getTexture(skyTexturePath);

			Logger::instance().logDebug("HOLY FUCK IT WORKS");
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
			depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER_EQUAL;
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
			cubemapTextureDesc.Width = 256;
			cubemapTextureDesc.Height = 256;
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
		}
		void ReflectionCapture::renderCube()
		{
			m_cubeVertexBuffer.bind();
			m_cubeIndexBuffer.bind();

			d3d::s_devcon->DrawIndexed(36, 0, 0);
		}
		void ReflectionCapture::generateDiffuseIrradianceCubemap(const XMMATRIX& projection)
		{

			std::array<DirectX::ScratchImage, 6> scratchImages;
			std::array<DirectX::Image, 6> images;

			// Create the render target view for the cubemap texture
			D3D11_RENDER_TARGET_VIEW_DESC rtvDIDesc;
			rtvDIDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			rtvDIDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			rtvDIDesc.Texture2DArray.MipSlice = 0;
			rtvDIDesc.Texture2DArray.ArraySize = 1;

			for (uint32_t i = 0; i < 6; ++i) {
				// ----- Create RTV -----
				rtvDIDesc.Texture2DArray.FirstArraySlice = i;
				HRESULT hr = d3d::s_device->CreateRenderTargetView(m_diffuseIrradianceCubemap.Get(), &rtvDIDesc, m_currentDIRTV.ReleaseAndGetAddressOf());
				if (FAILED(hr)) {
					Logger::instance().logErr("ReflectionCapture::generateDiffuseIrradianceCubemap: Failed at RTV creation");
				}

				// ----- Set RTV ------
				d3d::s_devcon->OMSetRenderTargets(1, m_currentDIRTV.GetAddressOf(), nullptr);
				float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
				d3d::s_devcon->ClearRenderTargetView(m_currentDIRTV.Get(), clearColor);

				// ----- Render -----
				bindPipeline(m_diffuseIrradiancePipeline);

				m_worldToClipBuffer.getData().worldToClip = projection;
				m_worldToClipBuffer.fill();

				d3d::s_devcon->VSSetConstantBuffers(0, 1, m_worldToClipBuffer.getBufferAddress());
				d3d::s_devcon->PSSetShaderResources(0, 1, m_skyBoxTexture->textureView.GetAddressOf());

				renderCube();

				Microsoft::WRL::ComPtr<ID3D11Resource> rtvResource;
				m_currentDIRTV->GetResource(rtvResource.GetAddressOf());
				DirectX::CaptureTexture(d3d::s_device, d3d::s_devcon, rtvResource.Get(), scratchImages[i]);
				images[i] = scratchImages[i].GetImages()[0];
			}
			DirectX::ScratchImage diffuseIrradiaceCubeImage;
			diffuseIrradiaceCubeImage.InitializeCubeFromImages(images.data(), images.size());

			std::wstring filepath = util::getExeDirW() + L"AAAA.dds";

			DirectX::SaveToDDSFile(
				diffuseIrradiaceCubeImage.GetImages(),
				diffuseIrradiaceCubeImage.GetImageCount(),
				diffuseIrradiaceCubeImage.GetMetadata(),
				DirectX::DDS_FLAGS_NONE,
				filepath.c_str()
			);

		}
		void ReflectionCapture::generatePreFilteredSpecularCubemap(const XMMATRIX& projection)
		{
		}
		void ReflectionCapture::generateBRDFIntegrationTexture()
		{
		}
	} // rend
} // engn