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

		std::pair<bool, RenderGroup<Instance, Material>::InstanceToDrag> MeshSystem::getClosestNormalMesh(geom::Ray& ray, mdl::MeshIntersection& nearest)
		{

			RenderGroup<Instance, Material>::InstanceToDrag i2d{};
			bool collided = m_normalGroup.checkRayIntersection(ray, nearest, i2d);

			return std::pair<bool, RenderGroup<Instance, Material>::InstanceToDrag>{collided, i2d};
		}
	} // rend
} // engn