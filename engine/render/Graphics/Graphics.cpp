#include <iostream>
#include <vector>

#include "Graphics.hpp"

#include "include/utility/utility.hpp"

namespace engn {
	namespace rend {
		void Graphics::init() {
			m_initRasterizer();
			m_initShaders();
			m_initScene();
		}

		void Graphics::renderFrame()
		{
			// Set Input Assembler Data
			d3d::s_devcon->IASetInputLayout(m_vertexShader.getInputLayout());
			d3d::s_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			d3d::s_devcon->RSSetState(m_rasterizerState.ptr());
			// Set Shaders
			d3d::s_devcon->VSSetShader(m_vertexShader.getShader(), NULL, 0);
			d3d::s_devcon->PSSetShader(m_pixelShader.getShader(), NULL, 0);

			// Bind the buffer
			UINT stride = sizeof(Vertex);
			UINT offset = 0;
			d3d::s_devcon->IASetVertexBuffers(
				0, // start slot
				1, // number of buffers
				m_vertexBuffer.getAddressOf(),
				&stride,
				&offset
			);
			// Draw(vertexCount and startLocation)
			d3d::s_devcon->Draw(6, 0);
		}

		void Graphics::m_initShaders() {

			// Interleaved layout for now
			D3D11_INPUT_ELEMENT_DESC layout[] = {
				{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(Vertex::pos), D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0}
			};
			std::wstring shaderFolder = util::getExeDirW();
			std::wcout << "Shader Folder found: " << shaderFolder << std::endl;

			m_vertexShader.init(shaderFolder + L"VS.cso", layout, ARRAYSIZE(layout));
			m_pixelShader.init(shaderFolder + L"PS.cso");
		}

		void Graphics::m_initScene()
		{
			std::vector vertices =
			{
				Vertex{{0.0f, 0.7f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}}, // top - red
				Vertex{{0.3f, -0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}}, // right - blue
				Vertex{{-0.3f, -0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}, // bottom - green
				Vertex{{-0.3f, -0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}, // bottom - green
				Vertex{{0.3f, -0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}}, // right - blue
				Vertex{{0.0f, -0.7f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}}, // top - red

			};

			// Create vertex buffer description
			D3D11_BUFFER_DESC vertexBufferDesc;
			memset(&vertexBufferDesc, 0, sizeof(vertexBufferDesc));

			vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			vertexBufferDesc.ByteWidth = sizeof(Vertex) * vertices.size(); // The size of buffer
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // We tell that thi sbuffer is a vertexbuffer
			vertexBufferDesc.CPUAccessFlags = 0;
			vertexBufferDesc.MiscFlags = 0;
			vertexBufferDesc.StructureByteStride = 0;

			// Subresource data just has a pointer to out vertices
			D3D11_SUBRESOURCE_DATA vertexBufferData;
			memset(&vertexBufferData, 0, sizeof(vertexBufferData));
			vertexBufferData.pSysMem = vertices.data();

			// Create Buffer
			HRESULT res = d3d::s_device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, m_vertexBuffer.reset());
			if (FAILED(res)) { std::cout << "CreateBuffer fail" << std::endl; }
		}

		void Graphics::m_initRasterizer()
		{
			D3D11_RASTERIZER_DESC rasterizerDesc{};
			rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
			rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

			HRESULT res = d3d::s_device->CreateRasterizerState(&rasterizerDesc, m_rasterizerState.reset());
			if (FAILED(res)) { std::cout << "CreateRasterizerState fail" << std::endl; }
		}
	} // rend
} // engn