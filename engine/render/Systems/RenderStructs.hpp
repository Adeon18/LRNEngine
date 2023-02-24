#pragma once
#include <DirectXMath.h>


namespace engn {

	namespace rend{
		using namespace DirectX;

		struct Instance {
			XMMATRIX modelToWorld;
			XMFLOAT4 color;
		};

		struct Material {
			bool operator==(const Material& other) {
				return true;
			}
		};
	} // rend
} // engn

