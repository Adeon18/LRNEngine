#pragma once

#include <unordered_map>

#include "HelperStructs.hpp"

#include "DXBuffers/IndexBuffer.hpp"
#include "DXBuffers/VertexBuffer.hpp"
#include "DXBuffers/ConstantBuffer.hpp"
#include "DXBuffers/InstanceBuffer.hpp"

#include "DXShaders/VertexShader.hpp"
#include "DXShaders/PixelShader.hpp"

#include "EngineCamera.hpp"

#include "windows/Window.h"

#include "render/Graphics/PostProcess/PostProcess.hpp"
#include "render/Objects/SkyTriangle/SkyTriangle.hpp"
#include "DXTextures/Sampler.hpp"

#include "utils/ReflectionCapture/ReflectionCapture.hpp"

#include "include/config.hpp"


namespace engn {
	namespace rend {
		struct RenderData {
			float iTime;
			float iResolutionX;
			float iResolutionY;
			float invResolutionX;
			float invResolutionY;
		};

		/// <summary>
		/// This class facilitates everything needed for the rendering pipeline
		/// </summary>
		class Renderer {
			inline static float BG_COLOR[] = { 0.2f, 0.2f, 0.2f, 1.0f };
#ifdef _WIN64
			const std::string TEX_REL_PATH_PREF = util::getExeDir() + "..\\..\\";
#else
			const std::string TEX_REL_PATH_PREF = util::getExeDir() + "..\\";
#endif
		public:
			void init();
			//! Render entire frame, return false at error or cubamap baking to entirely shut down the engine and free the resources
			bool renderFrame(std::unique_ptr<EngineCamera>& camPtr, std::unique_ptr<win::Window<WIN_WIDTH_DEF, WIN_HEIGHT_DEF>>& winPtr, const RenderData& renderData, const RenderModeFlags& flags);
		private:
			void m_initScene();
			//! Initialize all the per Frame constant buffers
			void m_initBuffers();
			//! Initialize all the global samplers
			void m_initSamplers();
			//! Bind samplers per frame: TODO: Can they be binded just at initialization?
			void m_bindSamplers();
			//! Fill the per frame shader CB, for not the same for each shader
			void m_fillPerFrameCBs(std::unique_ptr<EngineCamera>& camPtr, const RenderData& renderData);
			//! Initialize the skyTriangle
			void m_initializeSky();
			//! Initialize the postprocess class
			void m_initPostProcess();
			//! Constant buffers that are applied to each vertex and pixel shader per frame
			ConstantBuffer<CB_VS_RealTimeData> m_globalConstantBufferVS;
			ConstantBuffer<CB_PS_RealTimeData> m_globalConstantBufferPS;

			Sampler m_samplerPointWrap;
			Sampler m_samplerLinearWrap;
			Sampler m_samplerAnisotropicWrap;

			std::shared_ptr<tex::Texture> m_diffuseIrradianceMap;
			std::shared_ptr<tex::Texture> m_preFilteredSpecularMap;
			std::shared_ptr<tex::Texture> m_BRDFIntegrationTex;

			SkyTriangle m_skyTriangle;
			PostProcess m_postProcess;
#if BAKE_CUBEMAPS == 1
			ReflectionCapture m_reflectionCapture;
#endif

			std::unordered_map<std::string, MaterialTexturePaths> MATERIALS{
				{"STONE", 
					{
						TEX_REL_PATH_PREF + "assets\\Textures\\Stone\\Stone_COLOR.dds",
						TEX_REL_PATH_PREF + "assets\\Textures\\Stone\\Stone_NORM.dds",
						TEX_REL_PATH_PREF + "assets\\Textures\\Stone\\Stone_ROUGH.dds",
						""
					}
				},
				{"COBBLESTONE",
					{
						TEX_REL_PATH_PREF + "assets\\Textures\\Cobblestone\\Cobblestone_albedo.dds",
						TEX_REL_PATH_PREF + "assets\\Textures\\Cobblestone\\Cobblestone_normal.dds",
						TEX_REL_PATH_PREF + "assets\\Textures\\Cobblestone\\Cobblestone_roughness.dds",
						""
					}
				},
			};

			std::unordered_map<std::string, std::string> MODELS{
				{"HORSE", TEX_REL_PATH_PREF + "assets/Models/KnightHorse/KnightHorse.fbx"},
				{"SAMURAI", TEX_REL_PATH_PREF + "assets/Models/Samurai/Samurai.fbx"},
				{"TOWER", TEX_REL_PATH_PREF + "assets/Models/EastTower/EastTower.fbx"},
				{"CUBE", TEX_REL_PATH_PREF + "assets/Models/Cube/Cube.fbx"},
				{"SPHERE", TEX_REL_PATH_PREF + "assets/Models/Sphere/sphere.fbx"},
			};

			Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStensilState;
			Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;
		};
	} // render
} // engn