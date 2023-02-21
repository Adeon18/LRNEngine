#include "MeshSystem.hpp"

namespace engn {
	namespace rend {
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
		
		void MeshSystem::render(const RenderModeFlags& flags)
		{
			// Normal group 
			m_normalGroup.fillInstanceBuffer();
			this->bindPipelineViaType(PipelineTypes::NORMAL_RENDER);
			m_normalGroup.render();
			if (flags.renderFaceNormals) {
				this->bindPipelineViaType(PipelineTypes::FACE_NORMAL_DEBUG);
				m_normalGroup.render();
			}
			if (flags.renderWireframes) {
				this->bindPipelineViaType(PipelineTypes::WIREFRAME_DEBUG);
				m_normalGroup.render();
			}

			// Hologram group
			m_hologramGroup.fillInstanceBuffer();
			this->bindPipelineViaType((flags.renderFaceNormals || flags.renderWireframes) ? PipelineTypes::NORMAL_RENDER: PipelineTypes::HOLOGRAM_RENDER);
			m_hologramGroup.render();
			if (flags.renderFaceNormals) {
				this->bindPipelineViaType(PipelineTypes::FACE_NORMAL_DEBUG);
				m_hologramGroup.render();
			}
			if (flags.renderWireframes) {
				this->bindPipelineViaType(PipelineTypes::WIREFRAME_DEBUG);
				m_hologramGroup.render();
			}
		}

		void MeshSystem::addNormalInstance(std::shared_ptr<mdl::Model> mod, const Material& mtrl, const Instance& inc)
		{
			m_normalGroup.addModel(mod, mtrl, inc);
		}


		void MeshSystem::addHologramInstance(std::shared_ptr<mdl::Model> mod, const Material& mtrl, const Instance& inc)
		{
			m_hologramGroup.addModel(mod, mtrl, inc);
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

		std::pair<bool, InstanceProperties> MeshSystem::getClosestMesh(geom::Ray& ray, mdl::MeshIntersection& nearest) {
			InstanceProperties i2d{};

			// TODO: Like this for now, haven't found the idea for organizing it in any other way yet
			bool collided = false;
			std::vector<bool> collisionResults;
			collisionResults.push_back(m_normalGroup.checkRayIntersection(ray, nearest, i2d));
			collisionResults.push_back(m_hologramGroup.checkRayIntersection(ray, nearest, i2d));

			if (std::any_of(collisionResults.begin(), collisionResults.end(), [](bool v) { return v; })) {
				collided = true;
			}

			return std::pair<bool, InstanceProperties>{collided, i2d};
		}
		void MeshSystem::initPipelines()
		{
			for (auto& [type, data] : PIPELINE_TYPE_DATA) {
				initPipeline(m_pipelines[type], data);
			}
		}
		void MeshSystem::bindPipelineViaType(PipelineTypes pipelineType)
		{
			bindPipeline(m_pipelines[pipelineType]);
		}
	} // rend
} // engn