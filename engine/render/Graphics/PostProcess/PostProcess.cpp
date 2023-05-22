#include <string>

#include "PostProcess.hpp"

#include "utils/Logger/Logger.hpp"

#include "input/Keyboard.hpp"


namespace engn {
	namespace rend {
		void PostProcess::init()
		{
			//! Depth stencil state
			D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc{};
			depthStencilStateDesc.DepthEnable = true;
			depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER_EQUAL;
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
			PipelineData pipelineData{
				nullptr,
				0,
				D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				exeDirW + VS_NAME,
				L"", L"", L"",
				exeDirW + PS_NAME,
				rasterizerDesc,
				depthStencilStateDesc,
				blendDesc
			};
			initPipeline(m_pipeline, pipelineData);

			m_cbuffer.init();

			m_initialized = true;
		}
		void PostProcess::ressolve(const BindableRenderTarget& src)
		{
			if (!m_initialized) {
				Logger::instance().logWarn("SkyTriangle::The texture for a TextureCube is not bound");
				return;
			}
			// Check if EV100 was changed
			handleEV100Adjustment();

			// Turn off IL
			d3d::s_devcon->IASetInputLayout(NULL);

			// Handle textures and buffers
			src.bindSRV(0);
			m_cbuffer.getData().EV100 = { m_ev100Exposure, m_ev100Exposure, m_ev100Exposure, m_ev100Exposure };
			m_cbuffer.fill();
			d3d::s_devcon->PSSetConstantBuffers(0, 1, m_cbuffer.getBufferAddress());

			// Bind the pipeline
			bindPipeline(m_pipeline);
			// Draw call
			d3d::s_devcon->Draw(3, 0);
			// Clear the SRV to allow it to be bound next frame
			ID3D11ShaderResourceView* nullSRV = nullptr;
			d3d::s_devcon->PSSetShaderResources(0, 1, &nullSRV);
		}
		void PostProcess::handleEV100Adjustment()
		{
			if (inp::Keyboard::getInstance().isKeyJustPressed(inp::Keyboard::Keys::KEY_PLUS)) {
				m_ev100Exposure += EV100_CHANGE_VALUE;
				Logger::instance().logDebug("PostProcess::handleEV100Adjustment: New value: " + std::to_string(m_ev100Exposure));
			}
			else if (inp::Keyboard::getInstance().isKeyJustPressed(inp::Keyboard::Keys::KEY_MINUS)) {
				m_ev100Exposure -= EV100_CHANGE_VALUE;
				Logger::instance().logDebug("PostProcess::handleEV100Adjustment: New value: " + std::to_string(m_ev100Exposure));
			}
			return;
		}
	} // rend
} // engn