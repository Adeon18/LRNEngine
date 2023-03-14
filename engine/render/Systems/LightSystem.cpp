#include <string>

#include "LightSystem.hpp"

#include "render/Systems/MeshSystem.hpp"
#include "utils/TextureManager/TextureManager.hpp"
#include "utils/ModelManager/ModelManager.hpp"

namespace engn {
	namespace rend {
		void LightSystem::init()
		{
			tex::TextureManager::getInstance().loadTextureDDS(EXE_DIR + SPOTLIGHT_TEXTURE_PATH);

			m_spotLightTexture = tex::TextureManager::getInstance().getTexture(EXE_DIR + SPOTLIGHT_TEXTURE_PATH);
			m_lightBuffer.init();
		}
		void LightSystem::addDirLight(const XMFLOAT3& direction, const XMFLOAT3& intensity, const XMVECTOR& color)
		{
			light::DirectionalLight dLight;
			dLight.direction = { direction.x, direction.y, direction.z };
			dLight.intensity = { intensity.x, intensity.y, intensity.z };
			dLight.color = color;

			m_directionalLights.push_back(std::move(dLight));
		}
		void LightSystem::addPointLight(const XMMATRIX& modelToWorld, const XMFLOAT3& intensity, const XMFLOAT3& distParams, const XMVECTOR& color)
		{
			light::PointLight pLight;
			pLight.distanceCharacteristics = { distParams.x, distParams.y, distParams.z };
			pLight.intensity = { intensity.x, intensity.y, intensity.z };
			pLight.color = color;

			addPointLight(std::move(pLight), modelToWorld);
		}
		void LightSystem::addPointLight(light::PointLight&& pLight, const XMMATRIX& modelToWorld)
		{
			m_pointLightMatrixIndices.push_back(
				MeshSystem::getInstance().addEmissionInstance(
					mdl::ModelManager::getInstance().getModel(EXE_DIR + SPHERE_MODEL_PATH),
					{},
					// We decrease the sphere 2 times to visualize pointlight
					{ XMMatrixScaling(0.5f, 0.5f, 0.5f) * modelToWorld, {}, pLight.color }
				)
			);
			m_pointLights.push_back(pLight);
		}
		void LightSystem::setSpotLightSettings(float cutoffAngleDeg, const XMFLOAT3& intensity, const XMFLOAT3& distParams, const XMVECTOR& color)
		{
			float angRad = XMConvertToRadians(cutoffAngleDeg);
			m_spotLight.cutoffAngle = { angRad, angRad, angRad, angRad };
			m_spotLight.distanceCharacteristics = { distParams.x, distParams.y, distParams.z };
			m_spotLight.intensity = { intensity.x, intensity.y, intensity.z };
			m_spotLight.color = color;
		}
		void LightSystem::bindLighting(std::unique_ptr<EngineCamera>& camPtr, const RenderModeFlags& flags)
		{
			int32_t dirLightCount = m_directionalLights.size();
			m_lightBuffer.getData().dirLightCount = { dirLightCount , dirLightCount , dirLightCount , dirLightCount };
			for (int i = 0; i < m_directionalLights.size(); ++i) {
				m_lightBuffer.getData().dirLights[i] = m_directionalLights[i];
			}

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

			d3d::s_devcon->PSSetConstantBuffers(LIGHT_BUFFER_SLOT, 1, m_lightBuffer.getBufferAddress());
		}
		void LightSystem::bindSpotlight(const XMVECTOR& position, const XMVECTOR& direction)
		{
			if (XMVector3Equal(position, m_spotLight.position) && XMVector3Equal(direction, m_spotLight.direction)) { return;  }
			
			m_spotLight.position = position;
			m_spotLight.direction = XMVectorSetW(direction, 0.0f);;

			static constexpr XMVECTOR UP{ 0.0f, 1.0f, 0.0f };

			XMVECTOR right = XMVector3Normalize(XMVector3Cross(UP, m_spotLight.direction));
			XMVECTOR top = XMVector3Normalize(XMVector3Cross(m_spotLight.direction, right));

			XMMATRIX modelToWorld;
			modelToWorld.r[0] = right;
			modelToWorld.r[1] = top;
			modelToWorld.r[2] = m_spotLight.direction;
			modelToWorld.r[3] = position;

			m_spotLight.modelToWorld = XMMatrixTranspose(modelToWorld);
			m_spotLight.modelToWorldInv = XMMatrixTranspose(XMMatrixInverse(nullptr, modelToWorld));

			// Bind the texture
			d3d::s_devcon->PSSetShaderResources(SPOTLIGHT_TEXTURE_SLOT, 1, m_spotLightTexture->textureView.GetAddressOf());
		}
	} // rend
} // engn
