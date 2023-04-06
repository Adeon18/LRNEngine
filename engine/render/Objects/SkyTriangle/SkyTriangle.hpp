#pragma once

#include "render/D3D/d3d.hpp"

#include "utils/TextureManager/TextureManager.hpp"
#include "render/Systems/Pipeline.hpp"
#include "render/Graphics/EngineCamera.hpp"
#include "render/Graphics/DXBuffers/ConstantBuffer.hpp"

namespace engn {
	namespace rend {
		/// <summary>
		/// Sky rendering with the help of a cubemap and a fullscreen triangle
		/// </summary>
		class SkyTriangle {
		public:
			//! Initialize the SkyTriangle buffers, pipeline and texture, takes a path to a TEXTURECUBE
			void init(const std::string& texturePath);
			//! Should be called at the end of all the rendering in a frame
			void render(std::unique_ptr<EngineCamera>& camPtr);
		private:
			const std::wstring VS_NAME = L"VSSkyFullscreen.cso";
			const std::wstring PS_NAME = L"PSSkyFullscreen.cso";

			Pipeline m_skyPipeline;
			std::shared_ptr<tex::Texture> m_skyBoxTexture;
			ConstantBuffer<CB_VS_SkyFullscreen> m_skyBuffer;

			Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_skyDepthStensilState;

			bool m_initialized = false;
		};
	} // rend
} // engn