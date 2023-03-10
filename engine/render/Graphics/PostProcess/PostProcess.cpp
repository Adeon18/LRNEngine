#include "PostProcess.hpp"

#include "utils/Logger/Logger.hpp"


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
		void PostProcess::ressolve(const HDRRenderTarget& src, const LDRRenderTarget& dest)
		{
			if (!m_initialized) {
				Logger::instance().logWarn("SkyTriangle::The texture for a TextureCube is not bound");
				return;
			}
			// Turn off IL
			d3d::s_devcon->IASetInputLayout(NULL);
			// Unbind prev stuff

			src.bindSRV(0);

			// Bind the pipeline
			bindPipeline(m_pipeline);
			// Draw call
			d3d::s_devcon->Draw(3, 0);
			// Clear the SRV to allow it to be bound next frame
			ID3D11ShaderResourceView* nullSRV = nullptr;
			d3d::s_devcon->PSSetShaderResources(0, 1, &nullSRV);
		}
	} // rend
} // engn