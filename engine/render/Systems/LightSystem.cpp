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
		void LightSystem::addPointLight(const XMFLOAT3& position, const XMFLOAT3& distParams)
		{
			light::PointLight pLight;
			pLight.position = { position.x, position.y, position.z };
			pLight.distanceCharacteristics = { distParams.x, distParams.y, distParams.z };

			m_pointLights.push_back(std::move(pLight));
		}
		void LightSystem::bindLighting()
		{
			int32_t pointLightCount = m_pointLights.size();
			m_lightBuffer.getData().pointLightCount = { pointLightCount , pointLightCount , pointLightCount , pointLightCount };


			m_lightBuffer.getData().dirLight = m_directionalLight;

			for (int i = 0; i < m_pointLights.size(); ++i) {
				m_lightBuffer.getData().pointLights[i] = m_pointLights[i];
			}

			m_lightBuffer.fill();

			d3d::s_devcon->PSSetConstantBuffers(1, 1, m_lightBuffer.getBufferAddress());
		}
	} // rend
} // engn
