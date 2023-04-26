#include "MeshSystem.hpp"

namespace engn {
	namespace rend {
		void MeshSystem::init() {
			initPipelines();
			initNormalGroup();
			initHologramGroup();
			initEmissionGroup();
			initShadowMapData();

			m_directionalLightShadowMap.init(1024, 1024, DXGI_FORMAT_R24G8_TYPELESS, DXGI_FORMAT_D24_UNORM_S8_UINT, DXGI_FORMAT_R24_UNORM_X8_TYPELESS);
			m_shadowGenRTV.init(1024, 1024, DXGI_FORMAT_R16G16B16A16_FLOAT);
			m_dirLightVSCB.init();
		}

		void MeshSystem::initNormalGroup()
		{
			m_normalGroup.setType(GroupTypes::NORMAL);
			m_normalGroup.init();

		}
		void MeshSystem::initHologramGroup()
		{
			m_hologramGroup.setType(GroupTypes::HOLOGRAM);
			m_hologramGroup.init();
		}

		void MeshSystem::initEmissionGroup()
		{
			m_emissionOnlyGroup.setType(GroupTypes::EMISSION_ONLY);
			m_emissionOnlyGroup.init();
		}
		
		void MeshSystem::render(const RenderModeFlags& flags)
		{
			auto& geomWidgetData = UI::instance().getGeomWidgetData();
			// Normal group 
			m_normalGroup.fillInstanceBuffer();
			this->bindPipelineViaType(PipelineTypes::NORMAL_RENDER);
			m_normalGroup.render();
			if (geomWidgetData.normalVisEnabled){
				this->bindPipelineViaType(PipelineTypes::FACE_NORMAL_DEBUG);
				m_normalGroup.render();
			}
			if (geomWidgetData.vireframeVisEnabled) {
				this->bindPipelineViaType(PipelineTypes::WIREFRAME_DEBUG);
				m_normalGroup.render();
			}

			// Hologram group
			m_hologramGroup.fillInstanceBuffer();
			this->bindPipelineViaType((geomWidgetData.isDebugVisEnabled()) ? PipelineTypes::NORMAL_RENDER: PipelineTypes::HOLOGRAM_RENDER);
			m_hologramGroup.render();
			if (geomWidgetData.normalVisEnabled) {
				this->bindPipelineViaType(PipelineTypes::FACE_NORMAL_DEBUG);
				m_hologramGroup.render();
			}
			if (geomWidgetData.vireframeVisEnabled) {
				this->bindPipelineViaType(PipelineTypes::WIREFRAME_DEBUG);
				m_hologramGroup.render();
			}

			// Emission Only group
			m_emissionOnlyGroup.fillInstanceBuffer();
			this->bindPipelineViaType(PipelineTypes::EMISSION_ONLY_RENDER);
			m_emissionOnlyGroup.render();
			if (geomWidgetData.normalVisEnabled) {
				this->bindPipelineViaType(PipelineTypes::FACE_NORMAL_DEBUG);
				m_emissionOnlyGroup.render();
			}
			if (geomWidgetData.vireframeVisEnabled) {
				this->bindPipelineViaType(PipelineTypes::WIREFRAME_DEBUG);
				m_emissionOnlyGroup.render();
			}
		}

		void MeshSystem::renderDepth2D()
		{
			D3D11_VIEWPORT viewPort;
			viewPort.TopLeftX = 0;
			viewPort.TopLeftY = 0;
			viewPort.Width = 1024;
			viewPort.Height = 1024;
			// It is set this way, despite the reversed depth matrix
			viewPort.MinDepth = 0.0f;
			viewPort.MaxDepth = 1.0f;

			d3d::s_devcon->RSSetViewports(1, &viewPort);
			m_directionalLightShadowMap.clear();
			m_shadowGenRTV.OMSetCurrent(m_directionalLightShadowMap.getDSVPtr());
			//d3d::s_devcon->OMSetRenderTargets(1, nullptr, m_directionalLightShadowMap.getDSVPtr());
			m_normalGroup.fillInstanceBuffer();
			bindPipeline(m_shadowPipeline);
			d3d::s_devcon->PSSetShader(NULL, NULL, 0);

			const XMMATRIX PROJECTION = XMMatrixOrthographicLH(50, 50, 1000.0f, 0.1f);
			XMVECTOR lightDirection = { 0.0f, -0.8f, 0.6f };
			XMVECTOR worldCenter = { 0.0f, 0.0f, 0.0f };
			const XMMATRIX VIEW = XMMatrixLookAtLH(worldCenter + 20 * -lightDirection, { 0.0f,  0.0f,  0.0f }, { 0.0f, 1.0f, 0.0f });
			
			m_dirLightVSCB.getData().worldToClip = XMMatrixTranspose(VIEW * PROJECTION);
			m_dirLightVSCB.getData().worldToClipInv = XMMatrixInverse(nullptr, VIEW * PROJECTION);

			m_dirLightVSCB.fill();
			d3d::s_devcon->VSSetConstantBuffers(0, 1, m_dirLightVSCB.getBufferAddress());

			m_normalGroup.render();
		}

