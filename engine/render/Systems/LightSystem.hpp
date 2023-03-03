#pragma once

#include "render/D3D/d3d.hpp"

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
			//! Bind the lighting CB, TODO: FOR NOW IS BOUND EVERY FRAME
			void bindLighting();
		private:
			LightSystem() {}

			light::DirectionalLight m_directionalLight;

			ConstantBuffer<CB_PS_LightEmitters> m_lightBuffer;
		};
	} // rend
} // engn