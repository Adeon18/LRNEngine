#include "MeshSystem.hpp"

namespace engn {
	namespace rend {
		void MeshSystem::init() {
			initPipelines();
			initNormalGroup();
			initDissolutionGroup();
			initIncinerationGroup();
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
		void MeshSystem::initIncinerationGroup()
		{
			m_incinerationGroup.setType(GroupTypes::INCINERATION);
			m_incinerationGroup.init();
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
			/*auto& geomWidgetData = UI::instance().getGeomWidgetData();

			//// Hologram group
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

			m_shadowSubSystem.unbindDepthBuffers();*/

		}

		void MeshSystem::renderEmission(const RenderModeFlags& flags)
		{
			auto& geomWidgetData = UI::instance().getGeomWidgetData();

			//// Hologram group
			m_hologramGroup.fillInstanceBuffer();
			this->bindPipelineViaType((geomWidgetData.isDebugVisEnabled()) ? PipelineTypes::NORMAL_RENDER : PipelineTypes::HOLOGRAM_RENDER);
			m_hologramGroup.render();
			// TODO: Disabled for now
			/*if (geomWidgetData.normalVisEnabled) {
				this->bindPipelineViaType(PipelineTypes::FACE_NORMAL_DEBUG);
				m_hologramGroup.render();
			}
			if (geomWidgetData.vireframeVisEnabled) {
				this->bindPipelineViaType(PipelineTypes::WIREFRAME_DEBUG);
				m_hologramGroup.render();
			}*/

			// Emission Only group
			m_emissionOnlyGroup.fillInstanceBuffer();
			this->bindPipelineViaType(PipelineTypes::EMISSION_ONLY_RENDER);
			m_emissionOnlyGroup.render();
			// TODO: Disabled for now
			/*if (geomWidgetData.normalVisEnabled) {
				this->bindPipelineViaType(PipelineTypes::FACE_NORMAL_DEBUG);
				m_emissionOnlyGroup.render();
			}
			if (geomWidgetData.vireframeVisEnabled) {
				this->bindPipelineViaType(PipelineTypes::WIREFRAME_DEBUG);
				m_emissionOnlyGroup.render();
			}*/
		}

		void MeshSystem::renderPBR(const RenderModeFlags& flags)
		{
			auto& geomWidgetData = UI::instance().getGeomWidgetData();

			// Normal group 
			m_normalGroup.fillInstanceBuffer();
			this->bindPipelineViaType(PipelineTypes::NORMAL_RENDER);
			m_normalGroup.render();
			// TODO: Disabled for now
			/*if (geomWidgetData.normalVisEnabled) {
				this->bindPipelineViaType(PipelineTypes::FACE_NORMAL_DEBUG);
				m_normalGroup.render();
			}
			if (geomWidgetData.vireframeVisEnabled) {
				this->bindPipelineViaType(PipelineTypes::WIREFRAME_DEBUG);
				m_normalGroup.render();
			}*/

			// Dissolution group
			m_dissolutionGroup.fillInstanceBuffer();
			this->bindPipelineViaType(PipelineTypes::DISSOLUTION_RENDER);
			m_dissolutionGroup.render();

			// Dissolution group
			m_incinerationGroup.fillInstanceBuffer();
			this->bindPipelineViaType(PipelineTypes::INCINERATION_RENDER);
			m_incinerationGroup.render();
		}

		void MeshSystem::updateInstanceData(float iTime)
		{
			m_incinerationGroup.updateInstanceData(iTime);
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
				m_incinerationGroup.fillInstanceBuffer();
				m_incinerationGroup.render();
			}

			m_shadowSubSystem.captureSpotShadow();
			m_normalGroup.fillInstanceBuffer();
			m_normalGroup.render();
			m_dissolutionGroup.fillInstanceBuffer();
			m_dissolutionGroup.render();
			m_incinerationGroup.fillInstanceBuffer();
			m_incinerationGroup.render();
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
				m_incinerationGroup.fillInstanceBuffer();
				m_incinerationGroup.render();
			}
		}

		std::pair<uint32_t, InstanceProperties> MeshSystem::addNormalInstance(std::shared_ptr<mdl::Model> mod, const Material& mtrl, const Instance& inc)
		{
			return m_normalGroup.addModel(mod, mtrl, inc);
		}