		uint32_t MeshSystem::addNormalInstance(std::shared_ptr<mdl::Model> mod, const Material& mtrl, const Instance& inc)
		{
			return m_normalGroup.addModel(mod, mtrl, inc);
		}


		uint32_t MeshSystem::addHologramInstance(std::shared_ptr<mdl::Model> mod, const Material& mtrl, const Instance& inc)
		{
			return m_hologramGroup.addModel(mod, mtrl, inc);
		}

		uint32_t MeshSystem::addEmissionInstance(std::shared_ptr<mdl::Model> mod, const Material& mtrl, const Instance& inc)
		{
			return m_emissionOnlyGroup.addModel(mod, mtrl, inc);
		}

		void MeshSystem::addInstanceOffset(const InstanceProperties& instanceData, const XMVECTOR& offset)
		{
			switch (instanceData.group) {
			case GroupTypes::NORMAL:
			{
				m_normalGroup.addModelOffset(instanceData, offset);
			}
			break;
			case GroupTypes::HOLOGRAM:
			{
				m_hologramGroup.addModelOffset(instanceData, offset);
			}
			break;
			case GroupTypes::EMISSION_ONLY: {
				m_emissionOnlyGroup.addModelOffset(instanceData, offset);
			}
			}
		}

		void MeshSystem::addInstanceRotation(const InstanceProperties& instanceData, const XMVECTOR& rotation)
		{
			switch (instanceData.group) {
			case GroupTypes::NORMAL:
			{
				m_normalGroup.addModelRotation(instanceData, rotation);
			}
			break;
			case GroupTypes::HOLOGRAM:
			{
				m_hologramGroup.addModelRotation(instanceData, rotation);
			}
			}
		}

		std::pair<bool, InstanceProperties> MeshSystem::getClosestMesh(geom::Ray& ray, geom::MeshIntersection& nearest) {
			InstanceProperties i2d{};

			// TODO: Like this for now, haven't found the idea for organizing it in any other way yet
			bool collided = false;
			std::vector<bool> collisionResults;
			collisionResults.push_back(m_normalGroup.checkRayIntersection(ray, nearest, i2d));
			collisionResults.push_back(m_hologramGroup.checkRayIntersection(ray, nearest, i2d));
			collisionResults.push_back(m_emissionOnlyGroup.checkRayIntersection(ray, nearest, i2d));

			if (std::any_of(collisionResults.begin(), collisionResults.end(), [](bool v) { return v; })) {
				collided = true;
			}

			return std::pair<bool, InstanceProperties>{collided, i2d};
		}
		void MeshSystem::initPipelines()
		{
			for (auto& [type, data] : PIPELINE_TYPE_DATA) {
				// Fill the rasterized and stencil data before init
				data.rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
				data.rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

				data.depthStencilDesc.DepthEnable = true;
				data.depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
				data.depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER_EQUAL;

				initPipeline(m_pipelines[type], data);
			}
		}
		void MeshSystem::initShadowMapData()
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

			// Init Pipelines
			const std::wstring exeDirW = util::getExeDirW();

			// Diffuse Irradiance
			PipelineData shadowMapPipelineData{
				DEFAULT_LAYOUT,
				ARRAYSIZE(DEFAULT_LAYOUT),
				D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				exeDirW + L"VSBasicColor.cso",
				L"", L"", L"", L"",
				rasterizerDesc,
				depthStencilStateDesc
			};

			initPipeline(m_shadowPipeline, shadowMapPipelineData);

		}
		void MeshSystem::bindPipelineViaType(PipelineTypes pipelineType)
		{
			bindPipeline(m_pipelines[pipelineType]);
		}
	} // rend
} // engn