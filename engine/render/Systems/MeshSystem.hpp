#pragma once

#include <unordered_map>

#include "RenderStructs.hpp"

#include "utils/ModelManager/ModelManager.hpp"
#include "utils/TextureManager/TextureManager.hpp"
#include "render/Objects/Model.hpp"

#include "render/Graphics/DXBuffers/VertexBuffer.hpp"
#include "render/Graphics/DXBuffers/ConstantBuffer.hpp"
#include "render/Graphics/DXBuffers/InstanceBuffer.hpp"

#include "render/Systems/Pipeline.hpp"

#include "render/Graphics/Vertex.hpp"
#include "render/Graphics/HelperStructs.hpp"

namespace engn {
	namespace rend {
		enum GroupTypes {
			NORMAL,
			HOLOGRAM
		};

		//! Struct that identifies the instance of the material of the model to be dragged
		struct InstanceProperties {
			GroupTypes group;
			std::shared_ptr<mdl::Model> model;
			uint32_t materialIdx;
			uint32_t instanceIdx;
		};

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

			//! A unique enum identifier that allows to get the group type at dragger collision - Normal by default
			GroupTypes m_type = GroupTypes::NORMAL;
			//! Group input assembler topology - triangleList by default
		public:
			//! Make the groups definable my type
			void setType(const GroupTypes& t) { m_type = t; }


			//! Init the input layout(for now the same for all)
			void init() {
				m_meshData.init();
			}

			// Find the closest instance that intersects with a ray and fill in the infor struct
			bool checkRayIntersection(geom::Ray& ray, mdl::MeshIntersection& nearest, InstanceProperties& i2d) {

				bool hasIntersection = false;
				for (auto& perModel : m_models) {
					uint32_t meshIdx = 0;
					for (auto& perMesh: perModel.perMesh) {
						uint32_t matIdx = 0;
						for (auto& perMaterial: perMesh) {
							uint32_t insIdx = 0;
							for (auto& perInstance: perMaterial.instances) {
								// We find ray intersections in MESH SPACE
								XMMATRIX meshtoWorld =
									perModel.model->getMeshes()[meshIdx].meshToModel *
									perInstance.modelToWorld;
								// Ray to Mesh Space
								ray.transform(XMMatrixInverse(nullptr, meshtoWorld));
								// If there is a collision
								if (perModel.model->getMeshOcTrees()[meshIdx].intersect(ray, nearest)) {

									nearest.pos = XMVector3Transform(nearest.pos, meshtoWorld);
									i2d.group = m_type;
									i2d.model = perModel.model;
									i2d.materialIdx = matIdx;
									i2d.instanceIdx = insIdx;

									if (!hasIntersection) { hasIntersection = true; }
								}
								// Transform ray back to world space
								ray.transform(meshtoWorld);

								++insIdx;
							}
							++matIdx;
						}
						++meshIdx;
					}
				}
				return hasIntersection;
			}

			//! Add offset to module by multiplying it by the transformation matrix
			void addModelOffset(const InstanceProperties& insProps, const XMVECTOR& offset) {
				for (auto& perModel : m_models) {
					if (perModel.model->name == insProps.model->name) {
						for (auto& perMesh : perModel.perMesh) {
							for (uint32_t matIdx = 0; matIdx < perMesh.size(); ++matIdx) {
								if (matIdx == insProps.materialIdx) {
									for (uint32_t insIdx = 0; insIdx < perMesh[matIdx].instances.size(); ++insIdx) {
										if (insIdx == insProps.instanceIdx) {
											perMesh[matIdx].instances[insIdx].modelToWorld *= XMMatrixTranslationFromVector(offset);
										}
									}
								}
							}
						}
					}
				}
			}

