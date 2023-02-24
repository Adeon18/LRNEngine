#include "BaseShader.hpp"

namespace engn {
	namespace rend {
		bool BaseShader::readShaderFile(const std::wstring& shaderPath)
		{
			// Read vertex Shader
			HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), m_shaderBuffer.GetAddressOf());
			if (FAILED(hr)) {
				Logger::instance().logErr("Failed to read Shader: " +
					std::string(shaderPath.begin(), shaderPath.end()) + ": " + std::system_category().message(hr));
				return false;
			}
			return true;
		}
	} // rend
} // engn