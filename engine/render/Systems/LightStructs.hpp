#pragma once

#include <DirectXMath.h>


namespace engn {
	namespace light {
		using namespace DirectX;

		// Directional light struct(for now, partially constant)
		struct DirectionalLight {
			XMVECTOR direction{ 0.0f, 0.0f, 0.0f };
			XMVECTOR ambient{ 0.05f, 0.05f, 0.05f };
			XMVECTOR diffuse{ 0.1f, 0.1f, 0.1f };
			XMVECTOR specular{ 0.1f, 0.1f, 0.1f };
		};

		struct PointLight {

		};



	} // light
} // engn