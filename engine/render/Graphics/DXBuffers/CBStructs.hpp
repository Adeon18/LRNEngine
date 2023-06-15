#pragma once

#include <DirectXMath.h>

#include "render/Systems/LightStructs.hpp"

namespace engn {
	namespace rend {

		static constexpr int MAX_DIRLIGHT_COUNT = 1;
		static constexpr int MAX_POINTLIGHT_COUNT = 4;

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
			XMMATRIX worldToClip;
			XMMATRIX worldToClipInv;
			XMFLOAT4 gResolution;
			XMFLOAT4 gCameraPosition;
			XMFLOAT4 gPFSCubemapResolution;
			float gTime;
		};

		struct CB_PS_LightEmitters {
			XMINT4 dirLightCount;
			XMINT4 pointLightCount;
			light::DirectionalLight dirLights[MAX_DIRLIGHT_COUNT];
			light::PointLight pointLights[MAX_POINTLIGHT_COUNT];
			light::SpotLight spotLight;
		};

		struct CB_PS_LightControlFlags {
			int isDiffuseEnabled;
			int isSpecularEnabled;
			int isIBLEnabled;
		};

		struct CB_PS_ShadowMapToLightMatrices {
			XMMATRIX pointLightViewProj[MAX_POINTLIGHT_COUNT][6];
			XMMATRIX dirLightViewProj[MAX_DIRLIGHT_COUNT];
			XMMATRIX spotLightViewProj;
			XMVECTOR texelSizeClipSpaceDirectionalMap;
			XMVECTOR texelWorldSpaceSizeDirectionalMap;
		};

		struct CB_PS_ShadowControlFlags {
			bool enabled;
			float directionalBiasMax;
			float directionalBiasMin;
			float pointBiasMax;
			float pointBiasMin;
			float spotBiasMax;
			float spotBiasMin;
		};

		// Cb Struct for the postProcess effect
		struct CB_PS_HDR {
			XMFLOAT4 EV100;
		};

		//! CB Struct to tell whether a patricular material has respective textures, ints are used as booleans
		struct CB_PS_MaterialData {
			XMFLOAT4 defaultMetallicValue;
			XMFLOAT4 defaultRoughnessValue;
			int isDiffuseTexBound;
			int isNormalMapBound;
			int isRoughnessTexBound;
			int isMetallicTexBound;
		};

		struct CB_VS_WorldToClip {
			XMMATRIX worldToClip;
		};

		//! This buffer is used during preFilteredSpecular precomputing
		struct CB_PS_RoughnessBuffer {
			float roughness;
		};

		//! A buffer that is used at 2D directional and spotlight shadow generation
		struct CB_VS_Shadow2DGenBuffer {
			XMMATRIX worldToClip;
		};

		//! A buffer used in geometry shader to write shadows to a cubemap via 1 draw call by doing
		//! 6 transformations
		struct CB_GS_ShadowCubeGenBuffer {
			XMMATRIX projMat;
			XMMATRIX viewMatrices[6];
		};

		//! Dat aused for particle generation
		struct CB_VS_ParticleData {
			XMVECTOR cameraPosition;
		};

		struct CB_PS_ParticleData {
			// xy store V, zw store H
			XMINT4 atlasFrameCount;
			XMINT4 animationSpeedFPS;
		};

		struct CB_PS_FXAAData {
			XMVECTOR imageSize;
			XMVECTOR AAData;
		};

	} // rend
} // engn