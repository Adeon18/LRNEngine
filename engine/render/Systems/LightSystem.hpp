#pragma once

#include "render/D3D/d3d.hpp"

#include "render/Graphics/EngineCamera.hpp"
#include "render/Graphics/HelperStructs.hpp"

#include "render/Graphics/DXBuffers/CBStructs.hpp"
#include "render/Graphics/DXBuffers/ConstantBuffer.hpp"

#include "render/Systems/LightStructs.hpp"

#include "render/Objects/Texture.hpp"

namespace engn {
	namespace rend {
		class LightSystem {
			//! The paths to sphere model
			const std::string EXE_DIR = util::getExeDir();
#ifdef _WIN64 
			const std::string SPHERE_MODEL_PATH = "../../assets/Models/Sphere/sphere.fbx";
			const std::string SPOTLIGHT_TEXTURE_PATH = "..\\..\\assets\\Textures\\SpotLightMasks\\flashlight2.dds";

#else
			const std::string SPOTLIGHT_TEXTURE_PATH = "..\\assets\\Textures\\SpotLightMasks\\flashlight2.dds";
			const std::string SPHERE_MODEL_PATH = "../assets/Models/Sphere/sphere.fbx";
#endif // !_WIN64
		public:
			static LightSystem& getInstance() {
				static LightSystem l;
				return l;
			}

			LightSystem(const LightSystem& other) = delete;
			LightSystem& operator=(const LightSystem& other) = delete;

			//! Initialize the LightSystem, should be called in Engine::init()
			void init();
			//! Set the directional light parameters
			void addDirLight(const XMFLOAT3& direction, const XMFLOAT3& intensity, const XMVECTOR& color);
			//! Set the pointlight via parameters
			void addPointLight(const XMMATRIX& modelToWorld, const XMFLOAT3& intensity, const XMFLOAT3& distParams, const XMVECTOR& color);
			//! Set the lighting settings for a spotlight(angle is the entire spotligth "diameter" angle)
			void setSpotLightSettings(float cutoffAngle, const XMFLOAT3& intensity, const XMFLOAT3& distParams, const XMVECTOR& color);
			//! Bind the lighting CB, TODO: FOR NOW IS BOUND EVERY FRAME
			void bindLighting(std::unique_ptr<EngineCamera>& camPtr, const RenderModeFlags& flags);
		private:
			LightSystem() {}
			//! Add the pointlight and sync it with the sphere visualization
			void addPointLight(light::PointLight&& pLight, const XMMATRIX& modelToWorld);
			//! Bind spotlight direction and position to the camera
			void bindSpotlight(const XMVECTOR& position, const XMVECTOR& direction);
			
			std::vector <light::DirectionalLight> m_directionalLights;
			std::vector<light::PointLight> m_pointLights;
			// The indices at which the pointlights have their transform matrices in TransformSystem
			std::vector<uint32_t> m_pointLightMatrixIndices;
			// This light can be bound to player camera
			light::SpotLight m_spotLight;

			std::shared_ptr<tex::Texture> m_spotLightTexture;
			ConstantBuffer<CB_PS_LightEmitters> m_lightBuffer;
		};
	} // rend
} // engn