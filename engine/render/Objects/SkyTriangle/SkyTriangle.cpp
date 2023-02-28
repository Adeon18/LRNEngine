#include "SkyTriangle.hpp"

#include "utils/Logger/Logger.hpp"

namespace engn {
	namespace rend {
		void SkyTriangle::init(const std::string& texturePath)
		{
			// Init Pipeline
			const std::wstring exeDirW = util::getExeDirW();
			PipelineData skyPipelineData{
				nullptr,
				0,
				D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				exeDirW + VS_NAME,
				L"", L"", L"",
				exeDirW + PS_NAME
			};
			initPipeline(m_skyPipeline, skyPipelineData);
			// Init constant buffers
			m_skyBuffer.init();
			
			// Load texture
			if (!tex::TextureManager::getInstance().loadTextureDDS(texturePath)) {
				return;
			}
			m_skyBoxTexture = tex::TextureManager::getInstance().getTexture(texturePath);
			m_initialized = true;

			// Initialize Depth Stensil state
			initDepthStensil();
		}
		void SkyTriangle::render(std::unique_ptr<EngineCamera>& camPtr)
		{
			if (!m_initialized) {
				Logger::instance().logWarn("SkyTriangle::The texture for a TextureCube is not bound");
				return;
			}
			// Turn off IL
			d3d::s_devcon->IASetInputLayout(NULL);

			// Fill and bind CB
			std::vector<XMVECTOR> frustumFarPlaneCoords;
			camPtr->getCamFarPlaneDirForFullScreenTriangle(frustumFarPlaneCoords);

			m_skyBuffer.getData().BLFarPlane = frustumFarPlaneCoords[0];
			m_skyBuffer.getData().BRFarPlane = frustumFarPlaneCoords[3];
			m_skyBuffer.getData().TLFarPlane = frustumFarPlaneCoords[1];

			m_skyBuffer.fill();
			// Set depth buffer write to 0
			d3d::s_devcon->OMSetDepthStencilState(m_skyDepthStensilState.Get(), 0);

			d3d::s_devcon->VSSetConstantBuffers(0, 1, m_skyBuffer.getBufferAddress());
			// Bind a texture
			d3d::s_devcon->PSSetShaderResources(0, 1, m_skyBoxTexture->textureView.GetAddressOf());
			// Bind the pipeline
			bindPipeline(m_skyPipeline);
			// Draw call
			d3d::s_devcon->Draw(3, 0);
		}
		void SkyTriangle::initDepthStensil()
		{
			D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc{};

			depthStencilStateDesc.DepthEnable = true;
			// Zero here becaus ewe disable depth writing for skybox
			depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;
			depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER_EQUAL;

			HRESULT hr = d3d::s_device->CreateDepthStencilState(&depthStencilStateDesc, m_skyDepthStensilState.GetAddressOf());
			if (FAILED(hr)) {
				Logger::instance().logErr("SkyTriangle CreateDepthStencilState fail: " + std::system_category().message(hr));
				return;
			}
		}
	} // rend
} // engn