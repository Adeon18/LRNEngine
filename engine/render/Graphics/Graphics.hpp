#pragma once

#include "VertexShader.hpp"
#include "PixelShader.hpp"

namespace engn {
	namespace rend {
		/// <summary>
		/// This class facilitates everything needed for the rendering pipeline
		/// </summary>
		class Graphics {
		public:
			void init();
		private:
			void m_initShaders();
			
			VertexShader m_vertexShader;
			PixelShader m_pixelShader;
		};
	} // render
} // engn