#pragma once

#include "render/D3D/d3d.hpp"

#include "utils/TextureManager/TextureManager.hpp"
#include "render/Systems/Pipeline.hpp"
#include "render/Graphics/EngineCamera.hpp"
#include "render/Graphics/DXBuffers/ConstantBuffer.hpp"

namespace engn {
	namespace rend {
		class SkyTriangle {
		public:
			//! Initialize the SkyTriangle buffers, pipeline and texture, takes a path to a TEXTURECUBE
			void init(const std::string& texturePath);
			//! Should be called at the end of all the rendering in a frame
			void render(std::unique_ptr<EngineCamera>& camPtr);
		private:
			Pipeline m_skyPipeline;
			std::shared_ptr<tex::Texture> m_skyBoxTexture;
			ConstantBuffer<CB_VS_SkyFullscreen> m_skyBuffer;

			bool m_initialized = false;
		};
	} // rend
} // engn