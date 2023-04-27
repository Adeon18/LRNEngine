#pragma once

#include <vector>

#include "render/Systems/Pipeline.hpp"

#include "render/Graphics/DXBuffers/ConstantBuffer.hpp"
#include "render/Graphics/DXBuffers/CBStructs.hpp"

#include "render/Graphics/DXRTVs/BindableDepthBuffer.hpp"
#include "render/Graphics/DXRTVs/BindableRenderTarget.hpp"

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

			//! Bind textures and constant buffers to respective slots
			void bindDataAndBuffers();
			//! Unbind the buffers so you can render to them again
			void unbindDepthBuffers();

			[[nodiscard]] std::vector<BindableDepthBuffer>& getDirectionalLightShadowMaps();
		private:
			void initDepthBuffers();
			void initPipelines();
			void initBuffers();
			void fillDirectionalMatrices();
			//! Both spotlight and directional light matrices should be filled repeatedly because their positions change 
			void fillPointMatrices();
			void fillSpotMatrices();
			void initAndBindViewPort(uint32_t resolution);

			//! Shadow maps
			std::vector<BindableDepthBuffer> m_directionalShadowMaps;
			BindableDepthBuffer m_spotShadowMap;

			//! Main render bind
			ConstantBuffer<CB_PS_ShadowMapToLightMatrices> m_shadowMapProjectionsPSCB;

			//! Shadow gen pipeline/buffers
			Pipeline m_shadow2DPipeline;
			ConstantBuffer<CB_VS_Shadow2DGenBuffer> m_shadow2DVSCB;

			//! Matrices
			std::vector<XMMATRIX> m_directionalViewProjMatrices;
			XMMATRIX m_spotlightViewProjMatrix;

			//! Temporary constant to tell where are all the objects
			static constexpr XMVECTOR OBJECT_CENTER{ 0.0f, 0.0f, 9.0f };
			inline static XMMATRIX DIRECTIONAL_PROJECTION = XMMatrixOrthographicLH(20, 20, 1000.0f, 0.1f);

			static constexpr uint32_t SHADOW_MAP_RESOLUTION2D = 2048;
			static constexpr uint32_t SHADOW_MAP_MATRICES_BUFFER_SLOT = 4;
			static constexpr uint32_t SPOT_SHADOW_MAP_SLOT = 10;
			static constexpr uint32_t DIRECTIONAL_SHADOW_MAP_SLOT = 11;
		};
	} // rend
} // engn