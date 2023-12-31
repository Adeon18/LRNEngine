#pragma once

#include "render/D3D/d3d.hpp"

namespace engn {
	namespace rend {
		/// <summary>
		/// A render target that can be binded, can either be created from the groud up, or partially by setting the 
		/// texture externally
		/// </summary>
		class BindableRenderTarget {
		public:
			//! Initialize the texture => RTV -> SRV from the ground up, using screen width and height(typical for HDR buffers)
			void init(int screenWidth, int screenHeight, DXGI_FORMAT textureFormat);

			//! Initialize the RTV, when we already have the texture filled with data externally(typical for Final LDR RTV)
			//! WARNING: Does not initialize SRV!
			void init();

			//! Init the texture => RTV => SRV from a provided textureDesc
			void init(D3D11_TEXTURE2D_DESC pDesc);

			//! Set the Output Merger RTV to the current one with the specified depth stensil view
			void OMSetCurrent(ID3D11DepthStencilView* depthStensilView);

			//! Bind the SRV as texture to the current shader with the specified slot
			void bindSRV(int slot) const;

			//! Clear the RTV with the specified color
			void clear(const FLOAT* color);

			//! Release all the resourses, called at window resize
			void releaseAll();

			[[nodiscard]] ID3D11Texture2D* getTexturePtr();
			[[nodiscard]] ID3D11Texture2D** getTexturePtrAddress();
			[[nodiscard]] ID3D11RenderTargetView* getRTVPtr();
			[[nodiscard]] ID3D11RenderTargetView** getRTVPtrAddress();
			[[nodiscard]] ID3D11ShaderResourceView** getSRVPtrAddress();
			void getTextureDesc(D3D11_TEXTURE2D_DESC* descToFill);
		private:
			Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
		};
	} // rend
} // engn