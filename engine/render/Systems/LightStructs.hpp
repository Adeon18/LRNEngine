#pragma once

#include <DirectXMath.h>


namespace engn {
	namespace light {
		using namespace DirectX;

		//! Colors
		static constexpr XMVECTOR WHITE{ 1.0f, 1.0f, 1.0f, 1.0f };
		static constexpr XMVECTOR RED{ 1.0f, 0.0f, 0.0f, 1.0f };
		static constexpr XMVECTOR GREEN{ 0.0f, 1.0f, 0.0f, 1.0f };
		static constexpr XMVECTOR BLUE{ 0.0f, 0.0f, 1.0f, 1.0f };
		static constexpr XMVECTOR BLACK{ 0.0f, 0.0f, 0.0f, 1.0f };

		//! Ambient - Read as XDX is read as X DOT X as in X.X
		static constexpr XMFLOAT3 AMBIENT0D05{ 0.05f, 0.05f, 0.05f };

		//! Diffuse - Read as XDX is read as X DOT X as in X.X
		static constexpr XMFLOAT3 DIFFUSE0D1{ 0.1f, 0.1f, 0.1f };
		static constexpr XMFLOAT3 DIFFUSE0D2{ 0.2f, 0.2f, 0.2f };
		static constexpr XMFLOAT3 DIFFUSE0D4{ 0.4f, 0.4f, 0.4f };
		static constexpr XMFLOAT3 DIFFUSE0D6{ 0.6f, 0.6f, 0.6f };
		static constexpr XMFLOAT3 DIFFUSE0D8{ 0.8f, 0.8f, 0.8f };
		static constexpr XMFLOAT3 DIFFUSE1{ 1.0f, 1.0f, 1.0f };

		//! Specular - Read as XDX is read as X DOT X as in X.X
		static constexpr XMFLOAT3 SPEC0D1{ 0.1f, 0.1f, 0.1f };
		static constexpr XMFLOAT3 SPEC0D2{ 0.2f, 0.2f, 0.2f };
		static constexpr XMFLOAT3 SPEC0D4{ 0.4f, 0.4f, 0.4f };
		static constexpr XMFLOAT3 SPEC0D6{ 0.6f, 0.6f, 0.6f };
		static constexpr XMFLOAT3 SPEC0D8{ 0.8f, 0.8f, 0.8f };
		static constexpr XMFLOAT3 SPEC1{ 1.0f, 1.0f, 1.0f };

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
			XMVECTOR ambient{ 0.05f, 0.05f, 0.05f };
			XMVECTOR diffuse{ 0.1f, 0.1f, 0.1f };
			XMVECTOR specular{ 0.1f, 0.1f, 0.1f };

			XMVECTOR color;
		};

		// Pointlight structs(again, for now, partically constant)
		struct PointLight {
			XMVECTOR position{ 0.0f, 0.0f, 0.0f, 1.0f };
			XMVECTOR ambient{ 0.05f, 0.05f, 0.05f };
			XMVECTOR diffuse{ 0.8f, 0.8f, 0.8f };
			XMVECTOR specular{ 1.0f, 1.0f, 1.0f };

			// Constant, linear, quadratic and padding
			XMVECTOR distanceCharacteristics;

			XMVECTOR color;
		};

		struct SpotLight {
			XMVECTOR position;
			XMVECTOR direction;

			XMMATRIX modelToWorld;
			XMMATRIX modelToWorldInv;

			// The XMVECTOR filled with 4 cutoff angle values(inner and outer)
			XMVECTOR cutoffAngle;

			XMVECTOR ambient{ 0.05f, 0.05f, 0.05f };
			XMVECTOR diffuse{ 0.8f, 0.8f, 0.8f };
			XMVECTOR specular{ 1.0f, 1.0f, 1.0f };

			// Constant, linear, quadratic and padding
			XMVECTOR distanceCharacteristics;

			XMVECTOR color;
		};

	} // light
} // engn