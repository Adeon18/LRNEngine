#include <iostream>
#include <sstream>
#include <vector>

#include "Graphics.hpp"

#include "include/utility/utility.hpp"

#include "utils/ModelManager/ModelManager.hpp"

namespace engn {
	namespace rend {
		void Graphics::init() {
			m_initRasterizer();
			m_initDepthStencilState();
			m_initShaders();
			m_initScene();
			
			//std::shared_ptr<mdl::Model> mptr = util::ModelManager::getInstance().getModel(util::getExeDir() + "../../assets/Models/Samurai/Samurai.fbx");
			/*for (auto& m : mptr->getMeshes()) {
				std::stringstream ss;
				ss << "Name: " << m.name << std::endl;
				ss << "Vertice num: " << m.vertices.size() << std::endl;
				ss << "Trinagles num: " << m.triangles.size() << std::endl;
				ss << "Instances size: " << m.instances.size() << std::endl;
				ss << "Instances Inverse size: " << m.instancesInv.size() << std::endl;
				Logger::instance().logInfo(ss.str());
			}*/
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
			XMMATRIX world = DirectX::XMMatrixTranslation(0.0f, 0.0f, 5.0f);
			m_constantBufferVS.getData().worldToClip = world * camPtr->getViewMatrix() * camPtr->getProjMatrix();
			m_constantBufferVS.getData().worldToClip = DirectX::XMMatrixTranspose(m_constantBufferVS.getData().worldToClip);
			m_constantBufferVS.fill();
			d3d::s_devcon->VSSetConstantBuffers(0, 1, m_constantBufferVS.getBufferAddress());

			// PS constant Buffer
			m_constantBufferPS.getData().gResolution = { renderData.iResolutionX, renderData.iResolutionY, renderData.invResolutionX, renderData.invResolutionY };
			m_constantBufferPS.getData().gTime = renderData.iTime;
			m_constantBufferPS.fill();
			d3d::s_devcon->PSSetConstantBuffers(0, 1, m_constantBufferPS.getBufferAddress());

			std::vector<XMFLOAT3> poss =
			{
				{3.0f, 0.0f, 0.0f},
				{-3.0f, 0.0f, 0.0f}
			};

			if (!m_instanceBuffer.map()) {
				return;
			}
			XMFLOAT3* dst = static_cast<XMFLOAT3*>(m_instanceBuffer.getMappedBuffer().pData);
			dst[0] = { 3.0f, 0.0f, 0.0f };
			dst[1] = { -5.0f, 0.0f, 0.0f };
			m_instanceBuffer.unmap();

			// Bind the buffer
			m_vertexBuffer.bind();
			m_instanceBuffer.bind();
			m_indexBuffer.bind();

			// Draw(vertexCount and startLocation)
			//d3d::s_devcon->DrawIndexed(m_indexBuffer.getBufferSize(), 0, 0);
			//for (size_t i = 0; i < 2; ++i) {
			d3d::s_devcon->DrawIndexedInstanced(36, 2, 0, 0, 0);
			//}
		}

		void Graphics::m_initShaders() {

			// Interleaved layout for now
			D3D11_INPUT_ELEMENT_DESC layout[] = {
				{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"INSTANCEPOS", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1}
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
				Vertex{{1.0f, 1.0f, -1.0f}}, // top-right-front
				Vertex{{-1.0f, 1.0f, -1.0f}}, // top-left-front
				Vertex{{1.0f, -1.0f, -1.0f}}, // bottom-right-front
				Vertex{{-1.0f, -1.0f, -1.0f}}, // bottom-left-front

				Vertex{{1.0f, 1.0f, 1.0f}}, // top-right-back
				Vertex{{-1.0f, 1.0f, 1.0f}}, // top-left-back
				Vertex{{1.0f, -1.0f, 1.0f}}, // bottom-right-back
				Vertex{{-1.0f, -1.0f, 1.0f}}, // bottom-left-back
			};
			std::vector<DWORD> indices =
			{
				1, 0, 2, // front right
				1, 2, 3, // front left

				2, 0, 4, // right left
				2, 4, 6, // right right

				3, 5, 1, // left right
				3, 7, 5, // left left

				6, 5, 7, // back right
				6, 4, 5, // back left

				1, 5, 4, // top right
				1, 4, 0, // top left

				3, 2, 6, // bottom right
				3, 6, 7, // back left
			};

			m_vertexBuffer.init(vertices);
			m_indexBuffer.init(indices);
			m_instanceBuffer.init(2); // yeah
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