#include "BloomRessolver.hpp"

// Temporary, ubtil I add window resize support
#include "include/config.hpp"

namespace engn {
	namespace rend {
		void BloomRessolver::init()
		{
			initPipelines();
			initTextures();
		}
		void BloomRessolver::downSampleAndBlur(const BindableRenderTarget& src)
		{
			for (int mipLevel = 4; mipLevel >= 0; --mipLevel) {
				D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
				rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
				rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				// Handle viewports for different sizes of mips
				initAndBindViewPort(WIN_WIDTH_DEF >> mipLevel, WIN_HEIGHT_DEF >> mipLevel);
				rtvDesc.Texture2D.MipSlice = mipLevel;

				HRESULT hr = d3d::s_device->CreateRenderTargetView(m_bluredTextureStorage.Get(), &rtvDesc, m_bluredTextureRTV.ReleaseAndGetAddressOf());
				if (FAILED(hr)) {
					Logger::instance().logErr("ReflectionCapture::generatePreFilteredSpecularCubemap: Failed at RTV creation");
				}

				// ----- Set RTV ------
				d3d::s_devcon->OMSetRenderTargets(1, m_bluredTextureRTV.GetAddressOf(), nullptr);
				float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
				d3d::s_devcon->ClearRenderTargetView(m_bluredTextureRTV.Get(), clearColor);

				src.bindSRV(0);

				// ----- Render -----
				bindPipeline(m_downsamplePipeline);
				d3d::s_devcon->Draw(3, 0);
				// Clear the SRV to allow it to be bound next frame
				ID3D11ShaderResourceView* nullSRV = nullptr;
				d3d::s_devcon->PSSetShaderResources(0, 1, &nullSRV);
			}
		}
		void BloomRessolver::upSampleAndBlur()
		{
		}

		void BloomRessolver::initAndBindViewPort(uint32_t dimensionX, uint32_t dimensionY)
		{
			D3D11_VIEWPORT viewPort;
			viewPort.TopLeftX = 0;
			viewPort.TopLeftY = 0;
			viewPort.Width = dimensionX;
			viewPort.Height = dimensionY;
			// It is set this way, despite the reversed depth matrix
			viewPort.MinDepth = 0.0f;
			viewPort.MaxDepth = 1.0f;

			d3d::s_devcon->RSSetViewports(1, &viewPort);
		}

		void BloomRessolver::initPipelines()
		{
			//! Depth stencil state
			D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc{};
			depthStencilStateDesc.DepthEnable = false;
			depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;
			//! Rasterizer state
			D3D11_RASTERIZER_DESC rasterizerDesc{};
			rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
			rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

			//! Blend State Desc: MAYBE TODO: Move to some DEFAULT::DISABLED constant or something
			D3D11_RENDER_TARGET_BLEND_DESC blendDesc{};
			blendDesc.BlendEnable = false;
			blendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

			// Init Pipeline
			const std::wstring exeDirW = util::getExeDirW();
			PipelineData pipelineDataDownSample{
				nullptr,
				0,
				D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				exeDirW + VS_BLOOM,
				L"", L"", L"",
				exeDirW + PS_DOWNSAMPLE,
				rasterizerDesc,
				depthStencilStateDesc,
				blendDesc
			};
			initPipeline(m_downsamplePipeline, pipelineDataDownSample);

			PipelineData pipelineDataUpSample{
				nullptr,
				0,
				D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				exeDirW + VS_BLOOM,
				L"", L"", L"",
				exeDirW + PS_UPSAMPLE,
				rasterizerDesc,
				depthStencilStateDesc,
				blendDesc
			};
			initPipeline(m_upsamplePipeline, pipelineDataUpSample);
		}
		void BloomRessolver::initTextures()
		{
			D3D11_TEXTURE2D_DESC textureDesc{};
			textureDesc.Width = WIN_WIDTH_DEF;
			textureDesc.Height = WIN_HEIGHT_DEF;
			textureDesc.MipLevels = 5;
			textureDesc.ArraySize = 1;
			textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			textureDesc.CPUAccessFlags = 0;

			HRESULT hr = d3d::s_device->CreateTexture2D(&textureDesc, nullptr, m_bluredTextureStorage.ReleaseAndGetAddressOf());
			if (FAILED(hr)) {
				Logger::instance().logErr("BloomRessolver::initTextures: Failed at texture creation");
				return;
			}
		}
	} // rend
} // engn