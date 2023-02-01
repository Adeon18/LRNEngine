#pragma once

#include <DirectXMath.h>

namespace engn {
	namespace rend {
		//! This struct is for moving the vertices in the VS by some offset
		struct CB_VS_MeshData {
			DirectX::XMMATRIX worldToClip;
		};
		//! This struct contains basic data that ShaderToy has out of the box
		struct CB_RealTimeData {
			DirectX::XMFLOAT4 gResolution;
			DirectX::XMFLOAT4 gCameraPosition;
			float gTime;
		};
	} // rend
} // engn