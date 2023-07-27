#pragma once

#include "render/Graphics/Vertex.hpp"
#include "render/Systems/Pipeline.hpp"

#include "render/Graphics/DXBuffers/IndexBuffer.hpp"
#include "render/Graphics/DXBuffers/VertexBuffer.hpp"
#include "render/Graphics/DXBuffers/ConstantBuffer.hpp"

#include "render/Graphics/DXRTVs/BindableRenderTarget.hpp"

namespace engn {
	namespace rend {
		/// <summary>
		/// A class for ressolving physically based bloom, by downsampling and upsampling the HDR ressolved texture.
		/// </summary>
		class BloomRessolver {
		public:
			//! Init everything
			void init();
			//! DownSample and blue the HDR texture that is given to it, the blured texture is saved
			void downSampleAndBlur(const BindableRenderTarget& src);
			//! MUST BE CALLED AFTER downSampleAndBlur, upsamples the downSampled texture and basically stores the final result of the blur
			void upSampleAndBlur();

			void bindBloomTextureToSlot(uint32_t slot);

			const std::wstring VS_BLOOM = L"VSBloom.cso";
			const std::wstring PS_UPSAMPLE = L"PSBloomUP.cso";
			const std::wstring PS_DOWNSAMPLE = L"PSBloomDOWN.cso";

			static constexpr uint16_t SAMPLE_COUNT = 4;
		private:
			void initAndBindViewPort(uint32_t dimensionX, uint32_t dimensionY);

			void initPipelines();
			void initTextures();
			void initBuffers();

			//! This texture has mips that are progressively downsampled and upsampled from during blur ressolve
			Microsoft::WRL::ComPtr<ID3D11Texture2D> m_bluredTextureStorage;
			std::array<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>, SAMPLE_COUNT> m_blurredTextureMipRTVs;
			std::array<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, SAMPLE_COUNT> m_blurredTextureMipSRVs;

			ConstantBuffer<CB_PS_BloomSampleData> m_bloomCBPS;

			Pipeline m_downsamplePipeline;
			Pipeline m_upsamplePipeline;
		};
	} // rend
} // engn