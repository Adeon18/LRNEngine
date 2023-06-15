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
#include "render/Graphics/DeferredShading/DeferredRessolver.hpp"
#include "render/Objects/SkyTriangle/SkyTriangle.hpp"
#include "DXTextures/Sampler.hpp"

#include "utils/ReflectionCapture/ReflectionCapture.hpp"

#include "include/config.hpp"


namespace engn {
	namespace rend {
		struct RenderData {
			float iTime;
			float iDt;
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
			//! Initialize noise textures for spawning animation
			void m_initializeNoise();
			//! Initialize the postprocess class
			void m_initPostProcess();
			//! Initialize the deferred ressolver
			void m_initDeferred();
			//! Constant buffers that are applied to each vertex and pixel shader per frame
			ConstantBuffer<CB_VS_RealTimeData> m_globalConstantBufferVS;
			ConstantBuffer<CB_PS_RealTimeData> m_globalConstantBufferPS;

			Sampler m_samplerPointWrap;
			Sampler m_samplerLinearWrap;
			Sampler m_samplerAnisotropicWrap;
			Sampler m_samplerBilinearClamp;

			std::shared_ptr<tex::Texture> m_diffuseIrradianceMap;
			std::shared_ptr<tex::Texture> m_preFilteredSpecularMap;
			std::shared_ptr<tex::Texture> m_BRDFIntegrationTex;

			std::shared_ptr<tex::Texture> m_mainDissolutionNoise;

			SkyTriangle m_skyTriangle;
			PostProcess m_postProcess;
			DeferredRessolver m_deferredRessolver;
#if BAKE_CUBEMAPS == 1
			ReflectionCapture m_reflectionCapture;
#endif
			Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStensilState;
			Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;
		};
	} // render
} // engn