#pragma once

#include <unordered_map>

#include "RenderStructs.hpp"

#include "utils/ModelManager/ModelManager.hpp"
#include "utils/TextureManager/TextureManager.hpp"

#include "render/Graphics/DXBuffers/VertexBuffer.hpp"
#include "render/Graphics/DXBuffers/ConstantBuffer.hpp"
#include "render/Graphics/DXBuffers/InstanceBuffer.hpp"

#include "render/Systems/TransformSystem.hpp"

#include "render/Systems/Pipeline.hpp"
#include "render/Systems/ShadowSubSystem.hpp"

#include "render/Graphics/HelperStructs.hpp"

#include "render/Graphics/DXRTVs/BindableDepthBuffer.hpp"
#include "render/Graphics/DXRTVs/BindableRenderTarget.hpp"

#include "render/UI/UI.hpp"

namespace engn {
	namespace rend {
		enum GroupTypes {
			NORMAL,
			DISSOLUTION,
			HOLOGRAM,
			EMISSION_ONLY
		};

		//! Struct that identifies the instance of the material of the model to be dragged
		struct InstanceProperties {
			GroupTypes group;
			std::shared_ptr<mdl::Model> model;
			uint32_t materialIdx;
			uint32_t instanceIdx;
		};

		//! The instance data struct that is in each group, stores color and index
		struct GroupInstance {
			XMVECTOR color;
			uint32_t matrixIndex;
		};

		template<typename I, typename M>
		class RenderGroup {
		public:
			struct PerMaterial {
				M material;
				std::vector<GroupInstance> instances;
			};

			using PerMesh = std::vector<PerMaterial>;

			struct PerModel {
				std::shared_ptr<mdl::Model> model;
				std::vector<PerMesh> perMesh;
			};

			std::vector<PerModel> m_models;
			InstanceBuffer<I> m_instanceBuffer;
			ConstantBuffer<CB_VS_MeshData> m_meshData;
			ConstantBuffer<CB_PS_MaterialData> m_materialData;

			//! A unique enum identifier that allows to get the group type at dragger collision - Normal by default
			GroupTypes m_type = GroupTypes::NORMAL;
			//! Group input assembler topology - triangleList by default
		public:
			//! Make the groups definable my type
			void setType(const GroupTypes& t) { m_type = t; }

			//! Init the input layout(for now the same for all)
			void init() {
				m_meshData.init();
				m_materialData.init();
			}

			// Find the closest instance that intersects with a ray and fill in the infor struct
			bool checkRayIntersection(geom::Ray& ray, geom::MeshIntersection& nearest, InstanceProperties& i2d) {
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
									TransformSystem::getInstance().getMatrixById(perInstance.matrixIndex);
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
						// Only move the first mesh instnace as the matrices are all shared
						TransformSystem::getInstance().getMatrixById(perModel.perMesh[0][insProps.materialIdx].instances[insProps.instanceIdx].matrixIndex) *= XMMatrixTranslationFromVector(offset);
						return;
					}
				}
			}

			//! Add offset to module by multiplying it by the transformation matrix
			void addModelRotation(const InstanceProperties& insProps, const XMVECTOR& rotation) {
				for (auto& perModel : m_models) {
					if (perModel.model->name == insProps.model->name) {
						// Only rotate the first mesh instnace as the matrices are all shared
						TransformSystem::getInstance().getMatrixById(perModel.perMesh[0][insProps.materialIdx].instances[insProps.instanceIdx].matrixIndex) *= XMMatrixRotationRollPitchYaw(XMVectorGetX(rotation), XMVectorGetY(rotation), XMVectorGetZ(rotation));
						return;
					}
				}
			}

			//! Remove the instance of the given model, TODO: TEST!!!
			void removeInstance(const InstanceProperties& insProps) {
				for (auto& perModel : m_models) {
					if (perModel.model->name == insProps.model->name) {
						// Erase the instance matrix(only done once)
						TransformSystem::getInstance().eraseMatrixById(perModel.perMesh[0][insProps.materialIdx].instances[insProps.instanceIdx].matrixIndex);
						for (auto& mesh : perModel) {
							mesh[insProps.materialIdx].instances.erase(mesh[insProps.materialIdx].instances.begin() + insProps.instanceIdx);
						}
						return;
					}
				}
			}