			//! Add offset to module by multiplying it by the transformation matrix
			void addModelRotation(const InstanceProperties& insProps, const XMVECTOR& rotation) {
				for (auto& perModel : m_models) {
					if (perModel.model->name == insProps.model->name) {
						for (auto& perMesh : perModel.perMesh) {
							for (uint32_t matIdx = 0; matIdx < perMesh.size(); ++matIdx) {
								if (matIdx == insProps.materialIdx) {
									for (uint32_t insIdx = 0; insIdx < perMesh[matIdx].instances.size(); ++insIdx) {
										if (insIdx == insProps.instanceIdx) {
											perMesh[matIdx].instances[insIdx].modelToWorld *= XMMatrixRotationRollPitchYaw(XMVectorGetX(rotation), XMVectorGetY(rotation), XMVectorGetZ(rotation));
										}
									}
								}
							}
						}
					}
				}
			}

			// Add the model by filling in the respective structs
			void addModel(std::shared_ptr<mdl::Model> mod, const M& mtrl, const I& inc) {
				if (!mod) {
					Logger::instance().logErr("addModel: The model pointer is null");
				}

				//! Struct to check where exectly the data is added to stop at the right time
				struct ModelIsAdded {
					bool addedAsInstance = false;
					bool addedAsMaterial = false;

					bool wasAdded() { return addedAsInstance || addedAsMaterial; }
				};

				ModelIsAdded modelIsAdded;

				bool isCached = false;
				for (auto& perModel : m_models) {
					if (perModel.model->name == mod->name) {
						for (auto& perMesh : perModel.perMesh) {
							for (auto& perMaterial : perMesh) {
								// Push new instance to old material if it is the same
								if (perMaterial.material == mtrl) {
									perMaterial.instances.push_back(inc);
									modelIsAdded.addedAsInstance = true;
									break;
								}
							}
							if (!modelIsAdded.addedAsInstance) {
								PerMaterial perMat;
								perMat.material = mtrl;
								perMat.instances.push_back(inc);
								perMesh.push_back(perMat);
								modelIsAdded.addedAsMaterial = true;
							}
						}
						if (modelIsAdded.wasAdded()) { return; }
					}
				}

				Logger::instance().logInfo("Model " + mod->name + " created for the first time, creating PerModel struct...");

				PerModel newModel;
				newModel.model = std::shared_ptr<mdl::Model>(mod);

				for (auto& mesh : newModel.model->getMeshes()) {
					PerMesh perMesh;

					if (!mtrl.texPtr.get()) {
						// TODO: Has a bug that puts only last texture as acrive in case of multiple textures per mesh
						for (auto& texPath : mesh.texturePaths) {
							PerMaterial perMat;
							perMat.material = { tex::TextureManager::getInstance().getTexture(texPath) };
							perMat.instances.push_back(inc);
							perMesh.push_back(perMat);
						}
					}
					else {
						PerMaterial perMat;
						perMat.material = mtrl;
						perMat.instances.push_back(inc);
						perMesh.push_back(perMat);
					}
					newModel.perMesh.push_back(perMesh);
				}

				m_models.push_back(newModel);
			}

