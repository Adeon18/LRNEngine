#pragma once


#include <string>
#include "render/D3D/d3d.hpp"

namespace engn {
	namespace rend {
		/// <summary>
		/// A wrapper class for a PixelShader
		/// </summary>
		class PixelShader {
		public:
			void init(const std::wstring& shaderPath);
			[[nodiscard]] ID3D11PixelShader* getShader();
			[[nodiscard]] ID3D10Blob* getBuffer();
		private:
			DxResPtr<ID3D11PixelShader> m_shader;
			DxResPtr<ID3D10Blob> m_shaderBuffer;
		};
	} // rend
} // engn