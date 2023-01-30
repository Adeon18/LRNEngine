#pragma once

#include <DirectXMath.h>

namespace engn {
	struct Vertex {
		Vertex() {}
		Vertex(const DirectX::XMFLOAT3& p) : pos{ p } {};

		DirectX::XMFLOAT3 pos;
	};
} // engn