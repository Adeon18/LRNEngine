#include <iostream>

#include "utils/Logger/Logger.hpp"

#include "VertexShader.hpp"


namespace engn {
	namespace rend {
		void VertexShader::init(const std::wstring& shaderPath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElem) {
			if (!readShaderFile(shaderPath)) { return; }
			if (!createShader(shaderPath)) { return; }
			if (!createInputLayout(layoutDesc, numElem)) { return; }
		}

		void VertexShader::bind()
		{
			d3d::s_devcon->VSSetShader(m_shader.Get(), NULL, 0);
		}

		bool VertexShader::createShader(const std::wstring& shaderPath)
		{
			HRESULT hr = d3d::s_device->CreateVertexShader(
				m_shaderBuffer->GetBufferPointer(),
				m_shaderBuffer->GetBufferSize(),
				NULL, // The pointer to class linkage
				m_shader.GetAddressOf()
			);
			if (FAILED(hr)) {
				Logger::instance().logErr("Failed to create Vertex Shader " +
					std::string(shaderPath.begin(), shaderPath.end()) + ": " + std::system_category().message(hr));
				return false;
			}

			return true;
		}

		bool VertexShader::createInputLayout(D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElem)
		{
			HRESULT hr = d3d::s_device->CreateInputLayout(
				layoutDesc,
				numElem,
				m_shaderBuffer->GetBufferPointer(),
				m_shaderBuffer->GetBufferSize(),
				m_inputLayout.GetAddressOf()
			);

			if (FAILED(hr)) {
				Logger::instance().logErr("Failed to create Input Layout: " + std::system_category().message(hr));
				return false;
			}

			return true;
		}
	} // rend
} // engn