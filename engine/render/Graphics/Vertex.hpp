#pragma once

#include <DirectXMath.h>

namespace engn {
	struct Vertex {
		Vertex() {}
		Vertex(const DirectX::XMFLOAT3& p, const DirectX::XMFLOAT4& c) : pos{ p }, col{ c } {};

		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 col;
	};
} // engn