			//! Fill the data to be passed by instance
			void fillInstanceBuffer() {
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
								// Dangerous! TODO SFINAE
								I ins;
								ins.modelToWorld = material.instances[index].modelToWorld;
								ins.color = material.instances[index].color;
								dst[copiedNum++] = ins;
							}
						}
					}
				}

				m_instanceBuffer.unmap();
			}

			// Render all the group meshes, can be called multiple times per frame with different shaders
			void render() {
				// TODO: DON'T BIND SJADERS IF EMPTY
				if (m_instanceBuffer.getSize() == 0)
					return;

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
						m_meshData.getData().meshToModel = XMMatrixTranspose(mesh.meshToModel); // ... update shader local per-mesh uniform buffer
						m_meshData.getData().meshToModelInv = XMMatrixTranspose(mesh.meshToModelInv);
						m_meshData.fill();
						d3d::s_devcon->VSSetConstantBuffers(1, 1, m_meshData.getBufferAddress());

						for (const auto& perMaterial : model.perMesh[meshIndex])
						{
							if (perMaterial.instances.empty()) continue;

							const auto& material = perMaterial.material;

							// ... update shader local per-draw uniform buffer
							// materialData.update(...); // we don't have it in HW4

							// ... bind each material texture, we don't have it in HW4
							d3d::s_devcon->PSSetShaderResources(0, 1, material.texPtr->textureView.GetAddressOf());

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

			//! Init the ENTIRE Meshsystem singleton, should be called right after d3d init
			void init() {
				initPipelines();
				initNormalGroup();
				initHologramGroup();
			}

			//! Do all the mesh rendering, called every frame on each group
			void render(const RenderModeFlags& flags);
			
			//! Add a new instance to groups, by filling the respective rendergroup structs
			void addNormalInstance(std::shared_ptr<mdl::Model> mod, const Material& mtrl, const Instance& inc);
			void addHologramInstance(std::shared_ptr<mdl::Model> mod, const Material& mtrl, const Instance& inc);
			//! Add offset to a specified instance, used for dragging
			void addInstanceOffset(const InstanceProperties& instanceData, const XMVECTOR& offset);
			void addInstanceRotation(const InstanceProperties& instanceData, const XMVECTOR& rotation);
			//! Get closest mesh data that was hit by a ray, used for dragging
			std::pair<bool, InstanceProperties> getClosestMesh(geom::Ray& ray, mdl::MeshIntersection& nearest);
		private:
			MeshSystem() {};

			//! Init render groups and set respective parameters
			void initNormalGroup();
			void initHologramGroup();
			//! Initialize all the pipelines
			void initPipelines();
			//! Bind a certain pipeline by type - must be called before group render
			void bindPipelineViaType(PipelineTypes pipelineType);
			
			// These can have different instances and materials, hence cannot wrap in vector:(
			RenderGroup<Instance, Material> m_normalGroup;
			RenderGroup<Instance, Material> m_hologramGroup;

			std::unordered_map<PipelineTypes, Pipeline> m_pipelines;

			D3D11_INPUT_ELEMENT_DESC DEFAULT_LAYOUT[10] = {
					{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
					{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
					{"TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
					{"BITANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
					{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
					{"MODEL2WORLD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
					{"MODEL2WORLD", 1, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
					{"MODEL2WORLD", 2, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
					{"MODEL2WORLD", 3, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
					{"COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
			};

			const std::wstring SHADER_FOLDER = util::getExeDirW();

			const std::unordered_map<PipelineTypes, PipelineData> PIPELINE_TYPE_DATA{
				{
					PipelineTypes::NORMAL_RENDER,
					PipelineData{
						DEFAULT_LAYOUT,
						ARRAYSIZE(DEFAULT_LAYOUT),
						D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
						SHADER_FOLDER + L"VSBasicColor.cso",
						L"",
						L"",
						L"",
						SHADER_FOLDER + L"PSBasicColor.cso"
					}
				},
				{
					PipelineTypes::HOLOGRAM_RENDER,
					PipelineData{
						DEFAULT_LAYOUT,
						ARRAYSIZE(DEFAULT_LAYOUT),
						D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST,
						SHADER_FOLDER + L"VSHologram.cso",
						SHADER_FOLDER + L"HSHologram.cso",
						SHADER_FOLDER + L"DSHologram.cso",
						SHADER_FOLDER + L"GSHologram.cso",
						SHADER_FOLDER + L"PSHologram.cso"
					}
				},
				{
					PipelineTypes::FACE_NORMAL_DEBUG,
					PipelineData{
						DEFAULT_LAYOUT,
						ARRAYSIZE(DEFAULT_LAYOUT),
						D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
						SHADER_FOLDER + L"VSVisNormal.cso",
						L"",
						L"",
						SHADER_FOLDER + L"GSVisNormal.cso",
						SHADER_FOLDER + L"PSVisNormal.cso"
					}
				},
				{
					PipelineTypes::WIREFRAME_DEBUG,
					PipelineData{
						DEFAULT_LAYOUT,
						ARRAYSIZE(DEFAULT_LAYOUT),
						D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
						SHADER_FOLDER + L"VSVisWireframe.cso",
						L"",
						L"",
						SHADER_FOLDER + L"GSVisWireframe.cso",
						SHADER_FOLDER + L"PSVisWireframe.cso"
					}
				},
			};
		};
	} // rend
} // engn