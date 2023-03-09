#pragma once

#include "render/D3D/d3d.hpp"


namespace engn {
	namespace rend {
		/// <summary>
		/// A wrapper for the LDR RTV, this is usually the backbuffer, so you have to manualy get the texture from the swapchain
		/// </summary>
		class LDRRenderTarget {
		public:
			//! Initialize the texture, RTV
			void init();

			//! Set the Output Merger RTV to the current one with the specified depth stensil view
			void OMSetCurrent(ID3D11DepthStencilView* depthStensilView);

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
		};
	} // rend
} // engn