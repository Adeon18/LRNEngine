#pragma once

#include "DirectXTex.h"

#include "ReflectionCapture.hpp"

#include "utils/Logger/Logger.hpp"

namespace engn {
	namespace rend {
		void ReflectionCapture::init(const std::string& skyTexturePath)
		{
			initCubeBuffers();
			initPipelines();

			m_worldToClipBuffer.init();
			m_skyBoxTexture = tex::TextureManager::getInstance().getTexture(skyTexturePath);

			Logger::instance().logDebug("HOLY FUCK IT WORKS");
		}
		void ReflectionCapture::initCubeBuffers()
		{
			std::vector vertices =
			{
				VertexPos{{1.0f, 1.0f, -1.0f}}, // top-right-front
				VertexPos{{-1.0f, 1.0f, -1.0f}}, // top-left-front
				VertexPos{{1.0f, -1.0f, -1.0f}}, // bottom-right-front
				VertexPos{{-1.0f, -1.0f, -1.0f}}, // bottom-left-front

				VertexPos{{1.0f, 1.0f, 1.0f}}, // top-right-back
				VertexPos{{-1.0f, 1.0f, 1.0f}}, // top-left-back
				VertexPos{{1.0f, -1.0f, 1.0f}}, // bottom-right-back
				VertexPos{{-1.0f, -1.0f, 1.0f}}, // bottom-left-back
			};
			std::vector<DWORD> indices =
			{
				2, 0, 1, // front right
				3, 2, 1, // front left

				4, 0, 2, // right left
				6, 4, 2, // right right

				1, 5, 3, // left right
				5, 7, 3, // left left

				7, 5, 6, // back right
				5, 4, 6, // back left

				4, 5, 1, // top right
				0, 4, 1, // top left

				6, 2, 3, // back right
				7, 6, 3, // back left
			};

			m_cubeVertexBuffer.init(vertices);
			m_cubeIndexBuffer.init(indices);
		}
		void ReflectionCapture::initPipelines()
		{
			//! Input layout
			D3D11_INPUT_ELEMENT_DESC layout[] = { "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };

			//! Depth stencil state
			D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc{};
			depthStencilStateDesc.DepthEnable = true;
			depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER_EQUAL;
			//! Rasterizer state
			D3D11_RASTERIZER_DESC rasterizerDesc{};
			rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
			rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

			// Init Pipelines
			const std::wstring exeDirW = util::getExeDirW();

			// Diffuse Irradiance
			PipelineData diffIrradiancePipelineData{
				layout,
				ARRAYSIZE(layout),
				D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				exeDirW + DI_AND_PFS_VS_NAME,
				L"", L"", L"",
				exeDirW + DI_PS_NAME,
				rasterizerDesc,
				depthStencilStateDesc
			};

			initPipeline(m_diffuseIrradiancePipeline, diffIrradiancePipelineData);

			// Pre filtered Specular
			PipelineData preFiltSpecPipelineData{
				layout,
				ARRAYSIZE(layout),
				D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				exeDirW + DI_AND_PFS_VS_NAME,
				L"", L"", L"",
				exeDirW + PFS_PS_NAME,
				rasterizerDesc,
				depthStencilStateDesc
			};

			initPipeline(m_preFilteredSpecularPipeline, preFiltSpecPipelineData);

			// BRDF Integration Specular
			PipelineData integrBRDFPipelineData{
				nullptr,
				0,
				D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				exeDirW + BRDF_VS_NAME,
				L"", L"", L"",
				exeDirW + BRDF_PS_NAME,
				rasterizerDesc,
				depthStencilStateDesc
			};

			initPipeline(m_BRDFIntegrationPipeline, integrBRDFPipelineData);

		}
		void ReflectionCapture::renderCube()
		{
			m_cubeVertexBuffer.bind();
			m_cubeIndexBuffer.bind();

			d3d::s_devcon->DrawIndexed(36, 0, 0);
		}
		void ReflectionCapture::generateDiffuseIrradianceCubemap(const XMMATRIX& projection)
		{



			bindPipeline(m_diffuseIrradiancePipeline);

			m_worldToClipBuffer.getData().worldToClip = projection;
			m_worldToClipBuffer.fill();

			d3d::s_devcon->VSSetConstantBuffers(0, 1, m_worldToClipBuffer.getBufferAddress());
			d3d::s_devcon->PSSetShaderResources(0, 1, m_skyBoxTexture->textureView.GetAddressOf());

			renderCube();


		}
		void ReflectionCapture::generatePreFilteredSpecularCubemap(const XMMATRIX& projection)
		{
		}
		void ReflectionCapture::generateBRDFIntegrationTexture()
		{
		}
	} // rend
} // engn