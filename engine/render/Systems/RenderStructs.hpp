#pragma once

#include <DirectXMath.h>

#include "render/Objects/Texture.hpp"

namespace engn {

	namespace rend{
		using namespace DirectX;

		struct Instance {
			XMMATRIX modelToWorld;
			XMFLOAT4 color;
		};

		struct Material {
			std::shared_ptr<tex::Texture> texPtr;

			bool operator==(const Material& other) {
				return texPtr == other.texPtr;
			}
		};
	} // rend
} // engn

