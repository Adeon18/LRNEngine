#pragma once

#include "IndexBuffer.hpp"
#include "VertexBuffer.hpp"
#include "ConstantBuffer.hpp"
#include "InstanceBuffer.hpp"

#include "VertexShader.hpp"
#include "PixelShader.hpp"

#include "Vertex.hpp"

#include "EngineCamera.hpp"

namespace engn {
	namespace rend {
		struct RenderData {
			float iTime;
			float iResolutionX;
			float iResolutionY;
			float invResolutionX;
			float invResolutionY;
		};
		/// <summary>
		/// This class facilitates everything needed for the rendering pipeline
		/// </summary>
		class Graphics {
		public:
			void init();
			void renderFrame(std::unique_ptr<EngineCamera>& camPtr, const RenderData& renderData);
		private:
			void m_initScene();
			//! Initialize the RasterizerState
			/// RasterizerState basically tells us HOW to draw triangles
			///	Cull back, draw only Wireframe, etc.
			void m_initRasterizer();
			void m_initDepthStencilState();
			
			ConstantBuffer<CB_PS_ShaderToy> m_constantBufferPS;

			Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStensilState;
			Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;
		};
	} // render
} // engn