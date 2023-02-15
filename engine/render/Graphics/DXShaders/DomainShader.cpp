#include <iostream>

#include "utils/Logger/Logger.hpp"

#include "DomainShader.hpp"


namespace engn {
	namespace rend {
		void DomainShader::init(const std::wstring& shaderPath) {
			if (!readShaderFile(shaderPath)) { return; }
			if (!createShader(shaderPath)) { return; }
		}

		void DomainShader::bind() const
		{
			d3d::s_devcon->DSSetShader(m_shader.Get(), NULL, 0);
		}

		bool DomainShader::createShader(const std::wstring& shaderPath)
		{
			HRESULT hr = d3d::s_device->CreateDomainShader(
				m_shaderBuffer->GetBufferPointer(),
				m_shaderBuffer->GetBufferSize(),
				NULL, // The pointer to class linkage
				m_shader.GetAddressOf()
			);

			if (FAILED(hr)) {
				Logger::instance().logErr("Failed to create Domain Shader " +
					std::string(shaderPath.begin(), shaderPath.end()) + ": " + std::system_category().message(hr));
				return false;
			}

			return true;
		}
	} // rend
} // engn