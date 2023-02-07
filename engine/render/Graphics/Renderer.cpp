#include <iostream>
#include <sstream>
#include <vector>

#include "Renderer.hpp"

#include "render/Systems/MeshSystem.hpp"

#include "include/utility/utility.hpp"

#include "utils/ModelManager/ModelManager.hpp"

namespace engn {
	namespace rend {
		void Renderer::init() {
			m_initRasterizer();
			m_initDepthStencilState();
			m_initScene();
		}

		void Renderer::renderFrame(std::unique_ptr<EngineCamera>& camPtr, const RenderData& renderData)
		{
			// Set Input Assembler Data
			d3d::s_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			d3d::s_devcon->RSSetState(m_rasterizerState.Get());
			d3d::s_devcon->OMSetDepthStencilState(m_depthStensilState.Get(), 0);

			m_fillPerFrameCBs(camPtr, renderData);

			MeshSystem::getInstance().render(camPtr->getViewMatrix() * camPtr->getProjMatrix());
		}

		void Renderer::m_initScene()
		{
			m_constantBufferVS.init();

			// TODO: Later may move to some map
#ifdef _WIN64 
			const std::string SAMURAI_MODEL_PATH = "../../assets/Models/KnightHorse/KnightHorse.fbx";
#else
			const std::string SAMURAI_MODEL_PATH = "../assets/Models/KnightHorse/KnightHorse.fbx";
#endif // !_WIN64


			std::shared_ptr<mdl::Model> mptr = mdl::ModelManager::getInstance().getCubeModel();
			MeshSystem::getInstance().addNormalInstance(mptr, {}, { XMMatrixTranslation(-5.0f, 0.0f, 8.0f), {1.0f, 0.0f, 0.0f, 1.0f} });

			mptr.reset();
			mptr = mdl::ModelManager::getInstance().getCubeModel();
			MeshSystem::getInstance().addNormalInstance(mptr, {}, { XMMatrixTranslation(0.0f, 0.0f, 8.0f), {0.0f, 1.0f, 0.0f, 1.0f} });

			mptr.reset();
			mptr = mdl::ModelManager::getInstance().getCubeModel();
			MeshSystem::getInstance().addNormalInstance(mptr, {}, { XMMatrixTranslation(5.0f, 0.0f, 8.0f), {0.0f, 0.0f, 1.0f, 1.0f} });

			// Fill the field with cubes
			for (int i = -32; i < 32; ++i) {
				for (int j = 0; j < 32; ++j) {
					mptr.reset();
					mptr = mdl::ModelManager::getInstance().getCubeModel();
					MeshSystem::getInstance().addNormalInstance(mptr, {}, { XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(i * 2.0f, 0.0f, 12.0f + j * 2.0f), {1.0f, 0.0f, 0.0f, 1.0f} });
				}
			}

			mptr.reset();
			mptr = mdl::ModelManager::getInstance().getModel(util::getExeDir() + SAMURAI_MODEL_PATH);
			MeshSystem::getInstance().addHologramInstance(mptr, {}, { XMMatrixScaling(0.05f, 0.05f, 0.05f) * XMMatrixRotationRollPitchYaw(0.0f, XM_PI, 0.0f) * XMMatrixTranslation(-5.0f, 0.0f, 10.0f), {1.0f, 0.0f, 0.0f, 1.0f}});

			mptr.reset();
			mptr = mdl::ModelManager::getInstance().getModel(util::getExeDir() + SAMURAI_MODEL_PATH);
			MeshSystem::getInstance().addHologramInstance(mptr, {}, { XMMatrixScaling(0.05f, 0.05f, 0.05f) * XMMatrixRotationRollPitchYaw(0.0f, XM_PI, 0.0f) * XMMatrixTranslation(0.0f, 0.0f, 10.0f), {0.0f, 1.0f, 0.0f, 1.0f} });

			mptr.reset();
			mptr = mdl::ModelManager::getInstance().getModel(util::getExeDir() + SAMURAI_MODEL_PATH);
			MeshSystem::getInstance().addHologramInstance(mptr, {}, { XMMatrixScaling(0.05f, 0.05f, 0.05f) * XMMatrixRotationRollPitchYaw(0.0f, XM_PI, 0.0f) * XMMatrixTranslation(5.0f, 0.0f, 10.0f), {0.0f, 0.0f, 1.0f, 1.0f} });
			
			// Fill the field with samurai
			/*for (int i = 0; i < 32; ++i) {
				for (int j = 0; j < 32; ++j) {
					mptr.reset();
					mptr = mdl::ModelManager::getInstance().getModel(util::getExeDir() + "../../assets/Models/Samurai/Samurai.fbx");
					MeshSystem::getInstance().addHologramInstance(mptr, {}, { XMMatrixScaling(0.05f, 0.05f, 0.05f) * XMMatrixRotationRollPitchYaw(0.0f, XM_PI, 0.0f) * XMMatrixTranslation(i * 3.0f, 0.0f, j * 3.0f), {1.0f, 0.0f, 0.0f, 1.0f} });
				}
			}*/
		}

		void Renderer::m_initRasterizer()
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
		void Renderer::m_initDepthStencilState() {
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
		void Renderer::m_fillPerFrameCBs(std::unique_ptr<EngineCamera>& camPtr, const RenderData& renderData)
		{
			// General Data constant buffer
			m_constantBufferVS.getData().gResolution = { renderData.iResolutionX, renderData.iResolutionY, renderData.invResolutionX, renderData.invResolutionY };
			XMStoreFloat4(&(m_constantBufferVS.getData().gCameraPosition), camPtr->getCamPosition());
			m_constantBufferVS.getData().gTime = renderData.iTime;
			
			m_constantBufferVS.fill();
			d3d::s_devcon->VSSetConstantBuffers(0, 1, m_constantBufferVS.getBufferAddress());
			d3d::s_devcon->PSSetConstantBuffers(0, 1, m_constantBufferVS.getBufferAddress());
		}
	} // rend
} // engn