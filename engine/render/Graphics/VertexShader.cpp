#include <iostream>

#include "utils/Logger/Logger.hpp"

#include "VertexShader.hpp"


namespace engn {
	namespace rend {
		void VertexShader::init(const std::wstring& shaderPath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElem) {
			// Read vertex Shader
			HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), m_shaderBuffer.GetAddressOf());
			if (FAILED(hr)) {
				Logger::instance().logErr("Failed to load Vertex Shader " +
					std::string(shaderPath.begin(), shaderPath.end()) + ": " + std::system_category().message(hr));
				return;
			}

			// CreateVertexShader
			hr = d3d::s_device->CreateVertexShader(
				m_shaderBuffer->GetBufferPointer(),
				m_shaderBuffer->GetBufferSize(),
				NULL, // The pointer to class linkage
				m_shader.GetAddressOf()
			);
			if (FAILED(hr)) {
				Logger::instance().logErr("Failed to create Vertex Shader " +
					std::string(shaderPath.begin(), shaderPath.end()) + ": " + std::system_category().message(hr));
			}

			// InputLayout
			hr = d3d::s_device->CreateInputLayout(
				layoutDesc,
				numElem,
				m_shaderBuffer->GetBufferPointer(),
				m_shaderBuffer->GetBufferSize(),
				m_inputLayout.GetAddressOf()
			);
			if (FAILED(hr)) {
				Logger::instance().logErr("Failed to create Input Layout: " + std::system_category().message(hr));
			}
		}

		[[nodiscard]] ID3D11VertexShader* VertexShader::getShader() const {
			return m_shader.Get();
		}
		[[nodiscard]] ID3D10Blob* VertexShader::getBuffer() const {
			return m_shaderBuffer.Get();
		}
		ID3D11InputLayout* VertexShader::getInputLayout() const {
			return m_inputLayout.Get();
		}
	} // rend
} // engn