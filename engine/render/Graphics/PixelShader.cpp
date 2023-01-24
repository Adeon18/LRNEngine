#include <iostream>

#include "utils/Logger/Logger.hpp"

#include "PixelShader.hpp"


namespace engn {
	namespace rend {
		void PixelShader::init(const std::wstring& shaderPath) {
			// Read vertex Shader
			HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), m_shaderBuffer.GetAddressOf());
			if (FAILED(hr)) {
				Logger::instance().logErr("Failed to load Pixel Shader " +
					std::string(shaderPath.begin(), shaderPath.end()) + ": " + std::system_category().message(hr));
				return;
			}

			// CreateVertexShader
			hr = d3d::s_device->CreatePixelShader(
				m_shaderBuffer->GetBufferPointer(),
				m_shaderBuffer->GetBufferSize(),
				NULL, // The pointer to class linkage
				m_shader.GetAddressOf()
			);
			if (FAILED(hr)) {
				Logger::instance().logErr("Failed to create Pixel Shader " +
					std::string(shaderPath.begin(), shaderPath.end()) + ": " + std::system_category().message(hr));
				return;
			}
		}

		[[nodiscard]] ID3D11PixelShader* PixelShader::getShader() {
			return m_shader.Get();
		}
		[[nodiscard]] ID3D10Blob* PixelShader::getBuffer() {
			return m_shaderBuffer.Get();
		}
	} // rend
} // engn