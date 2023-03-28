#pragma once

#include <DirectXMath.h>


namespace engn {
	namespace light {
		using namespace DirectX;

		//! Colors
		static constexpr XMFLOAT3 WHITE{ 1.0f, 1.0f, 1.0f };
		static constexpr XMFLOAT3 RED{ 1.0f, 0.0f, 0.0f };
		static constexpr XMFLOAT3 GREEN{ 0.0f, 1.0f, 0.0f };
		static constexpr XMFLOAT3 BLUE{ 0.0f, 0.0f, 1.0f };
		static constexpr XMFLOAT3 BLACK{ 0.0f, 0.0f, 0.0f };

		//! Light intensities
		static constexpr XMFLOAT3 INTENSITY_0P05{ 0.05f, 0.05f, 0.05f };
		static constexpr XMFLOAT3 INTENSITY_0P1{ 0.1f, 0.1f, 0.1f };
		static constexpr XMFLOAT3 INTENSITY_0P5{ 0.5f, 0.5f, 0.5f };
		static constexpr XMFLOAT3 INTENSITY_DEFAULT{ 1.0f, 1.0f, 1.0f };
		static constexpr XMFLOAT3 INTENSITY_1P5X{ 1.5f, 1.5f, 1.5f };
		static constexpr XMFLOAT3 INTENSITY_2X{ 2.0f, 2.0f, 2.0f };
		static constexpr XMFLOAT3 INTENSITY_2P5X{ 2.5f, 2.5f, 2.5f };
		static constexpr XMFLOAT3 INTENSITY_3X{ 3.0f, 3.0f, 3.0f };

		//! Basic constant attenuation configs for PointLight
		static constexpr XMFLOAT3 LIGHT_DIST_7{ 1.0f, 0.7f, 1.8f };
		static constexpr XMFLOAT3 LIGHT_DIST_20{ 1.0f, 0.22f, 0.2f };
		static constexpr XMFLOAT3 LIGHT_DIST_50{ 1.0f, 0.09, 0.032f };
		static constexpr XMFLOAT3 LIGHT_DIST_65{ 1.0f, 0.07f, 0.017f };
		static constexpr XMFLOAT3 LIGHT_DIST_100{ 1.0f, 0.045f, 0.0075f };

		//! SpotLight size configs: READ: Spotlight Angle 12 and hanf 7 and half
		static constexpr XMFLOAT2 SLIGHT_ANG_12H_17H{ 12.5f, 17.5f };

		//! The light structs look like this, because they will later construct a constant buffer data
		//! so all the data must be 16-byte aligned

		// Directional light struct(for now, partially constant)
		struct DirectionalLight {
			XMVECTOR direction{ 0.0f, 0.0f, 0.0f };

			XMVECTOR radiance;
			XMVECTOR solidAngle;
		};

		// Pointlight structs(again, for now, partically constant)
		struct PointLight {
			XMVECTOR position{ 0.0f, 0.0f, 0.0f, 1.0f };

			XMVECTOR radiance;
			// XMVector filled with radius values
			XMVECTOR radius;
		};

		struct SpotLight {
			XMVECTOR position;
			XMVECTOR direction;

			XMMATRIX modelToWorld;
			XMMATRIX modelToWorldInv;

			// The XMVECTOR filled with 4 cutoff angle values(inner and outer)
			XMVECTOR cutoffAngle;

			XMVECTOR radiance;
			XMVECTOR radius;
		};

	} // light
} // engn