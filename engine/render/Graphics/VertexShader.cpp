#include <iostream>

#include "VertexShader.hpp"


namespace engn {
	namespace rend {
		void VertexShader::init(const std::wstring& shaderPath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElem) {
			HRESULT res = D3DReadFileToBlob(shaderPath.c_str(), m_shaderBuffer.reset());
			if (res) { std::wcout << L"Failed to load Vertex shader: " + shaderPath << std::endl; }

			res = d3d::s_device->CreateVertexShader(
				m_shaderBuffer->GetBufferPointer(),
				m_shaderBuffer->GetBufferSize(),
				NULL, // The pointer to class linkage
				m_shader.reset()
			);
			if (res) { std::wcout << L"Failed to create Vertex shader: " + shaderPath << std::endl; }

			HRESULT hr = d3d::s_device->CreateInputLayout(
				layoutDesc,
				numElem,
				m_shaderBuffer->GetBufferPointer(),
				m_shaderBuffer->GetBufferSize(),
				m_inputLayout.reset()
			);
			if (res) { std::cout << "Failed to create InputLayout" << std::endl; }
		}

		[[nodiscard]] ID3D11VertexShader* VertexShader::getShader() {
			return m_shader.ptr();
		}
		[[nodiscard]] ID3D10Blob* VertexShader::getBuffer() {
			return m_shaderBuffer.ptr();
		}
		ID3D11InputLayout* VertexShader::getInputLayout()
		{
			return m_inputLayout.ptr();
		}
	} // rend
} // engn