#include "MeshSystem.hpp"

namespace engn {
	namespace rend {
		void MeshSystem::init() {
			initPipelines();
			initNormalGroup();
			initDissolutionGroup();
			initHologramGroup();
			initEmissionGroup();
		}

		void MeshSystem::initNormalGroup()
		{
			m_normalGroup.setType(GroupTypes::NORMAL);
			m_normalGroup.init();
		}
		void MeshSystem::initDissolutionGroup()
		{
			m_dissolutionGroup.setType(GroupTypes::DISSOLUTION);
			m_dissolutionGroup.init();
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

			m_shadowSubSystem.bindDataAndBuffers();

			// Normal group 
			m_normalGroup.fillInstanceBuffer();
			this->bindPipelineViaType(PipelineTypes::NORMAL_RENDER);
			m_normalGroup.render();
			if (geomWidgetData.normalVisEnabled) {
				this->bindPipelineViaType(PipelineTypes::FACE_NORMAL_DEBUG);
				m_normalGroup.render();
			}
			if (geomWidgetData.vireframeVisEnabled) {
				this->bindPipelineViaType(PipelineTypes::WIREFRAME_DEBUG);
				m_normalGroup.render();
			}

			// Dissolution group
			m_dissolutionGroup.fillInstanceBuffer();
			this->bindPipelineViaType(PipelineTypes::DISSOLUTION_RENDER);
			m_dissolutionGroup.render();

			m_shadowSubSystem.unbindDepthBuffers();

		}

		void MeshSystem::renderDepth2D()
		{
			// TODO: THIS
			if (!shadowSystemInitialized) {
				m_shadowSubSystem.init();
				shadowSystemInitialized = true;
			}

			for (uint32_t i = 0; i < m_shadowSubSystem.getDirectionalLightShadowMaps().size(); ++i) {
				m_shadowSubSystem.captureDirectionalShadow(i);

				m_normalGroup.fillInstanceBuffer();
				m_normalGroup.render();
				m_dissolutionGroup.fillInstanceBuffer();
				m_dissolutionGroup.render();
			}

			m_shadowSubSystem.captureSpotShadow();
			m_normalGroup.fillInstanceBuffer();
			m_normalGroup.render();
			m_dissolutionGroup.fillInstanceBuffer();
			m_dissolutionGroup.render();
		}

		void MeshSystem::renderDepthCubemaps()
		{
			// TODO: THIS
			if (!shadowSystemInitialized) {
				m_shadowSubSystem.init();
				shadowSystemInitialized = true;
			}

			m_shadowSubSystem.fillPointMatrices();
			for (uint32_t i = 0; i < m_shadowSubSystem.getPointLightShadowMaps().size(); ++i) {
				m_shadowSubSystem.capturePointShadow(i);

				m_normalGroup.fillInstanceBuffer();
				m_normalGroup.render();
				m_dissolutionGroup.fillInstanceBuffer();
				m_dissolutionGroup.render();
			}
		}

		std::pair<uint32_t, InstanceProperties> MeshSystem::addNormalInstance(std::shared_ptr<mdl::Model> mod, const Material& mtrl, const Instance& inc)
		{
			return m_normalGroup.addModel(mod, mtrl, inc);
		}

		std::pair<uint32_t, InstanceProperties> MeshSystem::addDissolutionInstance(std::shared_ptr<mdl::Model> mod, const Material& mtrl, const InstanceDissolution& inc)
		{
			return m_dissolutionGroup.addModel(mod, mtrl, inc);
		}

		void MeshSystem::removeDissolutionInstance(const InstanceProperties& instanceData)
		{
			m_dissolutionGroup.removeInstance(instanceData);
		}

		std::pair<uint32_t, InstanceProperties> MeshSystem::addHologramInstance(std::shared_ptr<mdl::Model> mod, const Material& mtrl, const Instance& inc)
		{
			return m_hologramGroup.addModel(mod, mtrl, inc);
		}

		std::pair<uint32_t, InstanceProperties> MeshSystem::addEmissionInstance(std::shared_ptr<mdl::Model> mod, const Material& mtrl, const Instance& inc)
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

				if (type == DISSOLUTION_RENDER) {
					data.blendDesc.BlendEnable = true;
					data.blendDesc.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
					data.blendDesc.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
					data.blendDesc.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
					data.blendDesc.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
					data.blendDesc.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
					data.blendDesc.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
					data.blendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;
				}
				else {
					data.blendDesc.BlendEnable = false;
					data.blendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;
				}

				initPipeline(m_pipelines[type], data);
			}
		}
		void MeshSystem::bindPipelineViaType(PipelineTypes pipelineType)
		{
			bindPipeline(m_pipelines[pipelineType]);
		}
	} // rend
} // engn