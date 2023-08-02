#include "DeferredRessolver.hpp"

#include "render/Systems/MeshSystem.hpp"

namespace engn {
	namespace rend {
		void DeferredRessolver::init()
		{
			initPipelines();
		}
		void DeferredRessolver::ressolve(const GBuffer& gBuf)
		{
			// Turn off IL
			d3d::s_devcon->IASetInputLayout(NULL);

			// Handle textures and buffers
			gBuf.albedo.bindSRV(0);
			gBuf.normals.bindSRV(1);
			gBuf.roughMet.bindSRV(2);
			gBuf.emission.bindSRV(3);
			gBuf.ids.bindSRV(4);

			// Bind the pipeline
			bindPipeline(m_pbrPipeline);
			d3d::s_devcon->Draw(3, 0);

			bindPipeline(m_emissionPipeline);
			d3d::s_devcon->Draw(3, 0);
			// Clear the SRV to allow it to be bound next frame
			ID3D11ShaderResourceView* nullSRV[5] = { nullptr, nullptr, nullptr, nullptr, nullptr };
			d3d::s_devcon->PSSetShaderResources(0, 5, nullSRV);
		}
		void DeferredRessolver::initPipelines()
		{
			//! Depth stencil state
			D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc{};
			depthStencilStateDesc.DepthEnable = false;

			depthStencilStateDesc.StencilEnable = true;
			depthStencilStateDesc.StencilReadMask = 0xFF;
			depthStencilStateDesc.StencilWriteMask = 0xFF;
			depthStencilStateDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
			depthStencilStateDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depthStencilStateDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depthStencilStateDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			depthStencilStateDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
			depthStencilStateDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depthStencilStateDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depthStencilStateDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

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
			PipelineData pipelineDataPBR{
				nullptr,
				0,
				D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				exeDirW + L"VSDeferred.cso",
				L"", L"", L"",
				exeDirW + L"PSDeferredPBR.cso",
				rasterizerDesc,
				depthStencilStateDesc,
				blendDesc,
				MeshSystem::PBR_STENCIL_REF
			};
			initPipeline(m_pbrPipeline, pipelineDataPBR);

			PipelineData pipelineDataEmission{
				nullptr,
				0,
				D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				exeDirW + L"VSDeferred.cso",
				L"", L"", L"",
				exeDirW + L"PSDeferredEmission.cso",
				rasterizerDesc,
				depthStencilStateDesc,
				blendDesc,
				MeshSystem::EMISSION_STENCIL_REF
			};
			initPipeline(m_emissionPipeline, pipelineDataEmission);
		}
	} // rend
} // engn