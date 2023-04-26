#pragma once

#pragma once

#include "render/D3D/d3d.hpp"

namespace engn {
	namespace rend {

		class BindableDepthBuffer {
		public:
			//! Initialize the texture with SRV, DSV
			void init(int screenWidth, int screenHeight, DXGI_FORMAT textureFormat, DXGI_FORMAT DSVFromat, DXGI_FORMAT SRVFormat);

			//! Bind the SRV as texture to the current shader with the specified slot
			void bindSRV(int slot) const;

			//! Clear the DSV with the specified color
			void clear(const FLOAT* color);

			//! Release all the resourses, called at window resize
			void releaseAll();

			[[nodiscard]] ID3D11Texture2D* getTexturePtr();
			[[nodiscard]] ID3D11Texture2D** getTexturePtrAddress();
			[[nodiscard]] ID3D11DepthStencilView* getDSVPtr();
			[[nodiscard]] ID3D11DepthStencilView** getDSVPtrAddress();
		private:
			Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
		};
	} // rend
} // engn