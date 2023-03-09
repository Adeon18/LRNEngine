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
			void bindSRV(int slot);

			//! Release all the resourses, called at window resize
			void releaseAll();
		private:
			Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
		};
	} // rend
} // engn