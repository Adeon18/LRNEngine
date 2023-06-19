#include "DecalSystem.hpp"

namespace engn {
	namespace rend {
		void DecalSystem::init()
		{
			initPipelines();
			initBuffers();
			initTextures();
			initModels();
		}
		void DecalSystem::addDecal(const InstanceProperties& insProps, const geom::MeshIntersection& insHit, std::unique_ptr<EngineCamera>& camPtr)
		{
			uint32_t matIdx = MeshSystem::getInstance().getGroupMatrixIdx(insProps);
			XMMATRIX& modelToWorld = TransformSystem::getInstance().getMatrixById(matIdx);
			XMMATRIX worldToModel = XMMatrixInverse(nullptr, modelToWorld);

			XMVECTOR offset = XMVector3Transform(insHit.pos, worldToModel);
			//std::cout << "Offset: " << offset << std::endl;

			static constexpr XMVECTOR UP{ 0.0f, 1.0f, 0.0f };
			XMVECTOR zWorld = XMVector3Normalize(camPtr->getCamPosition() - insHit.pos);
			XMVECTOR xWorld = XMVector3Normalize(XMVector3Cross(UP, zWorld));
			XMVECTOR yWorld = XMVector3Normalize(XMVector3Cross(zWorld, xWorld));
			zWorld = XMVectorSetW(zWorld, 0);

			//std::cout << "WorldBasis Z: " << zWorld << std::endl;

			XMMATRIX worldDecalBasis;
			worldDecalBasis.r[0] = xWorld;
			worldDecalBasis.r[1] = yWorld;
			worldDecalBasis.r[2] = zWorld;
			worldDecalBasis.r[3] = XMVECTOR{0, 0, 0, 1};

			auto& decalData = m_decals.emplace_back();
			decalData.decalToModel = worldDecalBasis * worldToModel;
			for (uint16_t i = 0; i < 3; ++i) {
				decalData.decalToModel.r[i] = XMVector3Normalize(decalData.decalToModel.r[i]);
			}
			decalData.decalToModel.r[3] = offset;
			decalData.modelToDecal = XMMatrixInverse(nullptr, decalData.decalToModel);
			decalData.modelInstanceID = matIdx;

			//std::cout << "Decal To Model: \n" << decalData.decalToModel << std::endl;
		}
		void DecalSystem::handleDecals()
		{
			bindPipeline(m_pipeline);
			fillInstanceBuffers();
			renderInternal();
		}
		void DecalSystem::initBuffers()
		{
		}
		void DecalSystem::initPipelines()
		{
			D3D11_INPUT_ELEMENT_DESC DEFAULT_LAYOUT_DECALS[13] = {
				{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"BITANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"DECAL2WORLD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"DECAL2WORLD", 1, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"DECAL2WORLD", 2, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"DECAL2WORLD", 3, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"DECAL2WORLDINV", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"DECAL2WORLDINV", 1, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 80, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"DECAL2WORLDINV", 2, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 96, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"DECAL2WORLDINV", 3, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 112, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
			};

			auto shaderFolder = util::getExeDirW();

			D3D11_RASTERIZER_DESC rasterizerDesc{};
			rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
			rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;

			D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
			depthStencilDesc.DepthEnable = false;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;

			D3D11_RENDER_TARGET_BLEND_DESC blendDesc{};
			blendDesc.BlendEnable = false;
			blendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

			PipelineData pipelineData{
				DEFAULT_LAYOUT_DECALS,
				ARRAYSIZE(DEFAULT_LAYOUT_DECALS),
				D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				shaderFolder + L"VSDecals.cso",
				L"",
				L"",
				L"",
				shaderFolder + L"PSDecals.cso",
				rasterizerDesc,
				depthStencilDesc,
				blendDesc
			};

			initPipeline(m_pipeline, pipelineData);
		}
		void DecalSystem::initTextures()
		{
		}
		void DecalSystem::initModels()
		{
#ifdef _WIN64
			const std::string TEX_REL_PATH_PREF = util::getExeDir() + "..\\..\\";
#else
			const std::string TEX_REL_PATH_PREF = util::getExeDir() + "..\\";
#endif
			m_cubeModel = mdl::ModelManager::getInstance().getModel(TEX_REL_PATH_PREF + "assets/Models/Cube/Cube.fbx");

		}
		void DecalSystem::fillInstanceBuffers()
		{
			// Count total instances
			uint32_t totalInstances = m_decals.size();


			if (totalInstances == 0)
				return;

			// Initialize instanceBuffer
			m_instanceBuffer.init(totalInstances); // resizes if needed

			// Map buffer data
			if (!m_instanceBuffer.map()) {
				return;
			}
			DecalInstance* dst = static_cast<DecalInstance*>(m_instanceBuffer.getMappedBuffer().pData);

			// Fill mapped buffer
			uint32_t copiedNum = 0;
			for (const auto& decal : m_decals)
			{
				// Dangerous! TODO SFINAE
				DecalInstance ins;
				ins.decalToWorld = decal.decalToModel * TransformSystem::getInstance().getMatrixById(decal.modelInstanceID);
				// TODO: UNOPTIMIZED.
				ins.worldToDecal = XMMatrixInverse(nullptr, ins.decalToWorld);

				dst[copiedNum++] = ins;
			}

			m_instanceBuffer.unmap();
		}
		void DecalSystem::renderInternal()
		{
			// TODO: DON'T BIND SJADERS IF EMPTY
			if (m_instanceBuffer.getSize() == 0)
				return;

			m_instanceBuffer.bind();
			m_cubeModel->getVertices().bind();
			m_cubeModel->getIndices().bind();

			uint32_t renderedInstances = 0;
			uint32_t numInstances = m_decals.size();
			const auto& meshRange = m_cubeModel->getRanges()[0];
			d3d::s_devcon->DrawIndexedInstanced(meshRange.indexNum, numInstances, 0, 0, renderedInstances);
			renderedInstances += numInstances;
		}
	} // rend
} // engn