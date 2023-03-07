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
			std::shared_ptr<tex::Texture> texPtr = nullptr;

			bool operator==(const Material& other) {
				return texPtr == other.texPtr;
			}
		};
	} // rend
} // engn

