#include "SkyBox.hpp"

#include "include/utility/utility.hpp"

namespace engn {
	namespace rend {
		void SkyBox::init(const std::string& texturePath)
		{
			generateCube();
			generatePipeline();
			m_skyBoxTextureCube = tex::TextureManager::getInstance().getTexture(texturePath);
		}
		void SkyBox::render()
		{
			bindPipeline(m_pipeline);
			m_vertexBuffer.bind();
			m_indexBuffer.bind();

			d3d::s_devcon->PSSetShaderResources(0, 1, m_skyBoxTextureCube->textureView.GetAddressOf());

			d3d::s_devcon->DrawIndexed(36, 0, 0);
		}
		void SkyBox::generateCube()
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

			m_vertexBuffer.init(vertices);
			m_indexBuffer.init(indices);
		}
		void SkyBox::generatePipeline()
		{
			D3D11_INPUT_ELEMENT_DESC layout[] = { "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 };
			std::wstring shaderFolder = util::getExeDirW();

			PipelineData pipelineData{
				layout,
				ARRAYSIZE(layout),
				D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				shaderFolder + L"VSSky.cso",
				L"",
				L"",
				L"",
				shaderFolder + L"PSSky.cso"
			};

			initPipeline(m_pipeline, pipelineData);
		}
	} // rend
} // engn