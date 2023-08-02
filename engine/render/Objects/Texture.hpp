#pragma once

#include "render/D3D/d3d.hpp"


namespace engn {
	namespace tex {
		struct Texture {
			Microsoft::WRL::ComPtr<ID3D11Resource> texture = nullptr;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView = nullptr;
		};
	}
} // engn