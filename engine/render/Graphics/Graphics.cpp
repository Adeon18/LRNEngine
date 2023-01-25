#include <iostream>
#include <vector>

#include "Graphics.hpp"

#include "include/utility/utility.hpp"

namespace engn {
	namespace rend {
		void Graphics::init() {
			m_initRasterizer();
			m_initDepthStencilState();
			m_initShaders();
			m_initScene();
		}

		void Graphics::renderFrame(std::unique_ptr<EngineCamera>& camPtr, const RenderData& renderData)
		{
			// Set Input Assembler Data
			d3d::s_devcon->IASetInputLayout(m_vertexShader.getInputLayout());
			d3d::s_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			d3d::s_devcon->RSSetState(m_rasterizerState.Get());
			d3d::s_devcon->OMSetDepthStencilState(m_depthStensilState.Get(), 0);
			// Set Shaders
			d3d::s_devcon->VSSetShader(m_vertexShader.getShader(), NULL, 0);
			d3d::s_devcon->PSSetShader(m_pixelShader.getShader(), NULL, 0);

			// VS constant buffer
			XMMATRIX world = DirectX::XMMatrixIdentity();
			m_constantBufferVS.getData().worldToClip = world * camPtr->getViewMatrix() * camPtr->getProjMatrix();
			m_constantBufferVS.getData().worldToClip = DirectX::XMMatrixTranspose(m_constantBufferVS.getData().worldToClip);
			m_constantBufferVS.fill();
			d3d::s_devcon->VSSetConstantBuffers(0, 1, m_constantBufferVS.getBufferAddress());

			// PS constant Buffer
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
			Logger::instance().logInfo(L"Shader Folder found: " + shaderFolder);

			m_vertexShader.init(shaderFolder + L"VSMoveByOffset.cso", layout, ARRAYSIZE(layout));
			m_pixelShader.init(shaderFolder + L"PSVoronoi.cso");
		}

		void Graphics::m_initScene()
		{
			std::vector vertices =
			{
				Vertex{{0.0f, 0.5f, 10.0f}, {1.0f, 0.0f, 0.0f, 1.0f}}, // top
				Vertex{{0.5f, -0.5f, 10.0f}, {1.0f, 0.0f, 0.0f, 1.0f}}, // right
				Vertex{{-0.5f, -0.5f, 10.0f}, {1.0f, 0.0f, 0.0f, 1.0f}}, // left
				// Second triangle with new points
				Vertex{{0.0f, 0.3f, 0.9f}, {0.0f, 1.0f, 0.0f, 1.0f}}, // left
				Vertex{{0.3f, -0.3f, 0.9f}, {0.0f, 1.0f, 0.0f, 1.0f}}, // left
				Vertex{{-0.3f, -0.3f, 0.9f}, {0.0f, 1.0f, 0.0f, 1.0f}}, // left
			};
			std::vector<DWORD> indices =
			{
				3, 4, 5,
				0, 1, 2,
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

			HRESULT hr = d3d::s_device->CreateRasterizerState(&rasterizerDesc, m_rasterizerState.GetAddressOf());
			if (FAILED(hr)) {
				Logger::instance().logErr("CreateRasterizerState fail: " + std::system_category().message(hr));
				return;
			}
		}
		//! Initialize the depth stencil state, set only once in window constructor
		void Graphics::m_initDepthStencilState() {
			D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc{};

			depthStencilStateDesc.DepthEnable = true;
			// If it is ALL, the stancil is turned ON, if ZERO, turned OFF
			depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER_EQUAL;

			HRESULT hr = d3d::s_device->CreateDepthStencilState(&depthStencilStateDesc, m_depthStensilState.GetAddressOf());
			if (FAILED(hr)) {
				Logger::instance().logErr("Window CreateDepthStencilState fail: " + std::system_category().message(hr));
				return;
			}
		}
	} // rend
} // engn