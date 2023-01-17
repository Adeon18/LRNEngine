#pragma once

#include "VertexShader.hpp"
#include "PixelShader.hpp"

#include "Vertex.hpp"

namespace engn {
	namespace rend {
		/// <summary>
		/// This class facilitates everything needed for the rendering pipeline
		/// </summary>
		class Graphics {
		public:
			void init();
			void renderFrame();
		private:
			void m_initShaders();
			void m_initScene();
			
			VertexShader m_vertexShader;
			PixelShader m_pixelShader;

			DxResPtr<ID3D11Buffer> m_vertexBuffer;
		};
	} // render
} // engn