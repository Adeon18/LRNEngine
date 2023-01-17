#include <iostream>

#include "PixelShader.hpp"


namespace engn {
	namespace rend {
		void PixelShader::init(const std::wstring& shaderPath) {
			// Read vertex Shader
			HRESULT res = D3DReadFileToBlob(shaderPath.c_str(), m_shaderBuffer.reset());
			if (res) { std::wcout << L"Failed to load Pixel shader: " + shaderPath << std::endl; }

			// CreateVertexShader
			res = d3d::s_device->CreatePixelShader(
				m_shaderBuffer->GetBufferPointer(),
				m_shaderBuffer->GetBufferSize(),
				NULL, // The pointer to class linkage
				m_shader.reset()
			);
			if (res) { std::wcout << L"Failed to create Pixel shader: " + shaderPath << std::endl; }
		}

		[[nodiscard]] ID3D11PixelShader* PixelShader::getShader() {
			return m_shader.ptr();
		}
		[[nodiscard]] ID3D10Blob* PixelShader::getBuffer() {
			return m_shaderBuffer.ptr();
		}
	} // rend
} // engn