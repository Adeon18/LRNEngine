#include "LightSystem.hpp"

#include "render/Systems/MeshSystem.hpp"
#include "utils/ModelManager/ModelManager.hpp"

namespace engn {
	namespace rend {
		void LightSystem::init()
		{
			m_lightBuffer.init();
		}
		void LightSystem::setDirLight(const XMFLOAT3& direction, const XMFLOAT3& ambient, const XMFLOAT3& diffuse, const XMFLOAT3& specular, const XMVECTOR& color)
		{
			m_directionalLight.direction = { direction.x, direction.y, direction.z };
			m_directionalLight.ambient = { ambient.x, ambient.y, ambient.z };
			m_directionalLight.diffuse = { diffuse.x, diffuse.y, diffuse.z };
			m_directionalLight.specular = { specular.x, specular.y, specular.z };
			m_directionalLight.color = color;
		}
		void LightSystem::addPointLight(const XMMATRIX& modelToWorld, const XMFLOAT3& ambient, const XMFLOAT3& diffuse, const XMFLOAT3& specular, const XMFLOAT3& distParams, const XMVECTOR& color)
		{
			light::PointLight pLight;
			pLight.ambient = { ambient.x, ambient.y, ambient.z };
			pLight.diffuse = { diffuse.x, diffuse.y, diffuse.z };
			pLight.specular = { specular.x, specular.y, specular.z };
			pLight.distanceCharacteristics = { distParams.x, distParams.y, distParams.z };
			pLight.color = color;

			addPointLight(std::move(pLight), modelToWorld);
		}
		void LightSystem::addPointLight(light::PointLight&& pLight, const XMMATRIX& modelToWorld)
		{
			m_pointLightMatrixIndices.push_back(
				MeshSystem::getInstance().addEmissionInstance(
					mdl::ModelManager::getInstance().getModel(EXE_DIR + SPHERE_MODEL_PATH),
					{},
					// We decrease the sphere 5 times to visualize pointlight
					{ XMMatrixScaling(0.2f, 0.2f, 0.2f) * modelToWorld, {}, pLight.color }
				)
			);
			m_pointLights.push_back(pLight);
		}
		void LightSystem::setSpotLightSettings(const XMFLOAT2& cutoffAngles, const XMFLOAT3& ambient, const XMFLOAT3& diffuse, const XMFLOAT3& specular, const XMFLOAT3& distParams, const XMVECTOR& color)
		{
			m_spotLight.cutoffAngleInner = { XMScalarCos(XMConvertToRadians(cutoffAngles.x)) };
			m_spotLight.cutoffAngleOuter = { XMScalarCos(XMConvertToRadians(cutoffAngles.y)) };
			m_spotLight.ambient = { ambient.x, ambient.y, ambient.z };
			m_spotLight.diffuse = { diffuse.x, diffuse.y, diffuse.z };
			m_spotLight.specular = { specular.x, specular.y, specular.z };
			m_spotLight.distanceCharacteristics = { distParams.x, distParams.y, distParams.z };
			m_spotLight.color = color;
		}
		void LightSystem::bindLighting(std::unique_ptr<EngineCamera>& camPtr, const RenderModeFlags& flags)
		{
			m_lightBuffer.getData().dirLight = m_directionalLight;

			int32_t pointLightCount = m_pointLights.size();
			m_lightBuffer.getData().pointLightCount = { pointLightCount , pointLightCount , pointLightCount , pointLightCount };

			for (int i = 0; i < m_pointLights.size(); ++i) {
				m_pointLights[i].position = XMVector3Transform({0.0f, 0.0f, 0.0f}, TransformSystem::getInstance().getMatrixById(m_pointLightMatrixIndices[i]));
				m_lightBuffer.getData().pointLights[i] = m_pointLights[i];
			}

			// Bind flashlight
			if (flags.bindFlashlight) {
				bindSpotlight(camPtr->getCamPosition(), camPtr->getCamForward());
			}

			m_lightBuffer.getData().spotLight = m_spotLight;

			m_lightBuffer.fill();

			d3d::s_devcon->PSSetConstantBuffers(1, 1, m_lightBuffer.getBufferAddress());
		}
		void LightSystem::bindSpotlight(const XMVECTOR& position, const XMVECTOR& direction)
		{
			// TODO: For now set every frame at bind
			m_spotLight.position = position;
			m_spotLight.direction = direction;
		}
	} // rend
} // engn
