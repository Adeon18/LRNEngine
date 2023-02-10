#include <iostream>

#include "utils/Logger/Logger.hpp"

#include "GeometryShader.hpp"


namespace engn {
	namespace rend {
		void GeometryShader::init(const std::wstring& shaderPath) {
			if (!readShaderFile(shaderPath)) { return; }
			if (!createShader(shaderPath)) { return; }
		}

		void GeometryShader::bind()
		{
			d3d::s_devcon->GSSetShader(m_shader.Get(), NULL, 0);
		}

		bool GeometryShader::createShader(const std::wstring& shaderPath)
		{
			HRESULT hr = d3d::s_device->CreateGeometryShader(
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