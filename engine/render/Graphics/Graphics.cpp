#include "Graphics.hpp"


namespace engn {
	namespace rend {
		void Graphics::init() {
			m_initShaders();
		}

		void Graphics::m_initShaders() {

			D3D11_INPUT_ELEMENT_DESC layout[] = {
				{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0}
			};

			std::wstring shaderFolder;

// Determine the shader path
#ifdef _DEBUG // Debug
	#ifdef _WIN64 // x64
					shaderFolder = L"../x64/Debug/";
	#else
					shaderFolder = L"../Debug/";
	#endif
#else // Release
	#ifdef _WIN64 // x64
					shaderFolder = L"../x64/Release/";
	#else
					shaderFolder = L"../Release/";
	#endif
#endif

			m_vertexShader.init(shaderFolder + L"VS.cso", layout, ARRAYSIZE(layout));

		}
	} // rend
} // engn