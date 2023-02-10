#pragma once

#pragma comment(lib, "D3DCompiler.lib")

#include "BaseShader.hpp"

namespace engn {
	namespace rend {
		/// <summary>
		/// A wrapper class for a VertexShader
		/// </summary>
		class VertexShader : public BaseShader {
		public:
			void init(const std::wstring& shaderPath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElem);
			void bind();

			[[nodiscard]] ID3D11VertexShader* getShader() const { return m_shader.Get(); };
			[[nodiscard]] ID3D11InputLayout* getInputLayout() const { return m_inputLayout.Get(); }
		private:
			//! Call create shader on d3d_device
			bool createShader(const std::wstring& shaderPath);
			//! Create input layout(bound to vertexshader for now)
			bool createInputLayout(D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElem);

			Microsoft::WRL::ComPtr<ID3D11VertexShader> m_shader;
			Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
		};
	} // rend
} // engn