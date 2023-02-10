#include <iostream>

#include "utils/Logger/Logger.hpp"

#include "PixelShader.hpp"


namespace engn {
	namespace rend {
		void PixelShader::init(const std::wstring& shaderPath) {
			if (!readShaderFile(shaderPath)) { return; }
			if (!createShader(shaderPath)) { return; }
		}

		void PixelShader::bind()
		{
			d3d::s_devcon->PSSetShader(m_shader.Get(), NULL, 0);
		}

		bool PixelShader::createShader(const std::wstring& shaderPath)
		{
			HRESULT hr = d3d::s_device->CreatePixelShader(
				m_shaderBuffer->GetBufferPointer(),
				m_shaderBuffer->GetBufferSize(),
				NULL, // The pointer to class linkage
				m_shader.GetAddressOf()
			);

			if (FAILED(hr)) {
				Logger::instance().logErr("Failed to create Pixel Shader " +
					std::string(shaderPath.begin(), shaderPath.end()) + ": " + std::system_category().message(hr));
				return false;
			}

			return true;
		}
	} // rend
} // engn