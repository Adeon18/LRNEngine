#pragma once

#pragma comment(lib, "D3DCompiler.lib")

#include <string>
#include "render/D3D/d3d.hpp"

namespace engn {
	namespace rend {
		/// <summary>
		/// A wrapper class for a PixelShader
		/// </summary>
		/// 
		///
		class PixelShader {
		public:
			void init(const std::wstring& shaderPath);
			[[nodiscard]] ID3D11PixelShader* getShader();
			[[nodiscard]] ID3D10Blob* getBuffer();
			[[nodiscard]] ID3D11InputLayout* getInputLayout();
		private:
			DxResPtr<ID3D11PixelShader> m_shader;
			DxResPtr<ID3D10Blob> m_shaderBuffer;
			DxResPtr<ID3D11InputLayout> m_inputLayout;
		};
	} // rend
} // engn