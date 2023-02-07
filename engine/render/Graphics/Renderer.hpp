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
		class Renderer {
		public:
			void init();
			void renderFrame(std::unique_ptr<EngineCamera>& camPtr, const RenderData& renderData);
		private:
			void m_initScene();
			//! Initialize the RasterizerState
			/// RasterizerState basically tells us HOW to draw triangles
			///	Cull back, draw only Wireframe, etc.
			void m_initRasterizer();
			//! Depth stensil state
			void m_initDepthStencilState();
			//! Fill the per frame shader CB, for not the same for each shader
			void m_fillPerFrameCBs(std::unique_ptr<EngineCamera>& camPtr, const RenderData& renderData);
			
			ConstantBuffer<CB_RealTimeData> m_constantBufferVS;

			Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStensilState;
			Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;
		};
	} // render
} // engn