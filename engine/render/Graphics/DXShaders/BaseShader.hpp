#pragma once

#include <iostream>

#include "render/D3D/d3d.hpp"

#include "utils/Logger/Logger.hpp"


namespace engn {
	namespace rend {
		class BaseShader {
		public:
			//! Read the shader into the shader buffer
			bool readShaderFile(const std::wstring& shaderPath);

			[[nodiscard]] ID3D10Blob* getBuffer() const { m_shaderBuffer.Get(); }
			[[nodiscard]] bool isLoaded() const { return active; }
		protected:
			// The shader is active if shader path != ""
			bool active = false;
			Microsoft::WRL::ComPtr<ID3D10Blob> m_shaderBuffer;
		};
	} // rend
} // engn