#pragma once

#include <DirectXMath.h>

#include "render/Objects/Texture.hpp"

namespace engn {

	namespace rend{
		using namespace DirectX;

		struct Instance {
			XMMATRIX modelToWorld;
			XMMATRIX modelToWorldInv;
			XMVECTOR color;
			uint32_t objectId;
		};

		struct InstanceDissolution {
			XMMATRIX modelToWorld;
			XMMATRIX modelToWorldInv;
			//! XY store spawn time, ZW store spawn duration
			XMVECTOR time;
		};

		struct InstanceIncineration {
			XMMATRIX modelToWorld;
			XMMATRIX modelToWorldInv;
			// Stores incineration HitPoint in world space in .xyz and max model incineration radius in .a
			XMVECTOR rayHitPointAndMaxRadius;
			//! XY store spawn time, ZW store spawn duration
			XMVECTOR time;
			//! Particle color is random per instance
			XMVECTOR color;
			// Prev frame radius and current frame radius
			XMFLOAT2 prevCurRad;
			uint32_t objectId;
		};

		struct Material {
			std::shared_ptr<tex::Texture> ambientTex = nullptr;
			std::shared_ptr<tex::Texture> normalMap = nullptr;
			std::shared_ptr<tex::Texture> roughness = nullptr;
			std::shared_ptr<tex::Texture> metallness = nullptr;

			bool operator==(const Material& other) {
				return ambientTex == other.ambientTex &&
					normalMap == other.normalMap &&
					metallness == other.metallness &&
					roughness == other.roughness;
			}

			bool empty() const { return !ambientTex.get() && !normalMap.get(); }
		};
	} // rend
} // engn

