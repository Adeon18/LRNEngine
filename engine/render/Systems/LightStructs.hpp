#pragma once

#include <DirectXMath.h>


namespace engn {
	namespace light {
		using namespace DirectX;

		//! Basic constant attenuation configs for
		static constexpr XMFLOAT3 PLIGHT_DIST_7{ 1.0f, 0.7f, 1.8f };
		static constexpr XMFLOAT3 PLIGHT_DIST_20{ 1.0f, 0.22f, 0.2f };
		static constexpr XMFLOAT3 PLIGHT_DIST_50{ 1.0f, 0.09, 0.032f };
		static constexpr XMFLOAT3 PLIGHT_DIST_65{ 1.0f, 0.07f, 0.017f };
		static constexpr XMFLOAT3 PLIGHT_DIST_100{ 1.0f, 0.045f, 0.0075f };


		// Directional light struct(for now, partially constant)
		struct DirectionalLight {
			XMVECTOR direction{ 0.0f, 0.0f, 0.0f };
			XMVECTOR ambient{ 0.05f, 0.05f, 0.05f };
			XMVECTOR diffuse{ 0.1f, 0.1f, 0.1f };
			XMVECTOR specular{ 0.1f, 0.1f, 0.1f };
		};

		struct PointLight {
			XMVECTOR position;
			XMVECTOR ambient{ 0.05f, 0.05f, 0.05f };
			XMVECTOR diffuse{ 0.8f, 0.8f, 0.8f };
			XMVECTOR specular{ 1.0f, 1.0f, 1.0f };

			// Constant, linear, quadratic and padding
			XMVECTOR distanceCharacteristics;
		};



	} // light
} // engn