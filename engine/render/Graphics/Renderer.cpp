#include <iostream>
#include <sstream>
#include <vector>

#include "Renderer.hpp"

#include "render/Systems/MeshSystem.hpp"
#include "render/Systems/LightSystem.hpp"

#include "include/utility/utility.hpp"

#include "utils/TextureManager/TextureManager.hpp"
#include "utils/ModelManager/ModelManager.hpp"

namespace engn {
	namespace rend {
		void Renderer::init() {
			m_initBuffers();
			m_initSamplers();
			m_initializeSky();
			m_initPostProcess();
#if BAKE_CUBEMAPS == 1
			std::vector<std::string> cubemapsToBake{
				TEX_REL_PATH_PREF + "assets\\Textures\\SkyBoxes\\grass_field.dds",
				TEX_REL_PATH_PREF + "assets\\Textures\\SkyBoxes\\mountains.dds",
				TEX_REL_PATH_PREF + "assets\\Textures\\SkyBoxes\\lake_beach.dds",
				TEX_REL_PATH_PREF + "assets\\Textures\\SkyBoxes\\night_street.dds",
			};
			m_reflectionCapture.init(cubemapsToBake);
#endif
		}

		bool Renderer::renderFrame(std::unique_ptr<EngineCamera>& camPtr, std::unique_ptr<win::Window<WIN_WIDTH_DEF, WIN_HEIGHT_DEF>>& winPtr, const RenderData& renderData, const RenderModeFlags& flags)
		{
			if (UI::instance().getShadowWidgetData().enabled) {
				MeshSystem::getInstance().renderDepth2D();
				MeshSystem::getInstance().renderDepthCubemaps();
			}
			// ---- Clear the write buffer
			winPtr->bindAndClearInitialRTV(BG_COLOR);

			UI::instance().manageMenus();

			m_bindSamplers();

#if BAKE_CUBEMAPS == 1
			m_reflectionCapture.checkHemisphereIntegral();
			m_reflectionCapture.generateDiffuseIrradianceCubemap();
			m_reflectionCapture.generatePreFilteredSpecularCubemap();
			m_reflectionCapture.generateBRDFIntegrationTexture();
			return false;
#endif
			// Render the sky before everything(so that the transparency works)
			m_skyTriangle.render(camPtr);

			// ---- Render ----
			m_fillPerFrameCBs(camPtr, renderData);

			d3d::s_devcon->PSSetShaderResources(6, 1, m_diffuseIrradianceMap->textureView.GetAddressOf());
			d3d::s_devcon->PSSetShaderResources(7, 1, m_preFilteredSpecularMap->textureView.GetAddressOf());
			d3d::s_devcon->PSSetShaderResources(8, 1, m_BRDFIntegrationTex->textureView.GetAddressOf());

			LightSystem::getInstance().bindLighting(camPtr, flags);
			MeshSystem::getInstance().render(flags);

			// ---- Post Process ----
			winPtr->bindAndClearBackbuffer(BG_COLOR);
			m_postProcess.ressolve(winPtr->getHDRRTVRef());

			UI::instance().endFrame();

			return true;
		}

		void Renderer::m_initBuffers()
		{
			m_globalConstantBufferVS.init();
			m_globalConstantBufferPS.init();
		}
		void Renderer::m_initSamplers()
		{
			m_samplerPointWrap.init(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
			m_samplerLinearWrap.init(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);
			m_samplerAnisotropicWrap.init(D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP);
		}
		void Renderer::m_bindSamplers()
		{
			m_samplerPointWrap.bind(0);
			m_samplerLinearWrap.bind(1);
			m_samplerAnisotropicWrap.bind(2);
		}
		void Renderer::m_fillPerFrameCBs(std::unique_ptr<EngineCamera>& camPtr, const RenderData& renderData)
		{
			// General Data constant buffer
			const XMFLOAT4 gResolution = { renderData.iResolutionX, renderData.iResolutionY, renderData.invResolutionX, renderData.invResolutionY };

			//! Fill global constant VS CB
			m_globalConstantBufferVS.getData().worldToClip = XMMatrixTranspose( camPtr->getViewMatrix() * camPtr->getProjMatrix() );
			m_globalConstantBufferVS.getData().gResolution = gResolution;
			XMStoreFloat4(&(m_globalConstantBufferVS.getData().gCameraPosition), camPtr->getCamPosition());
			m_globalConstantBufferVS.getData().gTime = renderData.iTime;

			//! Fill global constant PS CB
			const float res = ReflectionCapture::PFS_TEXTURE_DIMENSION;
			m_globalConstantBufferPS.getData().gResolution = gResolution;
			m_globalConstantBufferPS.getData().gPFSCubemapResolution = { res, res, res, res };
			XMStoreFloat4(&(m_globalConstantBufferPS.getData().gCameraPosition), camPtr->getCamPosition());
			m_globalConstantBufferPS.getData().gTime = renderData.iTime;
			
			m_globalConstantBufferVS.fill();
			m_globalConstantBufferPS.fill();
			d3d::s_devcon->VSSetConstantBuffers(0, 1, m_globalConstantBufferVS.getBufferAddress());
			// For now it is like this
			d3d::s_devcon->GSSetConstantBuffers(0, 1, m_globalConstantBufferVS.getBufferAddress());
			d3d::s_devcon->PSSetConstantBuffers(0, 1, m_globalConstantBufferPS.getBufferAddress());
		}
		void Renderer::m_initializeSky()
		{
			// All the skybox textures
			//const std::string skyBoxTexturePath = TEX_REL_PATH_PREF + "assets\\Textures\\SkyBoxes\\night_street.dds";
			//const std::string skyBoxTexturePath = TEX_REL_PATH_PREF + "assets\\Textures\\SkyBoxes\\grass_field.dds";
			const std::string skyBoxTexturePath = TEX_REL_PATH_PREF + "assets\\Textures\\SkyBoxes\\mountains.dds";
			//const std::string skyBoxTexturePath = TEX_REL_PATH_PREF + "assets\\Textures\\SkyBoxes\\lake_beach.dds";

			m_diffuseIrradianceMap = tex::TextureManager::getInstance().getTexture(util::removeFileExt(skyBoxTexturePath) + ReflectionCapture::DI_TEXTURE_SUFFIX);
			m_preFilteredSpecularMap = tex::TextureManager::getInstance().getTexture(util::removeFileExt(skyBoxTexturePath) + ReflectionCapture::PFS_TEXTURE_SUFFIX);
			m_BRDFIntegrationTex = tex::TextureManager::getInstance().getTexture(TEX_REL_PATH_PREF + "assets\\Textures\\SkyBoxes\\" + ReflectionCapture::BRDFI_TEXTURE_NAME);

			m_skyTriangle.init(skyBoxTexturePath);
		}
		void Renderer::m_initPostProcess()
		{
			m_postProcess.init();
		}
	} // rend
} // engn