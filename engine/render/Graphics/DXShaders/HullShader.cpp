#include <iostream>

#include "utils/Logger/Logger.hpp"

#include "HullShader.hpp"


namespace engn {
	namespace rend {
		void HullShader::init(const std::wstring& shaderPath) {
			if (!readShaderFile(shaderPath)) { return; }
			if (!createShader(shaderPath)) { return; }
		}

		void HullShader::bind() const
		{
			d3d::s_devcon->HSSetShader(m_shader.Get(), NULL, 0);
		}

		bool HullShader::createShader(const std::wstring& shaderPath)
		{
			HRESULT hr = d3d::s_device->CreateHullShader(
				m_shaderBuffer->GetBufferPointer(),
				m_shaderBuffer->GetBufferSize(),
				NULL, // The pointer to class linkage
				m_shader.GetAddressOf()
			);

			if (FAILED(hr)) {
				Logger::instance().logErr("Failed to create Hull Shader " +
					std::string(shaderPath.begin(), shaderPath.end()) + ": " + std::system_category().message(hr));
				return false;
			}

			return true;
		}
	} // rend
} // engn