#pragma once

#include "RenderStructs.hpp"

#include "utils/ModelManager/ModelManager.hpp"
#include "render/Objects/Model.hpp"

#include "render/Graphics/VertexBuffer.hpp"
#include "render/Graphics/ConstantBuffer.hpp"
#include "render/Graphics/InstanceBuffer.hpp"

#include "render/Graphics/DXShaders/VertexShader.hpp"
#include "render/Graphics/DXShaders/HullShader.hpp"
#include "render/Graphics/DXShaders/DomainShader.hpp"
#include "render/Graphics/DXShaders/GeometryShader.hpp"
#include "render/Graphics/DXShaders/PixelShader.hpp"

#include "render/Graphics/Vertex.hpp"

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
			D3D11_PRIMITIVE_TOPOLOGY m_topology = D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

			VertexShader m_vertexShader;
			HullShader m_hullShader;
			DomainShader m_domainShader;
			GeometryShader m_geometryShader;
			PixelShader m_pixelShader;
		public:
			//! Make the groups definable my type
			void setType(const GroupTypes& t) { m_type = t; }
			//! Set the IA topology for the group
			void setTopology(const D3D11_PRIMITIVE_TOPOLOGY& t) { m_topology = t; }

			//! Init the input layout(for now the same for all)
			void init(const std::wstring& VSpath, const std::wstring& HSpath, const std::wstring& DSpath, const std::wstring& GSpath, const std::wstring& PSpath) {
				D3D11_INPUT_ELEMENT_DESC layout[] = {
					{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
					{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
					{"TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
					{"BITANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
					{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 44, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
					{"MODEL2WORLD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
					{"MODEL2WORLD", 1, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
					{"MODEL2WORLD", 2, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
					{"MODEL2WORLD", 3, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
					{"COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				};

				m_vertexShader.init(VSpath, layout, ARRAYSIZE(layout));
				m_hullShader.init(HSpath);
				m_domainShader.init(DSpath);
				m_geometryShader.init(GSpath);
				m_pixelShader.init(PSpath);
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

			// Add the model by filling in the respective structs
			void addModel(std::shared_ptr<mdl::Model> mod, const M& mtrl, const I& inc) {
				if (!mod) {
					Logger::instance().logErr("addModel: The model pointer is null");
				}

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

			void render() {
				if (m_instanceBuffer.getSize() == 0)
					return;

				d3d::s_devcon->IASetInputLayout(m_vertexShader.getInputLayout());
				d3d::s_devcon->IASetPrimitiveTopology(m_topology);
				m_vertexShader.bind();
				m_hullShader.bind();
				m_domainShader.bind();
				m_geometryShader.bind();
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
				initNormalGroup();
				initHologramGroup();
			}

			//! Init render gruups and set respective parameters
			void initNormalGroup();
			void initHologramGroup();
			//! Call render on each group
			void render();
			
			//! Add a new instance to groups, by filling the respective rendergroup structs
			void addNormalInstance(std::shared_ptr<mdl::Model> mod, const Material& mtrl, const Instance& inc);
			void addHologramInstance(std::shared_ptr<mdl::Model> mod, const Material& mtrl, const Instance& inc);
			//! Add offset to a specified instance, used for dragging
			void addInstanceOffset(const InstanceProperties& instanceData, const XMVECTOR& offset);
			//! Get closest mesh data that was hit by a ray, used for dragging
			std::pair<bool, InstanceProperties> getClosestMesh(geom::Ray& ray, mdl::MeshIntersection& nearest);
		private:
			MeshSystem() {};
			
			// These can have different instances and materials, hence cannot wrap in vector:(
			RenderGroup<Instance, Material> m_normalGroup;
			RenderGroup<Instance, Material> m_hologramGroup;
		};
	} // rend
} // engn