#pragma once

#include "VertexBuffer.hpp"

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
			//! Initialize the RasterizerState
			/// RasterizerState basically tells us HOW to draw triangles
			///	Cull back, draw only Wireframe, etc.
			void m_initRasterizer();
			
			VertexShader m_vertexShader;
			PixelShader m_pixelShader;

			VertexBuffer<Vertex> m_vertexBuffer;
			DxResPtr<ID3D11RasterizerState> m_rasterizerState;
		};
	} // render
} // engn