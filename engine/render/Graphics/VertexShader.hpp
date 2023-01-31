#pragma once

#pragma comment(lib, "D3DCompiler.lib")

#include <string>
#include "render/D3D/d3d.hpp"

namespace engn {
	namespace rend {
		/// <summary>
		/// A wrapper class for a VertexShader
		/// </summary>
		class VertexShader {
		public:
			void init(const std::wstring& shaderPath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElem);
			void bind();
			[[nodiscard]] ID3D11VertexShader* getShader() const;
			[[nodiscard]] ID3D10Blob* getBuffer() const;
			[[nodiscard]] ID3D11InputLayout* getInputLayout() const;
		private:
			Microsoft::WRL::ComPtr<ID3D11VertexShader> m_shader;
			Microsoft::WRL::ComPtr<ID3D10Blob> m_shaderBuffer;
			Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
		};
	} // rend
} // engn