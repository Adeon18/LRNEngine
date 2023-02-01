#pragma once

#include "RenderStructs.hpp"

#include "utils/ModelManager/ModelManager.hpp"
#include "render/Instances/Model.hpp"

#include "render/Graphics/VertexBuffer.hpp"
#include "render/Graphics/ConstantBuffer.hpp"
#include "render/Graphics/InstanceBuffer.hpp"

#include "render/Graphics/VertexShader.hpp"
#include "render/Graphics/PixelShader.hpp"

#include "render/Graphics/Vertex.hpp"

namespace engn {
	namespace rend {
		template<typename I, typename M>
		class RenderGroup {
		public:
			struct PerMaterial {
				M material;
				std::vector<I> instances;
			};

			using PerMesh = std::vector<PerMaterial>;

			struct PerModel {
				std::shared_ptr<mdl::Model> model;
				std::vector<PerMesh> perMesh;
			};

			std::vector<PerModel> m_models;
			InstanceBuffer<I> m_instanceBuffer;
			ConstantBuffer<CB_VS_MeshData> m_meshData;

			VertexShader m_vertexShader;
			PixelShader m_pixelShader;
		public:
			void init(const std::wstring& VSpath, const std::wstring& PSpath) {
				D3D11_INPUT_ELEMENT_DESC layout[] = {
					{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
					{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
					{"TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
					{"BITANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
					{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 44, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
					{"M2CLIP", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
					{"M2CLIP", 1, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
					{"M2CLIP", 2, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
					{"M2CLIP", 3, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
					{"COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				};

				m_vertexShader.init(VSpath, layout, ARRAYSIZE(layout));
				m_pixelShader.init(PSpath);
				m_meshData.init();
			}

			void addModel(std::shared_ptr<mdl::Model> mod, const M& mtrl, const I& inc) {
				// In all the meshes, looks for the first matching material
				bool isCached = false;
				for (auto& perModel : m_models) {
					if (perModel.model->name == mod->name) {
						for (auto& perMesh : perModel.perMesh) {
							for (auto& perMaterial : perMesh) {
								if (perMaterial.material == mtrl) {
									perMaterial.instances.push_back(inc);
									if (!isCached) { isCached = true; }
								}
							}
						}
						if (isCached) { return; }
					}
				}

				Logger::instance().logInfo("Model " + mod->name + " created for the first time, creating PerModel struct...");

				PerModel newModel;
				newModel.model = std::shared_ptr<mdl::Model>(mod);

				for (auto& mesh : newModel.model->getMeshes()) {
					PerMesh perMesh;

					PerMaterial perMat;
					perMat.material = mtrl;
					perMat.instances.push_back(inc);

					perMesh.push_back(perMat);
					newModel.perMesh.push_back(perMesh);
				}

				m_models.push_back(newModel);
			}

			//! Fill the data to be passed by instance
			void fillInstanceBuffer(const XMMATRIX& worldToView) {
				// Count total instances
				uint32_t totalInstances = 0;
				for (auto& model : m_models)
					for (auto& mesh : model.perMesh)
						for (const auto& material : mesh)
							totalInstances += uint32_t(material.instances.size());


				if (totalInstances == 0)
					return;

				// Initialize instanceBuffer
				m_instanceBuffer.init(totalInstances); // resizes if needed

				// Map buffer data
				if (!m_instanceBuffer.map()) {
					return;
				}
				I* dst = static_cast<Instance*>(m_instanceBuffer.getMappedBuffer().pData);

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
								// Dangerous!
								I ins;
								ins.modelToWorld = material.instances[index].modelToWorld * worldToView;
								ins.color = material.instances[index].color;
								dst[copiedNum++] = ins;
							}
						}
					}
				}

				m_instanceBuffer.unmap();
			}

			void render() {
				if (m_instanceBuffer.getSize() == 0)
					return;

				d3d::s_devcon->IASetInputLayout(m_vertexShader.getInputLayout());
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
						m_meshData.getData().modelToWorld = XMMatrixTranspose(mesh.meshToModel); // ... update shader local per-mesh uniform buffer
						m_meshData.getData().modelToWorldInv = XMMatrixTranspose(mesh.meshToModelInv);
						m_meshData.fill();
						d3d::s_devcon->VSSetConstantBuffers(1, 1, m_meshData.getBufferAddress());

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
		};

		class MeshSystem {
		public:
			static MeshSystem& getInstance() {
				static MeshSystem system;
				return system;
			}
			MeshSystem(const MeshSystem& other) = delete;
			MeshSystem& operator=(const MeshSystem& other) = delete;

			void init() {
				initNormalGroup();
				initHologramGroup();
			}

			void initNormalGroup();
			void initHologramGroup();

			void render(const XMMATRIX& worldToClip);
			
			void addNormalInstance(std::shared_ptr<mdl::Model> mod, const Material& mtrl, const Instance& inc);
			void addHologramInstance(std::shared_ptr<mdl::Model> mod, const Material& mtrl, const Instance& inc);
		private:
			MeshSystem() {};
			RenderGroup<Instance, Material> m_normalGroup;
			RenderGroup<Instance, Material> m_hologramGroup;
		};
	} // rend
} // engn