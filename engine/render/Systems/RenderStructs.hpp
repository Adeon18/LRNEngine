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

