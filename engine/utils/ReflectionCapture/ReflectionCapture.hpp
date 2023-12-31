#pragma once

#include <array>
#include <vector>

#include <DirectXTex/DirectXTex.h>

#include "render/Graphics/Vertex.hpp"
#include "render/Systems/Pipeline.hpp"

#include "render/Graphics/DXBuffers/IndexBuffer.hpp"
#include "render/Graphics/DXBuffers/VertexBuffer.hpp"
#include "render/Graphics/DXBuffers/ConstantBuffer.hpp"

#include "utils/TextureManager/TextureManager.hpp"

namespace engn {
	namespace rend {	
		enum class FileFormat
		{
			NONE,
			PNG,
			TGA,
			HDR,
			BC1_LINEAR = DXGI_FORMAT_BC1_UNORM,			// RGB, 1 bit Alpha
			BC1_SRGB = DXGI_FORMAT_BC1_UNORM_SRGB,		// RGB, 1-bit Alpha, SRGB
			BC3_LINEAR = DXGI_FORMAT_BC3_UNORM,			// RGBA
			BC3_SRGB = DXGI_FORMAT_BC3_UNORM_SRGB,		// RGBA, SRGB
			BC4_UNSIGNED = DXGI_FORMAT_BC4_UNORM,		// GRAY, unsigned
			BC4_SIGNED = DXGI_FORMAT_BC4_SNORM,			// GRAY, signed
			BC5_UNSIGNED = DXGI_FORMAT_BC5_UNORM,		// RG, unsigned
			BC5_SIGNED = DXGI_FORMAT_BC5_SNORM,			// RG, signed
			BC6_UNSIGNED = DXGI_FORMAT_BC6H_UF16,		// RGB HDR, unsigned
			BC6_SIGNED = DXGI_FORMAT_BC6H_SF16,			// RGB HDR, signed
			BC7_LINEAR = DXGI_FORMAT_BC7_UNORM,			// RGBA Advanced
			BC7_SRGB = DXGI_FORMAT_BC7_UNORM_SRGB,		// RGBA Advanced, SRGB
		};

		class ReflectionCapture {
		public:
			static constexpr uint32_t DI_TEXTURE_DIMENSION = 8;
			static constexpr uint32_t PFS_TEXTURE_DIMENSION = 512;
			static constexpr uint32_t PFS_TEXTURE_MIPS = 6;
			static constexpr uint32_t BRDFI_TEXTURE_DIMENSION = 512;
			static constexpr uint32_t FIBONACCI_SAMPLE_COUNT = 1024;
			static constexpr float PI_CONST = 3.14159f;

			const std::wstring DI_AND_PFS_VS_NAME = L"VSDIPFS.cso";
			const std::wstring DI_PS_NAME = L"PSDiffuseIrradiance.cso";
			const std::wstring PFS_PS_NAME = L"PSPreFilteredSpecular.cso";
			const std::wstring BRDF_VS_NAME = L"VSBRDFIntegration.cso";
			const std::wstring BRDF_PS_NAME = L"PSBRDFIntegration.cso";
		public:
			inline static std::string DI_TEXTURE_SUFFIX = "DI.dds";
			inline static std::string PFS_TEXTURE_SUFFIX = "PFS.dds";
			inline static std::string BRDFI_TEXTURE_NAME = "2DLUT.dds";
		public:
			//! Initialize everything from the reflection capture
			void init(const std::vector<std::string>& skyTexturePaths);
			//! Check if the integral over hemisphere is correct - should print PI
			void checkHemisphereIntegral();
			//! Generate and save to .dds the diffuse irradiance cubemap
			void generateDiffuseIrradianceCubemap();
			//! Generate and save the pre filtered cubemap
			void generatePreFilteredSpecularCubemap();
			//! Generate and save the BRDF integration texture
			void generateBRDFIntegrationTexture();
		private:
			//! Initialize the inside out cube buffers
			void initCubeBuffers();
			//! Init all the pipelines
			void initPipelines();
			//! Generate the dot product between the view vector and the fibonacci hemisphere
			void fibonacciHemisphere(float& NdotV, float i, float N);

			//! Initialize texture objects and RTVs properly here
			void initDiffuseIrradianceCubeMap();
			void initPreFilteredSpecularCubeMap();
			void initBRDFIntegrationTexture();

			//! Init and bind a square viewport
			void initAndBindViewPort(uint32_t dimension);
			//! Render the inside out cube
			void renderCube();

			void compressAndSave(const DirectX::ScratchImage& image, const std::wstring& filename, FileFormat format);
		private:
			Pipeline m_diffuseIrradiancePipeline;
			Pipeline m_preFilteredSpecularPipeline;
			Pipeline m_BRDFIntegrationPipeline;

			VertexBuffer<VertexPos> m_cubeVertexBuffer;
			IndexBuffer m_cubeIndexBuffer;

			ConstantBuffer<CB_VS_WorldToClip> m_worldToClipBuffer;
			ConstantBuffer<CB_PS_RoughnessBuffer> m_roughnessBuffer;

			std::array<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>, 6> m_currentRTVs;

			Microsoft::WRL::ComPtr<ID3D11Texture2D> m_diffuseIrradianceCubemap;
			D3D11_RENDER_TARGET_VIEW_DESC m_RTVDIDesc;

			Microsoft::WRL::ComPtr<ID3D11Texture2D> m_preFilteredSpecularCubemap;
			D3D11_RENDER_TARGET_VIEW_DESC m_RTVPFSDesc;

			Microsoft::WRL::ComPtr<ID3D11Texture2D> m_BRDFIntegrationTexture;
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_BRDFIntegrationRTV;

			//! All the map paths to be converted
			std::vector<std::string> m_textureMapPaths;

			const std::array<XMMATRIX, 6> CAMERA_CAPTURE_VIEWS{
				XMMatrixLookAtLH({0.0f, 0.0f, 0.0f}, {1.0f,  0.0f,  0.0f}, {0.0f, 1.0f, 0.0f}),
				XMMatrixLookAtLH({0.0f, 0.0f, 0.0f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f, 0.0f}),
				XMMatrixLookAtLH({0.0f, 0.0f, 0.0f}, {0.0f,  1.0f,  0.0f}, {0.0f, 0.0f, -1.0f}),
				// TODO: Possible mistake, needs texting, UP VECTOR
				XMMatrixLookAtLH({0.0f, 0.0f, 0.0f}, {0.0f,  -1.0f,  0.0f}, {1.0f, 0.0f, 0.0f}),
				XMMatrixLookAtLH({0.0f, 0.0f, 0.0f}, {0.0f,  0.0f,  1.0f}, {0.0f, 1.0f, 0.0f}),
				XMMatrixLookAtLH({0.0f, 0.0f, 0.0f}, {0.0f,  0.0f,  -1.0f}, {0.0f, 1.0f, 0.0f})
			};

			const XMMATRIX PROJECTION = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.0f), 1.0f, 0.1f, 1000.0f);
		};
	} // rend
} // engn