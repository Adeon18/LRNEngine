#include "LightSystem.hpp"

namespace engn {
	namespace rend {
		void LightSystem::init()
		{
			m_lightBuffer.init();
		}
		void LightSystem::setDirLight(const XMFLOAT3& direction)
		{
			m_directionalLight.direction = { direction.x, direction.y, direction.z };
		}
		void LightSystem::bindLighting()
		{
			m_lightBuffer.getData().dirLight = m_directionalLight;

			m_lightBuffer.fill();

			d3d::s_devcon->PSSetConstantBuffers(1, 1, m_lightBuffer.getBufferAddress());
		}
	} // rend
} // engn
