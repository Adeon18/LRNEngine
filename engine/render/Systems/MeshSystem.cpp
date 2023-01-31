#include "MeshSystem.hpp"

namespace engn {
	namespace rend {

		void MeshSystem::render(const XMMATRIX& worldToClip)
		{
			m_normalGroup.fillInstanceBuffer(worldToClip);
			m_normalGroup.render();
		}

		void MeshSystem::addNormalInstance(std::shared_ptr<mdl::Model> mod)
		{
			m_normalGroup.addModel(mod);
		}

		void NormalGroup::init()
		{
			D3D11_INPUT_ELEMENT_DESC layout[] = {
				{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"M2CLIP", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"M2CLIP", 1, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"M2CLIP", 2, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"M2CLIP", 3, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
			};
			std::wstring shaderFolder = util::getExeDirW();
			Logger::instance().logInfo(L"Shader Folder found: " + shaderFolder);

			m_vertexShader.init(shaderFolder + L"VSMoveByOffset.cso", layout, ARRAYSIZE(layout));
			m_pixelShader.init(shaderFolder + L"PSVoronoi.cso");
		}

		void NormalGroup::addModel(std::shared_ptr<mdl::Model> mod)
		{
			PerModel newModel;
			newModel.model = std::shared_ptr < mdl::Model > (mod);

			PerMesh perMesh;

			PerMaterial perMat;
			perMat.material = {};
			for (auto& mesh : newModel.model->getMeshes()) {
				Instance i;
				i.modelToWorld = mesh.instances[0];
				i.color = { 1.0f, 0.0f, 0.0f, 1.0f };
				perMat.instances.push_back(i);
			}

			perMesh.push_back(perMat);

			newModel.perMesh.push_back(perMesh);

			m_models.push_back(newModel);
		}

		void NormalGroup::fillInstanceBuffer(const XMMATRIX& worldToView)
		{
			// Count total instances
			uint32_t totalInstances = 0;
			for (auto& model : m_models)
				for (auto& mesh : model.perMesh)
					for (const auto& material : mesh)
						totalInstances += uint32_t(material.instances.size());


			if (totalInstances == 0)
				//std::cout << "AAA" << std::endl;
				return;

			// Initialize instanceBuffer
			m_instanceBuffer.init(totalInstances); // resizes if needed

			// Map buffer data
			if (!m_instanceBuffer.map()) {
				return;
			}
			Instance* dst = static_cast<Instance*>(m_instanceBuffer.getMappedBuffer().pData);

			// Fill mapped buffer
			uint32_t copiedNum = 0;
			for (const auto& model : m_models)
			{
				for (uint32_t meshIndex = 0; meshIndex < model.perMesh.size(); ++meshIndex)
				{
					const mdl::Mesh& mesh = model.model->getMeshes()[meshIndex];

					for (const auto& material : model.perMesh[meshIndex])
					{
						auto& instances = material.instances;

						uint32_t numModelInstances = instances.size();
						for (uint32_t index = 0; index < numModelInstances; ++index)
						{
							Instance ins;
							ins.modelToWorld = material.instances[index].modelToWorld * worldToView;
							ins.color = material.instances[index].color;
							dst[copiedNum++] = ins;
						}
					}
				}
			}

			m_instanceBuffer.unmap();
		}

		void NormalGroup::render()
		{
			if (m_instanceBuffer.getSize() == 0)
				//std::cout << "DDDD" << std::endl;
				return;

			m_vertexShader.bind();
			m_pixelShader.bind();
			m_instanceBuffer.bind();

			uint32_t renderedInstances = 0;
			for (const auto& model : m_models)
			{
				//if (model.empty) continue;

				model.model->getVertices().bind();
				model.model->getIndices().bind();

				for (uint32_t meshIndex = 0; meshIndex < model.perMesh.size(); ++meshIndex)
				{
					const mdl::Mesh& mesh = model.model->getMeshes()[meshIndex];
					const auto& meshRange = model.model->getRanges()[meshIndex];

					// You have to upload a Mesh-to-Model transformation matrix retrieved from model file via Assimp
					// meshData.update(mesh.meshToModel); // ... update shader local per-mesh uniform buffer

					for (const auto& perMaterial : model.perMesh[meshIndex])
					{
						if (perMaterial.instances.empty()) continue;

						const auto& material = perMaterial.material;

						// ... update shader local per-draw uniform buffer
						// materialData.update(...); // we don't have it in HW4

						// ... bind each material texture, we don't have it in HW4

						uint32_t numInstances = uint32_t(perMaterial.instances.size());
						d3d::s_devcon->DrawIndexedInstanced(meshRange.indexNum, numInstances, meshRange.indexOffset, meshRange.vertexOffset, renderedInstances);
						renderedInstances += numInstances;
					}
				}
			}

		}
	} // rend
} // engn