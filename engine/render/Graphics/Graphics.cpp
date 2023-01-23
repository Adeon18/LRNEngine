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

		void Graphics::renderFrame(const RenderData& renderData)
		{
			// Set Input Assembler Data
			d3d::s_devcon->IASetInputLayout(m_vertexShader.getInputLayout());
			d3d::s_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			d3d::s_devcon->RSSetState(m_rasterizerState.Get());
			// Set Shaders
			d3d::s_devcon->VSSetShader(m_vertexShader.getShader(), NULL, 0);
			d3d::s_devcon->PSSetShader(m_pixelShader.getShader(), NULL, 0);

			// VS constant buffer
			m_constantBufferVS.getData().offset.x = 0.0f;
			m_constantBufferVS.getData().offset.y = 0.0f;
			m_constantBufferVS.fill();
			d3d::s_devcon->VSSetConstantBuffers(0, 1, m_constantBufferVS.getBufferAddress());

			m_constantBufferPS.getData().gResolution = { renderData.iResolutionX, renderData.iResolutionY, renderData.invResolutionX, renderData.invResolutionY };
			m_constantBufferPS.getData().gTime = renderData.iTime;
			m_constantBufferPS.fill();
			d3d::s_devcon->PSSetConstantBuffers(0, 1, m_constantBufferPS.getBufferAddress());

			// Bind the buffer
			UINT offset = 0;
			d3d::s_devcon->IASetVertexBuffers(
				0, // start slot
				1, // number of buffers
				m_vertexBuffer.getBufferAddress(),
				m_vertexBuffer.getStride(),
				&offset
			);
			d3d::s_devcon->IASetIndexBuffer(m_indexBuffer.getBufferPtr(), DXGI_FORMAT_R32_UINT, 0);

			// Draw(vertexCount and startLocation)
			d3d::s_devcon->DrawIndexed(m_indexBuffer.getBufferSize(), 0, 0);
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
			m_pixelShader.init(shaderFolder + L"Voronoi.cso");
		}

		void Graphics::m_initScene()
		{
			std::vector vertices =
			{
				Vertex{{0.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}}, // top - red
				Vertex{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}}, // right - blue
				Vertex{{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}, // left - green
			};
			std::vector<DWORD> indices =
			{
				0, 1, 2
			};
			m_vertexBuffer.init(vertices);
			m_indexBuffer.init(indices);
			m_constantBufferVS.init();
			m_constantBufferPS.init();
		}

		void Graphics::m_initRasterizer()
		{
			D3D11_RASTERIZER_DESC rasterizerDesc{};
			rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
			rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

			HRESULT res = d3d::s_device->CreateRasterizerState(&rasterizerDesc, m_rasterizerState.GetAddressOf());
			if (FAILED(res)) { std::cout << "CreateRasterizerState fail" << std::endl; }
		}
	} // rend
} // engn