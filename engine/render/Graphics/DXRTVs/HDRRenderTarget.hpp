#pragma once

#include "render/D3D/d3d.hpp"

namespace engn {
	namespace rend {
		/// <summary>
		/// The HDR render target wrapper, it wraps the texture and RTV and SRV needed to render to 
		/// a texture that can store FLOOAT16 RGBA.
		/// </summary>
		class HDRRenderTarget {
		public:
			//! Initialize the texture, RTV and SRV and more, should be called at the start or after releaseAll only
			void init(int screenWidth, int screenHeight);

			//! Set the Output Merger RTV to the current one with the specified depth stensil view
			void OMSetCurrent(ID3D11DepthStencilView *depthStensilView);

			//! Bind the SRV as texture to the current shader with the specified slot
			void bindSRV(int slot) const;

			//! Clear the RTV with the specified color
			void clear(const FLOAT* color);

			//! Release all the resourses, called at window resize
			void releaseAll();

			[[nodiscard]] ID3D11Texture2D* getTexturePtr() { return m_texture.Get(); }
			[[nodiscard]] ID3D11Texture2D** getTexturePtrAddress() { return m_texture.GetAddressOf(); }
			[[nodiscard]] ID3D11RenderTargetView* getRTVPtr() { return m_renderTargetView.Get(); }
			[[nodiscard]] ID3D11RenderTargetView** getRTVPtrAddress() { return m_renderTargetView.GetAddressOf(); }
		private:
			Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
		};
	} // rend
} // engn