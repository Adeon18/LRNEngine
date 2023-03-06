#pragma once

#include "render/D3D/d3d.hpp"

#include "render/Graphics/EngineCamera.hpp"
#include "render/Graphics/HelperStructs.hpp"

#include "render/Graphics/DXBuffers/CBStructs.hpp"
#include "render/Graphics/DXBuffers/ConstantBuffer.hpp"

#include "render/Systems/LightStructs.hpp"

namespace engn {
	namespace rend {
		class LightSystem {
		public:
			static LightSystem& getInstance() {
				static LightSystem l;
				return l;
			}

			LightSystem(const LightSystem& other) = delete;
			LightSystem& operator=(const LightSystem& other) = delete;

			//! Initialize the LightSystem, should be called in Engine::init()
			void init();
			//! Set the direction of the directional light
			void setDirLight(const XMFLOAT3& direction);
			//! Add the pointLight by position and distance characteristics
			void addPointLight(const XMFLOAT3& position, const XMFLOAT3& distParams);
			//! Set the lighting settings for a spotlight like attenuation and angles
			void setSpotLightSettings(float cutoffAngleDeg, const XMFLOAT3& distParams);
			//! Bind the lighting CB, TODO: FOR NOW IS BOUND EVERY FRAME
			void bindLighting(std::unique_ptr<EngineCamera>& camPtr, const RenderModeFlags& flags);
		private:
			LightSystem() {}

			void bindSpotlight(const XMVECTOR& position, const XMVECTOR& direction);
			
			light::DirectionalLight m_directionalLight;
			std::vector<light::PointLight> m_pointLights;
			// This light can be bound to player camera
			light::SpotLight m_spotLight;

			ConstantBuffer<CB_PS_LightEmitters> m_lightBuffer;
		};
	} // rend
} // engn