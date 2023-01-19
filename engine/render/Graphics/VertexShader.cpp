#include <iostream>

#include "VertexShader.hpp"


namespace engn {
	namespace rend {
		void VertexShader::init(const std::wstring& shaderPath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElem) {
			// Read vertex Shader
			HRESULT res = D3DReadFileToBlob(shaderPath.c_str(), m_shaderBuffer.GetAddressOf());
			if (res) { std::wcout << L"Failed to load Vertex shader: " + shaderPath << std::endl; }

			// CreateVertexShader
			res = d3d::s_device->CreateVertexShader(
				m_shaderBuffer->GetBufferPointer(),
				m_shaderBuffer->GetBufferSize(),
				NULL, // The pointer to class linkage
				m_shader.GetAddressOf()
			);
			if (res) { std::wcout << L"Failed to create Vertex shader: " + shaderPath << std::endl; }

			// InputLayout
			HRESULT hr = d3d::s_device->CreateInputLayout(
				layoutDesc,
				numElem,
				m_shaderBuffer->GetBufferPointer(),
				m_shaderBuffer->GetBufferSize(),
				m_inputLayout.GetAddressOf()
			);
			if (res) { std::cout << "Failed to create InputLayout" << std::endl; }
		}

		[[nodiscard]] ID3D11VertexShader* VertexShader::getShader() const {
			return m_shader.Get();
		}
		[[nodiscard]] ID3D10Blob* VertexShader::getBuffer() const {
			return m_shaderBuffer.Get();
		}
		ID3D11InputLayout* VertexShader::getInputLayout() const {
			return m_inputLayout.Get();
		}
	} // rend
} // engn