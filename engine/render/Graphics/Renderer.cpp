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
			m_initScene();
			m_initializeSky();
			m_initPostProcess();
		}

		void Renderer::renderFrame(std::unique_ptr<EngineCamera>& camPtr, std::unique_ptr<win::Window<WIN_WIDTH_DEF, WIN_HEIGHT_DEF>>& winPtr, const RenderData& renderData, const RenderModeFlags& flags)
		{
			// ---- Clear the write buffer
			//winPtr->bindAndClearInitialRTV(BG_COLOR);
			winPtr->bindAndClearInitialRTV(BG_COLOR);

			// ---- Render ----
			m_fillPerFrameCBs(camPtr, renderData);
			m_bindSamplers();
			LightSystem::getInstance().bindLighting(camPtr, flags);
			MeshSystem::getInstance().render(flags);
			// Render the sky after we are done
			m_skyTriangle.render(camPtr);

			// ---- Post Process ----
			winPtr->bindAndClearBackbuffer(BG_COLOR);
			m_postProcess.ressolve(winPtr->getHDRRTVRef());
		}

		void Renderer::m_initScene()
		{
			// TODO: Later may move to some map
#ifdef _WIN64 
			const std::string CUBE_MODEL_PATH = "../../assets/Models/Cube/Cube.fbx";
			const std::string SPHERE_MODEL_PATH = "../../assets/Models/Sphere/sphere.fbx";
			const std::string HORSE_MODEL_PATH = "../../assets/Models/KnightHorse/KnightHorse.fbx";
			const std::string SAMURAI_MODEL_PATH = "../../assets/Models/Samurai/Samurai.fbx";
			const std::string TOWER_MODEL_PATH = "../../assets/Models/EastTower/EastTower.fbx";
			const std::string MAXWELL_MODEL_PATH = "../../assets/Models/Maxwell/source/Maxwell.fbx";

			const std::string BRICK_TEXTURE_PATH = "..\\..\\assets\\Textures\\128x128\\Bricks\\Bricks_06-128x128.dds";
			const std::string CRATE_TEXTURE_PATH = "..\\..\\assets\\Textures\\128x128\\Crate\\crate.dds";
#else
			const std::string CUBE_MODEL_PATH = "../assets/Models/Cube/Cube.fbx";
			const std::string SPHERE_MODEL_PATH = "../assets/Models/Sphere/sphere.fbx";
			const std::string HORSE_MODEL_PATH = "../assets/Models/KnightHorse/KnightHorse.fbx";
			const std::string SAMURAI_MODEL_PATH = "../assets/Models/Samurai/Samurai.fbx";
			const std::string TOWER_MODEL_PATH = "../assets/Models/EastTower/EastTower.fbx";
			const std::string MAXWELL_MODEL_PATH = "../assets/Models/Maxwell/maxwell.fbx";

			const std::string BRICK_TEXTURE_PATH = "..\\assets\\Textures\\128x128\\Bricks\\Bricks_06-128x128.dds";
			const std::string CRATE_TEXTURE_PATH = "..\\assets\\Textures\\128x128\\Crate\\crate.dds";
#endif // !_WIN64

			const std::string EXE_DIR = util::getExeDir();

			LightSystem::getInstance().addDirLight(
				{ 0.0f, -0.8f, 0.6f }, light::INTENSITY_0P05, light::WHITE
			);
			LightSystem::getInstance().addPointLight(
				XMMatrixTranslation(5.0f, 5.0f, 7.0f), light::INTENSITY_DEFAULT, light::LIGHT_DIST_50, light::WHITE
			);
			LightSystem::getInstance().addPointLight(
				XMMatrixTranslation(-5.0f, 5.0f, 7.0f), light::INTENSITY_DEFAULT, light::LIGHT_DIST_50, light::WHITE
			);
			LightSystem::getInstance().addPointLight(
				XMMatrixTranslation(0.0f, 0.0f, 8.0f), light::INTENSITY_2X, light::LIGHT_DIST_20, light::RED
			);

			LightSystem::getInstance().setSpotLightSettings(
				17.0f, light::INTENSITY_1P5X, light::LIGHT_DIST_50, light::WHITE
			);

			std::shared_ptr<mdl::Model> mptr = mdl::ModelManager::getInstance().getModel(EXE_DIR + TOWER_MODEL_PATH);
			MeshSystem::getInstance().addNormalInstance(
				mptr,
				{ },
				{ XMMatrixTranslation(0.0f, 0.0f, 10.0f), {},  {1.0f, 0.0f, 0.0f, 1.0f} }
			);

			mptr.reset();
			mptr = mdl::ModelManager::getInstance().getModel(EXE_DIR + CUBE_MODEL_PATH);
			MeshSystem::getInstance().addNormalInstance(
				mptr,
				{ tex::TextureManager::getInstance().getTexture(util::getExeDir() + BRICK_TEXTURE_PATH) },
				{ XMMatrixTranslation(-7.0f, 0.0f, 10.0f), {}, {1.0f, 0.0f, 0.0f, 1.0f} }
			);

			mptr.reset();
			mptr = mdl::ModelManager::getInstance().getModel(EXE_DIR + SAMURAI_MODEL_PATH);
			MeshSystem::getInstance().addNormalInstance(
				mptr,
				{ },
				{ XMMatrixTranslation(5.0f, 0.0f, 10.0f), {}, {1.0f, 0.0f, 0.0f, 1.0f} }
			);

			mptr.reset();
			mptr = mdl::ModelManager::getInstance().getModel(EXE_DIR + HORSE_MODEL_PATH);
			MeshSystem::getInstance().addNormalInstance(
				mptr,
				{ },
				{ XMMatrixTranslation(-5.0f, 0.0f, 10.0f), {}, {1.0f, 0.0f, 0.0f, 1.0f} }
			);

			mptr.reset();
			mptr = mdl::ModelManager::getInstance().getModel(EXE_DIR + HORSE_MODEL_PATH);
			MeshSystem::getInstance().addNormalInstance(
				mptr,
				{ },
				{ XMMatrixTranslation(-3.0f, 0.0f, 10.0f), {}, {1.0f, 0.0f, 0.0f, 1.0f} }
			);

			mptr.reset();
			mptr = mdl::ModelManager::getInstance().getModel(EXE_DIR + CUBE_MODEL_PATH);
			MeshSystem::getInstance().addNormalInstance(
				mptr,
				{ tex::TextureManager::getInstance().getTexture(util::getExeDir() + CRATE_TEXTURE_PATH) },
				{ XMMatrixTranslation(7.0f, 0.0f, 10.0f), {}, {1.0f, 0.0f, 0.0f, 1.0f} }
			);

			//// Fill the field with cubes
			//for (int i = -32; i < 32; ++i) {
			//	for (int j = 0; j < 32; ++j) {
			//		mptr.reset();
			//		mptr = mdl::ModelManager::getInstance().getModel(EXE_DIR + CUBE_MODEL_PATH);
			//		MeshSystem::getInstance().addNormalInstance(mptr, {}, { XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(i * 2.0f, 0.0f, 12.0f + j * 2.0f), {1.0f, 0.0f, 0.0f, 1.0f} });
			//	}
			//}

			//mptr.reset();
			//mptr = mdl::ModelManager::getInstance().getModel(EXE_DIR + SAMURAI_MODEL_PATH);
			//MeshSystem::getInstance().addHologramInstance(mptr, {}, { XMMatrixScaling(0.05f, 0.05f, 0.05f) * XMMatrixRotationRollPitchYaw(0.0f, XM_PI, 0.0f) * XMMatrixTranslation(-8.0f, 0.0f, 15.0f), {1.0f, 0.0f, 0.0f, 1.0f}});

			//mptr.reset();
			//mptr = mdl::ModelManager::getInstance().getModel(EXE_DIR + HORSE_MODEL_PATH);
			//MeshSystem::getInstance().addHologramInstance(mptr, {}, { XMMatrixScaling(0.05f, 0.05f, 0.05f) * XMMatrixRotationRollPitchYaw(0.0f, XM_PI, 0.0f) * XMMatrixTranslation(0.0f, 0.0f, 15.0f), {0.0f, 1.0f, 0.0f, 1.0f} });

			//mptr.reset();
			//mptr = mdl::ModelManager::getInstance().getModel(EXE_DIR + SAMURAI_MODEL_PATH);
			//MeshSystem::getInstance().addHologramInstance(mptr, {}, { XMMatrixScaling(0.05f, 0.05f, 0.05f) * XMMatrixRotationRollPitchYaw(0.0f, XM_PI, 0.0f) * XMMatrixTranslation(8.0f, 0.0f, 15.0f), {0.0f, 0.0f, 1.0f, 1.0f} });
			
			// Fill the field with samurai
			/*for (int i = 0; i < 32; ++i) {
				for (int j = 0; j < 32; ++j) {
					mptr.reset();
					mptr = mdl::ModelManager::getInstance().getModel(util::getExeDir() + "../../assets/Models/Samurai/Samurai.fbx");
					MeshSystem::getInstance().addHologramInstance(mptr, {}, { XMMatrixScaling(0.05f, 0.05f, 0.05f) * XMMatrixRotationRollPitchYaw(0.0f, XM_PI, 0.0f) * XMMatrixTranslation(i * 3.0f, 0.0f, j * 3.0f), {1.0f, 0.0f, 0.0f, 1.0f} });
				}
			}*/
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
			m_globalConstantBufferPS.getData().worldToClip = XMMatrixTranspose( camPtr->getViewMatrix() * camPtr->getProjMatrix() );
			m_globalConstantBufferPS.getData().gResolution = gResolution;
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
#ifdef _WIN64 
			const std::string SKYBOX_TEXTURE_PATH = util::getExeDir() + "..\\..\\assets\\Textures\\SkyBoxes\\night_street.dds";
#else
			const std::string SKYBOX_TEXTURE_PATH = util::getExeDir() + "..\\assets\\Textures\\SkyBoxes\\night_street.dds";
#endif // !_WIN64

			m_skyTriangle.init(SKYBOX_TEXTURE_PATH);
		}
		void Renderer::m_initPostProcess()
		{
			m_postProcess.init();
		}
	} // rend
} // engn