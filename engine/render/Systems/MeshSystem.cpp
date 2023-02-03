#include "MeshSystem.hpp"

namespace engn {
	namespace rend {
		void MeshSystem::initNormalGroup()
		{
			std::wstring shaderFolder = util::getExeDirW();
			Logger::instance().logInfo(L"Shader Folder found: " + shaderFolder);

			m_normalGroup.init(shaderFolder + L"VSBasicColor.cso", shaderFolder + L"PSVoronoi.cso");

		}
		void MeshSystem::initHologramGroup()
		{
			std::wstring shaderFolder = util::getExeDirW();
			Logger::instance().logInfo(L"Shader Folder found: " + shaderFolder);

			m_hologramGroup.init(shaderFolder + L"VSHologram.cso", shaderFolder + L"PSHologram.cso");
		}
		void MeshSystem::render(const XMMATRIX& worldToClip)
		{
			m_normalGroup.fillInstanceBuffer(worldToClip);
			m_normalGroup.render();

			m_hologramGroup.fillInstanceBuffer(worldToClip);
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

		void MeshSystem::setNormalInstancePosition(std::shared_ptr<mdl::Model> mod, uint32_t mtrlIdx, uint32_t insIdx, XMVECTOR pos)
		{
			m_normalGroup.setModelPosition(mod, mtrlIdx, insIdx, pos);
		}

		std::pair<bool, InstanceToDrag> MeshSystem::getClosestNormalMesh(geom::Ray& ray, mdl::MeshIntersection& nearest)
		{
			InstanceToDrag i2d{};
			bool collided = m_normalGroup.checkRayIntersection(ray, nearest, i2d);

			return std::pair<bool, InstanceToDrag>{collided, i2d};
		}
		std::pair<bool, InstanceToDrag> MeshSystem::getClosestHologramMesh(geom::Ray& ray, mdl::MeshIntersection& nearest)
		{
			InstanceToDrag i2d{};
			bool collided = m_hologramGroup.checkRayIntersection(ray, nearest, i2d);

			return std::pair<bool, InstanceToDrag>{collided, i2d};
		}
	} // rend
} // engn