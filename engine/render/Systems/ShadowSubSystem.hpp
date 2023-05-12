#pragma once

#include <vector>

#include "render/Systems/Pipeline.hpp"

#include "render/Graphics/DXBuffers/ConstantBuffer.hpp"
#include "render/Graphics/DXBuffers/CBStructs.hpp"

#include "render/Graphics/DXRTVs/BindableDepthBuffer.hpp"
#include "render/Graphics/DXRTVs/BindableRenderTarget.hpp"

#include "render/Graphics/DXTextures/Sampler.hpp"

namespace engn {
	namespace rend {
		/// <summary>
		/// This shadow subsystem is not a singleton but a part of meshsystem =>
		/// All inits and managing is called wit the help of the meshsystem
		/// </summary>
		class ShadowSubSystem {
		public:
			//! Init all the pipelines abd buffers
			void init();

			//! Bind all the needed data so that at MeshSystem.normalGroup.render() we will fill the respective shadow buffer
			//! Directional and pointlighs have indexes
			void captureDirectionalShadow(uint32_t idx);
			//! We only have 1 spotlight
			void captureSpotShadow();

			void capturePointShadow(uint32_t idx);

			//! Bind textures and constant buffers to respective slots
			void bindDataAndBuffers();
			//! Unbind the buffers so you can render to them again
			void unbindDepthBuffers();

			//! Both spotlight and pointlights light matrices should be filled repeatedly because their positions change
			//! For OPTIMIZATION PURPOSES THIS FUNCTION SHOULD BE CALLED ONCE BY THE MESHSYSTEM, so would be with
			//! spotlgths but we only have 1 
			void fillPointMatrices();
			void fillSpotMatrices();

			[[nodiscard]] std::vector<BindableDepthBuffer>& getDirectionalLightShadowMaps();
			[[nodiscard]] std::vector<BindableDepthBuffer>& getPointLightShadowMaps();
		private:
			void initDepthBuffers();
			void initPipelines();
			void initBuffers();
			void fillDirectionalMatrices();
			void initSamplers();
			void initAndBindViewPort(uint32_t resolution);
			//! Fill and bind Imgui debug data buffer
			void fillAndBindDebugBuffer();

			//! Comparison sampler
			Sampler m_comparionSampler;

			//! Shadow maps
			std::vector<BindableDepthBuffer> m_directionalShadowMaps;
			std::vector<BindableDepthBuffer> m_pointShadowCubeMaps;
			BindableDepthBuffer m_spotShadowMap;

			//! Main render bind
			ConstantBuffer<CB_PS_ShadowMapToLightMatrices> m_shadowMapProjectionsPSCB;

			//! Shadow gen pipeline/buffers
			Pipeline m_shadow2DPipeline;
			ConstantBuffer<CB_VS_Shadow2DGenBuffer> m_shadow2DVSCB;

			//! Shadow cube map gen pipeline
			Pipeline m_shadowCubemapPipeline;
			ConstantBuffer<CB_GS_ShadowCubeGenBuffer> m_shadowOmniGSCB;

			//! Debug
			ConstantBuffer<CB_PS_ShadowControlFlags> m_shadowDebugData;

			//! Matrices
			std::vector<XMMATRIX> m_directionalViewProjMatrices;
			XMMATRIX m_spotlightViewProjMatrix;
			std::vector<std::array<XMMATRIX, 6>> m_pointViewMatrices;

			//! Temporary constant to tell where are all the objects
			static constexpr XMVECTOR OBJECT_CENTER{ 0.0f, 0.0f, 0.0f };
			inline static XMMATRIX DIRECTIONAL_PROJECTION = XMMatrixOrthographicLH(25, 25, 1000.0f, 0.1f);
			inline static XMMATRIX POINTLIGHT_PROJECTION = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.0f), 1.0f, 1000.0f, 0.1f);

			static constexpr uint32_t DEBUG_BUFFER_SLOT = 11;

			static constexpr uint32_t SHADOW_CUBEMAP_SIDE_RESOLUTION = 512;
			static constexpr uint32_t SHADOW_MAP_RESOLUTION2D = 2048;
			static constexpr uint32_t SHADOW_MAP_MATRICES_BUFFER_SLOT = 4;
			static constexpr uint32_t SPOT_SHADOW_MAP_SLOT = 10;
			static constexpr uint32_t DIRECTIONAL_SHADOW_MAP_SLOT = 11;
			static constexpr uint32_t POINT_SHADOW_MAP_START_SLOT = 12;

			static constexpr XMVECTOR UP_VECTOR{ 0.0f, 1.0f, 0.0f };
			static constexpr XMVECTOR FORWARD_VECTOR{ 0.0f, 0.0f, 1.0f };
			static constexpr XMVECTOR RIGHT_VECTOR{ 1.0f, 0.0f, 0.0f };
		};
	} // rend
} // engn