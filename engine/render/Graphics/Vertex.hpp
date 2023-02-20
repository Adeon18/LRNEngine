#pragma once

#include <DirectXMath.h>

namespace engn {
	struct Vertex {
		Vertex() {}
		Vertex(
			const DirectX::XMFLOAT3& ps,
			const DirectX::XMFLOAT3& norm,
			const DirectX::XMFLOAT3& tnt,
			const DirectX::XMFLOAT3& btnt,
			const DirectX::XMFLOAT2& tcods) : 
			pos{ ps },
			normal{ norm },
			tangent{ tnt },
			bitangent{ btnt },
			tc{ tcods }
		{}

		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 tangent;
		DirectX::XMFLOAT3 bitangent;
		DirectX::XMFLOAT2 tc;
	};
} // engn