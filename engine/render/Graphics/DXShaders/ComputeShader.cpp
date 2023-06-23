#include "ComputeShader.hpp"

namespace engn {
	namespace rend {
		void ComputeShader::init(const std::wstring& shaderPath)
		{
			if (!readShaderFile(shaderPath)) { return; }
			if (!createShader(shaderPath)) { return; }
		}
		void ComputeShader::bind() const
		{
			d3d::s_devcon->CSSetShader(m_shader.Get(), NULL, 0);
		}
		bool ComputeShader::createShader(const std::wstring& shaderPath)
		{
			HRESULT hr = d3d::s_device->CreateComputeShader(
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