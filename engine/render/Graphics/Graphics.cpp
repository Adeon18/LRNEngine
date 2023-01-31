#include <iostream>
#include <sstream>
#include <vector>

#include "Graphics.hpp"

#include "render/Systems/MeshSystem.hpp"

#include "include/utility/utility.hpp"

#include "utils/ModelManager/ModelManager.hpp"

namespace engn {
	namespace rend {
		void Graphics::init() {
			m_initRasterizer();
			m_initDepthStencilState();
			m_initScene();
		}

		void Graphics::renderFrame(std::unique_ptr<EngineCamera>& camPtr, const RenderData& renderData)
		{
			// Set Input Assembler Data
			d3d::s_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			d3d::s_devcon->RSSetState(m_rasterizerState.Get());
			d3d::s_devcon->OMSetDepthStencilState(m_depthStensilState.Get(), 0);

			MeshSystem::getInstance().render(camPtr->getViewMatrix() * camPtr->getProjMatrix());

			// PS constant Buffer
			m_constantBufferPS.getData().gResolution = { renderData.iResolutionX, renderData.iResolutionY, renderData.invResolutionX, renderData.invResolutionY };
			m_constantBufferPS.getData().gTime = renderData.iTime;
			m_constantBufferPS.fill();
			d3d::s_devcon->PSSetConstantBuffers(0, 1, m_constantBufferPS.getBufferAddress());
		}

		void Graphics::m_initScene()
		{
			m_constantBufferPS.init();


			std::shared_ptr<mdl::Model> mptr = mdl::ModelManager::getInstance().getCubeModel();
			MeshSystem::getInstance().addNormalInstance(mptr, {}, { XMMatrixTranslation(-5.0f, 0.0f, 10.0f), {1.0f, 0.0f, 0.0f, 1.0f} });

			mptr.reset();
			mptr = mdl::ModelManager::getInstance().getCubeModel();
			MeshSystem::getInstance().addNormalInstance(mptr, {}, { XMMatrixTranslation(0.0f, 0.0f, 10.0f), {0.0f, 1.0f, 0.0f, 1.0f} });

			mptr.reset();
			mptr = mdl::ModelManager::getInstance().getCubeModel();
			MeshSystem::getInstance().addNormalInstance(mptr, {}, { XMMatrixTranslation(5.0f, 0.0f, 10.0f), {0.0f, 0.0f, 1.0f, 1.0f} });

			mptr.reset();
			mptr = mdl::ModelManager::getInstance().getModel(util::getExeDir() + "../../assets/Models/KnightHorse/KnightHorse.fbx");
			for (auto& m : mptr->getMeshes()) {
				std::stringstream ss;
				ss << "Name: " << m.name << std::endl;
				ss << "Vertice num: " << m.vertices.size() << std::endl;
				ss << "Trinagles num: " << m.triangles.size() << std::endl;
				ss << "Instances size: " << m.instances.size() << std::endl;
				ss << "Instances Inverse size: " << m.instancesInv.size() << std::endl;
				Logger::instance().logInfo(ss.str());
			}

			MeshSystem::getInstance().addNormalInstance(mptr, {}, { XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixTranslation(0.0f, 0.0f, 30.0f), {0.0f, 0.0f, 1.0f, 1.0f}});
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