		void MeshSystem::removeNormalInstance(const InstanceProperties& instanceData)
		{
			m_normalGroup.removeInstance(instanceData);
		}

		std::pair<uint32_t, InstanceProperties> MeshSystem::addDissolutionInstance(std::shared_ptr<mdl::Model> mod, const Material& mtrl, const InstanceDissolution& inc)
		{
			return m_dissolutionGroup.addModel(mod, mtrl, inc);
		}

		void MeshSystem::removeDissolutionInstance(const InstanceProperties& instanceData)
		{
			m_dissolutionGroup.removeInstance(instanceData);
		}

		std::pair<uint32_t, InstanceProperties> MeshSystem::addIncinerationInstance(std::shared_ptr<mdl::Model> mod, const Material& mtrl, const InstanceIncineration& inc)
		{
			return m_incinerationGroup.addModel(mod, mtrl, inc);
		}

		void MeshSystem::removeIncinerationInstance(const InstanceProperties& instanceData)
		{
			m_incinerationGroup.removeInstance(instanceData);
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

				data.depthStencilDesc.StencilEnable = true;
				data.depthStencilDesc.StencilWriteMask = 0xFF;
				data.depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
				data.depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
				data.depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				data.depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
				data.depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
				data.depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
				data.depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				data.depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

				data.stencilRef = (type == DISSOLUTION_RENDER || type == NORMAL_RENDER || type == INCINERATION_RENDER) ? PBR_STENCIL_REF : EMISSION_STENCIL_REF;

				data.blendDesc.BlendEnable = false;
				data.blendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

				initPipeline(m_pipelines[type], data);
			}
		}
		void MeshSystem::bindPipelineViaType(PipelineTypes pipelineType)
		{
			bindPipeline(m_pipelines[pipelineType]);
		}
		void MeshSystem::bindShadows()
		{
			m_shadowSubSystem.bindDataAndBuffers();
		}
		void MeshSystem::unbindShadows()
		{
			m_shadowSubSystem.unbindDepthBuffers();
		}
		uint32_t MeshSystem::getGroupMatrixIdx(const InstanceProperties& insProps)
		{
			switch (insProps.group) {
			case GroupTypes::NORMAL:
			{
				return m_normalGroup.getMatrixIdx(insProps);
			}
			case GroupTypes::HOLOGRAM:
			{
				return m_hologramGroup.getMatrixIdx(insProps);
			}
			case GroupTypes::EMISSION_ONLY:
			{
				return m_emissionOnlyGroup.getMatrixIdx(insProps);
			}
			case GroupTypes::INCINERATION:
			{
				return m_incinerationGroup.getMatrixIdx(insProps);
			}
			}
		}
		std::shared_ptr<mdl::Model> MeshSystem::getModelByInsProps(const InstanceProperties& insProps)
		{
			switch (insProps.group) {
			case GroupTypes::NORMAL:
			{
				return m_normalGroup.getModelByInsProps(insProps);
			}
			case GroupTypes::HOLOGRAM:
			{
				return m_hologramGroup.getModelByInsProps(insProps);
			}
			case GroupTypes::EMISSION_ONLY:
			{
				return m_emissionOnlyGroup.getModelByInsProps(insProps);
			}
			case GroupTypes::INCINERATION:
			{
				return m_incinerationGroup.getModelByInsProps(insProps);
			}
			}
		}
		Material MeshSystem::getMaterialByInsProps(const InstanceProperties& insProps)
		{
			switch (insProps.group) {
			case GroupTypes::NORMAL:
			{
				return m_normalGroup.getMaterialByInsProps(insProps);
			}
			case GroupTypes::HOLOGRAM:
			{
				return m_hologramGroup.getMaterialByInsProps(insProps);
			}
			case GroupTypes::EMISSION_ONLY:
			{
				return m_emissionOnlyGroup.getMaterialByInsProps(insProps);
			}
			case GroupTypes::INCINERATION:
			{
				return m_incinerationGroup.getMaterialByInsProps(insProps);
			}
			}
		}
	} // rend
} // engn