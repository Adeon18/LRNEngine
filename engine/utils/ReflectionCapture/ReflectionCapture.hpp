#pragma once

#include <array>

#include "render/Graphics/Vertex.hpp"
#include "render/Systems/Pipeline.hpp"

#include "render/Graphics/DXBuffers/IndexBuffer.hpp"
#include "render/Graphics/DXBuffers/VertexBuffer.hpp"
#include "render/Graphics/DXBuffers/ConstantBuffer.hpp"

#include "utils/TextureManager/TextureManager.hpp"

namespace engn {
	namespace rend {		
		class ReflectionCapture {
			const std::wstring DI_AND_PFS_VS_NAME = L"VSDIPFS.cso";
			const std::wstring DI_PS_NAME = L"PSDiffuseIrradiance.cso";
			const std::wstring PFS_PS_NAME = L"PSPreFilteredSpecular.cso";
			const std::wstring BRDF_VS_NAME = L"VSBRDFIntegration.cso";
			const std::wstring BRDF_PS_NAME = L"PSBRDFIntegration.cso";
		public:
			//! Initialize everything from the reflection capture
			void init(const std::string& skyTexturePat);
			//! Generate and save to .dds the diffuse irradiance cubemap
			void generateDiffuseIrradianceCubemap(const XMMATRIX& projection);
			//! Generate and save the pre filtered cubemap
			void generatePreFilteredSpecularCubemap(const XMMATRIX& projection);
			//! Generate and save the BRDF integration texture
			void generateBRDFIntegrationTexture();
		private:
			//! Initialize the inside out cube buffers
			void initCubeBuffers();
			//! Init all the pipelines
			void initPipelines();

			void initDiffuseIrradianceCubeMap();
			//! Render the inside out cube
			void renderCube();
		private:
			Pipeline m_diffuseIrradiancePipeline;
			Pipeline m_preFilteredSpecularPipeline;
			Pipeline m_BRDFIntegrationPipeline;

			VertexBuffer<VertexPos> m_cubeVertexBuffer;
			IndexBuffer m_cubeIndexBuffer;

			ConstantBuffer<CB_VS_WorldToClip> m_worldToClipBuffer;

			Microsoft::WRL::ComPtr<ID3D11Texture2D> m_diffuseIrradianceCubemap;
			std::array<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>, 6> m_currentDIRTVs;

			std::shared_ptr<tex::Texture> m_skyBoxTexture;

			const std::array<XMMATRIX, 6> CAMERA_CAPTURE_VIEWS{
				XMMatrixLookAtLH({0.0f, 0.0f, 0.0f}, {1.0f,  0.0f,  0.0f}, {0.0f, 1.0f, 0.0f}),
				XMMatrixLookAtLH({0.0f, 0.0f, 0.0f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f, 0.0f}),
				XMMatrixLookAtLH({0.0f, 0.0f, 0.0f}, {0.0f,  1.0f,  0.0f}, {0.0f, 0.0f, -1.0f}),
				XMMatrixLookAtLH({0.0f, 0.0f, 0.0f}, {0.0f,  -1.0f,  0.0f}, {1.0f, 0.0f, 0.0f}),
				XMMatrixLookAtLH({0.0f, 0.0f, 0.0f}, {0.0f,  0.0f,  1.0f}, {0.0f, 1.0f, 0.0f}),
				XMMatrixLookAtLH({0.0f, 0.0f, 0.0f}, {0.0f,  0.0f,  -1.0f}, {0.0f, 1.0f, 0.0f})
			};

			const XMMATRIX PROJECTION = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.0f), 1.0f, 0.1f, 1000.0f);
		};
	} // rend
} // engn