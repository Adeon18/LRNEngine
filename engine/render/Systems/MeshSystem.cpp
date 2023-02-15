#include "MeshSystem.hpp"

namespace engn {
	namespace rend {
		void MeshSystem::initNormalGroup()
		{
			std::wstring shaderFolder = util::getExeDirW();
			Logger::instance().logInfo(L"Shader Folder found: " + shaderFolder);

			m_normalGroup.setType(GroupTypes::NORMAL);
			m_normalGroup.setTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_normalGroup.init(shaderFolder + L"VSVisNormal.cso", L"", L"", shaderFolder + L"GSVisNormal.cso", shaderFolder + L"PSVisNormal.cso");

		}
		void MeshSystem::initHologramGroup()
		{
			std::wstring shaderFolder = util::getExeDirW();
			Logger::instance().logInfo(L"Shader Folder found: " + shaderFolder);

			m_hologramGroup.setType(GroupTypes::HOLOGRAM);
			m_hologramGroup.setTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
			m_hologramGroup.init(
				shaderFolder + L"VSHologram.cso",
				shaderFolder + L"HSHologram.cso",
				shaderFolder + L"DSHologram.cso",
				shaderFolder + L"GSHologram.cso",
				shaderFolder + L"PSHologram.cso"
			);
		}
		void MeshSystem::render()
		{
			m_normalGroup.fillInstanceBuffer();
			this->bindPipelineViaType(PipelineTypes::NORMAL_RENDER);
			m_normalGroup.render();
			this->bindPipelineViaType(PipelineTypes::FACE_NORMAL_DEBUG);
			m_normalGroup.render();

			m_hologramGroup.fillInstanceBuffer();
			this->bindPipelineViaType(PipelineTypes::HOLOGRAM_RENDER);
			m_hologramGroup.render();
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