#include "ShadowSubSystem.hpp"

#include "render/Systems/LightSystem.hpp"
#include "render/Systems/MeshSystem.hpp"

namespace engn {
	namespace rend {
		void ShadowSubSystem::init()
		{
			initDepthBuffers();
			initPipelines();
			initBuffers();
			fillDirectionalMatrices();
		}
		void ShadowSubSystem::captureDirectionalShadow(uint32_t idx)
		{
			initAndBindViewPort(SHADOW_MAP_RESOLUTION2D);

			d3d::s_devcon->OMSetRenderTargets(
				0,
				nullptr,
				m_directionalShadowMaps[idx].getDSVPtr()
			);
			m_directionalShadowMaps[idx].clear();
			bindPipeline(m_shadow2DPipeline);

			m_shadow2DVSCB.getData().worldToClip = XMMatrixTranspose(m_directionalViewProjMatrices[idx]);
			m_shadow2DVSCB.fill();

			d3d::s_devcon->VSSetConstantBuffers(0, 1, m_shadow2DVSCB.getBufferAddress());
		}

		void ShadowSubSystem::captureSpotShadow()
		{
			fillSpotMatrices();
			initAndBindViewPort(SHADOW_MAP_RESOLUTION2D);

			d3d::s_devcon->OMSetRenderTargets(
				0,
				nullptr,
				m_spotShadowMap.getDSVPtr()
			);
			m_spotShadowMap.clear();
			//d3d::s_devcon->OMSetRenderTargets(1, nullptr, m_directionalLightShadowMap.getDSVPtr());
			bindPipeline(m_shadow2DPipeline);

			m_shadow2DVSCB.getData().worldToClip = XMMatrixTranspose(m_spotlightViewProjMatrix);
			m_shadow2DVSCB.fill();

			d3d::s_devcon->VSSetConstantBuffers(0, 1, m_shadow2DVSCB.getBufferAddress());
		}
		void ShadowSubSystem::bindDataAndBuffers()
		{
			//! Matrices
			for (uint32_t i = 0; i < m_directionalViewProjMatrices.size(); ++i) {
				m_shadowMapProjectionsPSCB.getData().dirLightViewProj[i] = XMMatrixTranspose(m_directionalViewProjMatrices[i]);
			}

			m_shadowMapProjectionsPSCB.getData().spotLightViewProj = XMMatrixTranspose(m_spotlightViewProjMatrix);
			m_shadowMapProjectionsPSCB.getData().texelSizeClipSpaceDirectionalMap = { 1.0f / static_cast<float>(SHADOW_MAP_RESOLUTION2D) };
			m_shadowMapProjectionsPSCB.fill();

			d3d::s_devcon->PSSetConstantBuffers(SHADOW_MAP_MATRICES_BUFFER_SLOT, 1, m_shadowMapProjectionsPSCB.getBufferAddress());

			//! Depth buffers
			m_spotShadowMap.bindSRV(SPOT_SHADOW_MAP_SLOT);
			for (uint32_t i = 0; i < m_directionalShadowMaps.size(); ++i) {
				m_directionalShadowMaps[i].bindSRV(DIRECTIONAL_SHADOW_MAP_SLOT + i);
			}
		}
		void ShadowSubSystem::unbindDepthBuffers()
		{
			ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
			// TODO: I think this can be done via 1 call
			d3d::s_devcon->PSSetShaderResources(SPOT_SHADOW_MAP_SLOT, 1, nullSRV);
			for (uint32_t i = 0; i < m_directionalShadowMaps.size(); ++i) {
				d3d::s_devcon->PSSetShaderResources(DIRECTIONAL_SHADOW_MAP_SLOT + i, 1, nullSRV);
			}
		}
		std::vector<BindableDepthBuffer>& ShadowSubSystem::getDirectionalLightShadowMaps()
		{
			return m_directionalShadowMaps;
		}
		void ShadowSubSystem::initDepthBuffers()
		{
			auto& lightSystem = LightSystem::getInstance();
			for (uint32_t i = 0; i < lightSystem.getDirectionalLights().size(); ++i) {
				m_directionalShadowMaps.emplace_back();
				m_directionalShadowMaps[i].init(
					SHADOW_MAP_RESOLUTION2D,
					SHADOW_MAP_RESOLUTION2D,
					DXGI_FORMAT_R24G8_TYPELESS,
					DXGI_FORMAT_D24_UNORM_S8_UINT,
					DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
					false
				);
			}
			m_spotShadowMap.init(
				SHADOW_MAP_RESOLUTION2D,
				SHADOW_MAP_RESOLUTION2D,
				DXGI_FORMAT_R24G8_TYPELESS,
				DXGI_FORMAT_D24_UNORM_S8_UINT,
				DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
				false
			);
		}
		void ShadowSubSystem::initPipelines()
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

			PipelineData shadow2DPipelineData{
				MeshSystem::getInstance().getDefaultLayoutPtr(),
				MeshSystem::getInstance().getDefaultLayoutArraySize(),
				D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				util::getExeDirW() + L"VS2DShadow.cso",
				L"", L"", L"", L"",
				rasterizerDesc,
				depthStencilStateDesc
			};

			initPipeline(m_shadow2DPipeline, shadow2DPipelineData);
		}
		void ShadowSubSystem::initBuffers()
		{
			m_shadow2DVSCB.init();
			m_shadowMapProjectionsPSCB.init();
		}
		void ShadowSubSystem::fillDirectionalMatrices()
		{
			auto& lightSystem = LightSystem::getInstance();
			for (auto& dirLight : lightSystem.getDirectionalLights()) {
				m_directionalViewProjMatrices.push_back(
					XMMatrixLookAtLH(OBJECT_CENTER + 15 * -dirLight.direction, OBJECT_CENTER, { 0.0f, 1.0f, 0.0f }) *
					DIRECTIONAL_PROJECTION
				);
			}
		}
		void ShadowSubSystem::fillPointMatrices()
		{
		}
		void ShadowSubSystem::fillSpotMatrices()
		{
			auto& lightSystem = LightSystem::getInstance();
			auto& spotLight = lightSystem.getSpotLight();
			m_spotlightViewProjMatrix =
				XMMatrixLookAtLH(spotLight.position, spotLight.position + spotLight.direction, { 0.0f, 1.0f, 0.0f }) *
				XMMatrixPerspectiveFovLH(XMVectorGetX(spotLight.cutoffAngle) * 2, 1.0f, 1000.0f, 0.1f);
		}
		void ShadowSubSystem::initAndBindViewPort(uint32_t resolution)
		{
			D3D11_VIEWPORT viewPort;
			viewPort.TopLeftX = 0;
			viewPort.TopLeftY = 0;
			viewPort.Width = resolution;
			viewPort.Height = resolution;
			// It is set this way, despite the reversed depth matrix
			viewPort.MinDepth = 0.0f;
			viewPort.MaxDepth = 1.0f;

			d3d::s_devcon->RSSetViewports(1, &viewPort);
		}
	} // rend
} // engn