			// Add the model by filling in the respective structs and return the instance matrix ID
			uint32_t addModel(std::shared_ptr<mdl::Model> mod, const M& mtrl, const I& inc) {
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

				// Add the new instance world matrix to transform system and get the index fo later usage
				uint32_t newInstanceIdx = TransformSystem::getInstance().addMatrixById(inc.modelToWorld);

				bool isCached = false;
				for (auto& perModel : m_models) {
					if (perModel.model->name == mod->name) {
						for (auto& perMesh : perModel.perMesh) {
							for (auto& perMaterial : perMesh) {
								// Push new instance to old material if it is the same
								if (perMaterial.material == mtrl || mtrl.empty()) {
									perMaterial.instances.push_back({ inc.color, newInstanceIdx });
									modelIsAdded.addedAsInstance = true;
								}
							}
							if (!modelIsAdded.addedAsInstance) {
								PerMaterial perMat;
								perMat.material = mtrl;
								perMat.instances.push_back({ inc.color, newInstanceIdx });
								perMesh.push_back(perMat);
								modelIsAdded.addedAsMaterial = true;
							}
						}
						if (modelIsAdded.wasAdded()) {
							return newInstanceIdx;
						}
					}
				}

				Logger::instance().logInfo("Model " + mod->name + " created for the first time, creating PerModel struct...");

				PerModel newModel;
				newModel.model = std::shared_ptr<mdl::Model>(mod);

				for (auto& mesh : newModel.model->getMeshes()) {
					PerMesh perMesh;

					//! If we have textures in model and texture is not explicitly specified, use model textures
					if (mesh.texturePaths.size() > 0) {
						// TODO: Has a bug that puts only last texture as acrive in case of multiple textures per mesh
						PerMaterial perMat;
						perMat.material = {
							// TODO: CHANGE
							(!mesh.texturePaths[aiTextureTypeToString(aiTextureType_DIFFUSE)].empty()) ? tex::TextureManager::getInstance().getTexture(mesh.texturePaths[aiTextureTypeToString(aiTextureType_DIFFUSE)]) : nullptr,
							(!mesh.texturePaths[aiTextureTypeToString(aiTextureType_NORMALS)].empty()) ? tex::TextureManager::getInstance().getTexture(mesh.texturePaths[aiTextureTypeToString(aiTextureType_NORMALS)]) : nullptr,
							(!mesh.texturePaths[aiTextureTypeToString(aiTextureType_SHININESS)].empty()) ? tex::TextureManager::getInstance().getTexture(mesh.texturePaths[aiTextureTypeToString(aiTextureType_SHININESS)]) : nullptr,
							(!mesh.texturePaths[aiTextureTypeToString(aiTextureType_METALNESS)].empty()) ? tex::TextureManager::getInstance().getTexture(mesh.texturePaths[aiTextureTypeToString(aiTextureType_METALNESS)]) : nullptr,
						};
						perMat.instances.push_back({ inc.color, newInstanceIdx });
						perMesh.push_back(perMat);
					}
					else {
						PerMaterial perMat;
						perMat.material = mtrl;
						perMat.instances.push_back({inc.color, newInstanceIdx});
						perMesh.push_back(perMat);
					}
					newModel.perMesh.push_back(perMesh);
				}

				m_models.push_back(newModel);

				return newInstanceIdx;
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
								ins.modelToWorld = TransformSystem::getInstance().getMatrixById(material.instances[index].matrixIndex);
								// TODO: VERY MUSH UNOPTIMIZED - can try to save the matrix with the help of Id+1, etc.
								ins.modelToWorldInv = XMMatrixInverse(nullptr, TransformSystem::getInstance().getMatrixById(material.instances[index].matrixIndex));
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

				auto& guiMatData = UI::instance().getMatWidgetData();
				m_materialData.getData().defaultMetallicValue = XMFLOAT4{ guiMatData.defaultMetallic, guiMatData.defaultMetallic, guiMatData.defaultMetallic, guiMatData.defaultMetallic };
				m_materialData.getData().defaultRoughnessValue = XMFLOAT4{ guiMatData.defaultRoughness, guiMatData.defaultRoughness, guiMatData.defaultRoughness, guiMatData.defaultRoughness};

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
							m_materialData.getData().isDiffuseTexBound = (material.ambientTex) ? 1: 0;
							m_materialData.getData().isNormalMapBound = (material.normalMap) ? 1 : 0;
							m_materialData.getData().isRoughnessTexBound = (material.roughness && guiMatData.useTextureRoughness) ? 1 : 0;
							m_materialData.getData().isMetallicTexBound = (material.metallness && guiMatData.useTextureMetallic) ? 1 : 0;
							m_materialData.fill();
							d3d::s_devcon->PSSetConstantBuffers(1, 1, m_materialData.getBufferAddress());

							// ... bind each material texture
							ID3D11ShaderResourceView* nullSRV = nullptr;
							d3d::s_devcon->PSSetShaderResources(0, 1, (material.ambientTex) ? material.ambientTex->textureView.GetAddressOf(): &nullSRV);
							d3d::s_devcon->PSSetShaderResources(1, 1, (material.normalMap) ? material.normalMap->textureView.GetAddressOf() : &nullSRV);
							d3d::s_devcon->PSSetShaderResources(2, 1, (material.roughness) ? material.roughness->textureView.GetAddressOf() : &nullSRV);
							d3d::s_devcon->PSSetShaderResources(3, 1, (material.metallness) ? material.metallness->textureView.GetAddressOf(): &nullSRV);

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
			void init();

			//! Do all the mesh rendering, called every frame on each group
			void render(const RenderModeFlags& flags);
			//! Precompute the Directional and SpotLight shadow maps into the respective textures
			void renderDepth2D();

			//! Precompute the cubemap depths for pointlights
			void renderDepthCubemaps();
			
			//! Add a new instance to groups, by filling the respective rendergroup structs
			uint32_t addNormalInstance(std::shared_ptr<mdl::Model> mod, const Material& mtrl, const Instance& inc);
			uint32_t addDissolutionInstance(std::shared_ptr<mdl::Model> mod, const Material& mtrl, const Instance& inc);
			uint32_t addHologramInstance(std::shared_ptr<mdl::Model> mod, const Material& mtrl, const Instance& inc);
			uint32_t addEmissionInstance(std::shared_ptr<mdl::Model> mod, const Material& mtrl, const Instance& inc);
			//! Add offset to a specified instance, used for dragging
			void addInstanceOffset(const InstanceProperties& instanceData, const XMVECTOR& offset);
			void addInstanceRotation(const InstanceProperties& instanceData, const XMVECTOR& rotation);
			//! Get closest mesh data that was hit by a ray, used for dragging
			std::pair<bool, InstanceProperties> getClosestMesh(geom::Ray& ray, geom::MeshIntersection& nearest);

			[[nodiscard]] uint32_t getDefaultLayoutArraySize() const { return ARRAYSIZE(DEFAULT_LAYOUT); }
			[[nodiscard]] D3D11_INPUT_ELEMENT_DESC* getDefaultLayoutPtr() { return DEFAULT_LAYOUT; }
		private:
			MeshSystem() {};

			//! Init render groups and set respective parameters
			void initNormalGroup();
			void initDissolutionGroup();
			void initHologramGroup();
			void initEmissionGroup();
			//! Initialize all the pipelines
			void initPipelines();
			//! Bind a certain pipeline by type - must be called before group render
			void bindPipelineViaType(PipelineTypes pipelineType);

			//! Here for now
			bool shadowSystemInitialized = false;
			ShadowSubSystem m_shadowSubSystem;

			// These can have different instances and materials, hence cannot wrap in vector:(
			RenderGroup<Instance, Material> m_normalGroup;
			RenderGroup<Instance, Material> m_dissolutionGroup;
			RenderGroup<Instance, Material> m_hologramGroup;
			RenderGroup<Instance, Material> m_emissionOnlyGroup;

			std::unordered_map<PipelineTypes, Pipeline> m_pipelines;

			D3D11_INPUT_ELEMENT_DESC DEFAULT_LAYOUT[14] = {
				{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"BITANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"MODEL2WORLD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"MODEL2WORLD", 1, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"MODEL2WORLD", 2, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"MODEL2WORLD", 3, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"MODEL2WORLDINV", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"MODEL2WORLDINV", 1, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 80, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"MODEL2WORLDINV", 2, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 96, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"MODEL2WORLDINV", 3, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 112, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 128, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
			};

			D3D11_RASTERIZER_DESC DEFAULT_RASTERIZER_DESC{};
			D3D11_DEPTH_STENCIL_DESC DEFAULT_DEPTH_STENSIL_STATE_DESC{};

			const std::wstring SHADER_FOLDER = util::getExeDirW();

			std::unordered_map<PipelineTypes, PipelineData> PIPELINE_TYPE_DATA{
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
						SHADER_FOLDER + L"PSBasicColor.cso",
						D3D11_RASTERIZER_DESC{},
						D3D11_DEPTH_STENCIL_DESC{}
					}
				},
				{
					PipelineTypes::DISSOLUTION_RENDER,
					PipelineData{
						DEFAULT_LAYOUT,
						ARRAYSIZE(DEFAULT_LAYOUT),
						D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
						SHADER_FOLDER + L"VSBasicColor.cso",
						L"",
						L"",
						L"",
						SHADER_FOLDER + L"PSBasicColor.cso",
						D3D11_RASTERIZER_DESC{},
						D3D11_DEPTH_STENCIL_DESC{}
					}
				},
				{
					PipelineTypes::EMISSION_ONLY_RENDER,
					PipelineData{
						DEFAULT_LAYOUT,
						ARRAYSIZE(DEFAULT_LAYOUT),
						D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
						SHADER_FOLDER + L"VSEmission.cso",
						L"",
						L"",
						L"",
						SHADER_FOLDER + L"PSEmission.cso",
						D3D11_RASTERIZER_DESC{},
						D3D11_DEPTH_STENCIL_DESC{}
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
						SHADER_FOLDER + L"PSHologram.cso",
						D3D11_RASTERIZER_DESC{},
						D3D11_DEPTH_STENCIL_DESC{}
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
						SHADER_FOLDER + L"PSVisNormal.cso",
						D3D11_RASTERIZER_DESC{},
						D3D11_DEPTH_STENCIL_DESC{}
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
						SHADER_FOLDER + L"PSVisWireframe.cso",
						D3D11_RASTERIZER_DESC{},
						D3D11_DEPTH_STENCIL_DESC{}
					}
				},
			};
		};
	} // rend
} // engn