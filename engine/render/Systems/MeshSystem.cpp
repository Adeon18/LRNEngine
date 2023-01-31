#include "MeshSystem.hpp"

namespace engn {
	namespace rend {
		void MeshSystem::initNormalGroup()
		{
			std::wstring shaderFolder = util::getExeDirW();
			Logger::instance().logInfo(L"Shader Folder found: " + shaderFolder);

			m_normalGroup.init(shaderFolder + L"VSBasicColor.cso", shaderFolder + L"PSVoronoi.cso");

		}
		void MeshSystem::render(const XMMATRIX& worldToClip)
		{
			m_normalGroup.fillInstanceBuffer(worldToClip);
			m_normalGroup.render();
		}

		void MeshSystem::addNormalInstance(std::shared_ptr<mdl::Model> mod, const Material& mtrl, const Instance& inc)
		{
			m_normalGroup.addModel(mod, mtrl, inc);
		}
	} // rend
} // engn