#include "MeshSystem.hpp"

namespace engn {
	namespace rend {
		void MeshSystem::init() {
			initPipelines();
			initNormalGroup();
			initHologramGroup();
			initEmissionGroup();
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
			m_shadowSubSystem.bindDataAndBuffers();

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

			m_shadowSubSystem.unbindDepthBuffers();

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
			// TODO: THIS
			if (!shadowSystemInitialized) {
				m_shadowSubSystem.init();
				shadowSystemInitialized = true;
			}

			for (uint32_t i = 0; i < m_shadowSubSystem.getDirectionalLightShadowMaps().size(); ++i) {
				m_shadowSubSystem.captureDirectionalShadow(i);

				m_normalGroup.fillInstanceBuffer();
				m_normalGroup.render();
			}

			m_shadowSubSystem.captureSpotShadow();
			m_normalGroup.fillInstanceBuffer();
			m_normalGroup.render();
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
			}
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
		void MeshSystem::bindPipelineViaType(PipelineTypes pipelineType)
		{
			bindPipeline(m_pipelines[pipelineType]);
		}
	} // rend
} // engn