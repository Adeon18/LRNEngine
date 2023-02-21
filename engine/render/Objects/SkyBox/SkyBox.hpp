#pragma once

#include "render/Graphics/Vertex.hpp"
#include "render/Systems/Pipeline.hpp"

#include "render/Graphics/DXBuffers/IndexBuffer.hpp"
#include "render/Graphics/DXBuffers/VertexBuffer.hpp"

#include "utils/TextureManager/TextureManager.hpp"


namespace engn {
	namespace rend {
		class SkyBox {
		public:
			//! Initialize the entire skybox: load textures, create cube, create pipeline
			void init(const std::string& texturePath);
			//! Bind the pipeline, textures, buffers and call draw on the entire skybox
			void render();
		private:
			//! Generate the cube that is only visible from the inside and fill the respective buffers
			void generateCube();
			//! Initialize the pipeline exclusive to Skybox with the respective CONSTANT data
			void generatePipeline();
		private:
			Pipeline m_pipeline;
			VertexBuffer<VertexPos> m_vertexBuffer;
			IndexBuffer m_indexBuffer;
			std::shared_ptr<tex::Texture> m_skyBoxTextureCube;
		};
	} // rend
} // engn