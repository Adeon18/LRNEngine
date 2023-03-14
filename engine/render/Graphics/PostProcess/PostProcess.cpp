#include <string>

#include "PostProcess.hpp"

#include "utils/Logger/Logger.hpp"

#include "input/Keyboard.hpp"


namespace engn {
	namespace rend {
		void PostProcess::init()
		{
			// Init Pipeline
			const std::wstring exeDirW = util::getExeDirW();
			PipelineData pipelineData{
				nullptr,
				0,
				D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				exeDirW + VS_NAME,
				L"", L"", L"",
				exeDirW + PS_NAME
			};
			initPipeline(m_pipeline, pipelineData);

			m_cbuffer.init();

			m_initialized = true;
		}
		void PostProcess::ressolve(const HDRRenderTarget& src)
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