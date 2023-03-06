#pragma once

#include <DirectXMath.h>

#include "render/Systems/LightStructs.hpp"

namespace engn {
	namespace rend {

		static constexpr int MAX_POINTLIGHT_COUNT = 10;

		//! This struct is for moving the vertices in the VS by some offset
		struct CB_VS_MeshData {
			XMMATRIX meshToModel;
			XMMATRIX meshToModelInv;
		};
		//! Basic struct for real-time per-frame data for the vertex shader - has the worldToClip matrix and inverse(for normal in world space calc)
		struct CB_VS_RealTimeData {
			XMMATRIX worldToClip;
			XMMATRIX worldToClipInv;
			XMFLOAT4 gResolution;
			XMFLOAT4 gCameraPosition;
			float gTime;
		};
		//! Skybox data
		struct CB_VS_SkyBoxData {
			XMMATRIX worldToView;
			XMMATRIX viewToClip;
		};

		//! Skybox for a fullscreen triangle method
		struct CB_VS_SkyFullscreen {
			XMVECTOR BLFarPlane;
			XMVECTOR BRFarPlane;
			XMVECTOR TLFarPlane;
		};

		// CB struct for the pixelshader realtime data, does not have worldToClip matrix
		struct CB_PS_RealTimeData {
			XMFLOAT4 gResolution;
			XMFLOAT4 gCameraPosition;
			float gTime;
		};

		struct CB_PS_LightEmitters {
			XMINT4 pointLightCount;
			light::DirectionalLight dirLight;
			light::PointLight pointLights[MAX_POINTLIGHT_COUNT];
			light::SpotLight spotLight;
		};
	} // rend
} // engn