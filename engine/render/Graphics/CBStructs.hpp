#pragma once

#include <DirectXMath.h>

namespace engn {
	namespace rend {
		//! This struct is for moving the vertices in the VS by some offset
		struct CB_VS_MoveBuffer {
			DirectX::XMFLOAT2 offset;
		};
		//! This struct contains basic data that ShaderToy has out of the box
		struct CB_FS_ShaderToy {
			DirectX::XMFLOAT4 gResolution;
			float gTime;
		};
	} // rend
} // engn