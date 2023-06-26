#include "ModelManager.hpp"

#include "utils/TextureManager/TextureManager.hpp"

#include "utility/utility.hpp"

namespace engn {
	namespace mdl {
		std::shared_ptr<Model> ModelManager::getModel(const std::string& filename)
		{
			if (m_loadedModels.find(filename) != m_loadedModels.end()) {
				return m_loadedModels[filename];
			}

			Logger::instance().logInfo("ModelManager: Model is not cached, will perform load. Location: " + filename);

			if (!loadModel(filename)) {
				return nullptr;
			}
			
			return m_loadedModels[filename];
		}

		std::shared_ptr<Model> ModelManager::getCubeModel()
		{
			if (m_loadedModels.find("unit_box") != m_loadedModels.end()) {
				return m_loadedModels["unit_box"];
			}
			Logger::instance().logInfo("ModelManager::Model is not cached, will perform load. Location: unit_box");

			m_loadedModels.insert({ "unit_box", std::make_shared<mdl::Model>() });
			std::vector vertices =
			{
				Vertex{{1.0f, 1.0f, -1.0f}, {0.33f, 0.33f, -0.33f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f}}, // top-right-front
				Vertex{{-1.0f, 1.0f, -1.0f}, {-0.33f, 0.33f, -0.33f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f}}, // top-left-front
				Vertex{{1.0f, -1.0f, -1.0f}, {0.33f, -0.33f, -0.33f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f}}, // bottom-right-front
				Vertex{{-1.0f, -1.0f, -1.0f}, {-0.33f, -0.33f, -0.33f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f}}, // bottom-left-front

				Vertex{{1.0f, 1.0f, 1.0f}, {0.33f, 0.33f, 0.33f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f}}, // top-right-back
				Vertex{{-1.0f, 1.0f, 1.0f}, {-0.33f, 0.33f, 0.33f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f}}, // top-left-back
				Vertex{{1.0f, -1.0f, 1.0f}, {0.33f, -0.33f, 0.33f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f}}, // bottom-right-back
				Vertex{{-1.0f, -1.0f, 1.0f}, {-0.33f, -0.33f, 0.33f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f}}, // bottom-left-back
			};
			std::vector<DWORD> indices =
			{
				1, 0, 2, // front right
				1, 2, 3, // front left

				2, 0, 4, // right left
				2, 4, 6, // right right

				3, 5, 1, // left right
				3, 7, 5, // left left

				6, 5, 7, // back right
				6, 4, 5, // back left

				1, 5, 4, // top right
				1, 4, 0, // top left

				3, 2, 6, // back right
				3, 6, 7, // back left
			};

			Mesh boxMesh;
			boxMesh.name = "unit_box";
			boxMesh.box = geom::BoundingBox::unit();
			boxMesh.vertices = vertices;
			for (size_t i = 0; i < indices.size(); i += 3) {
				Mesh::Triangle t{ indices[i], indices[i + 1], indices[i + 2] };
				boxMesh.triangles.push_back(t);
			}
			boxMesh.meshToModel = XMMatrixIdentity();
			boxMesh.meshToModelInv = XMMatrixIdentity();

			Model::MeshRange boxMeshRange{ 0, 0, vertices.size(), indices.size() };

			m_loadedModels["unit_box"]->name = "unit_box";
			m_loadedModels["unit_box"]->getMeshes().push_back(boxMesh);
			m_loadedModels["unit_box"]->getRanges().push_back(boxMeshRange);

			// Initialize OcTree
			util::TriangleOctree boxOcTree;
			boxOcTree.initialize(m_loadedModels["unit_box"]->getMeshes()[0]);
			m_loadedModels["unit_box"]->getMeshOcTrees().push_back(std::move(boxOcTree));
			
			m_loadedModels["unit_box"]->fillBuffersFromMeshes();

			return m_loadedModels["unit_box"];
		}

		std::shared_ptr<Model> ModelManager::getUnitSphereModel()
		{
			if (m_loadedModels.find("UNIT_SPHERE_FLAT") != m_loadedModels.end()) {
				return m_loadedModels["UNIT_SPHERE_FLAT"];
			}

			std::shared_ptr<mdl::Model> modelPtr = std::shared_ptr<mdl::Model>(new mdl::Model{});

			const uint32_t SIDES = 6;
			const uint32_t GRID_SIZE = 12;
			const uint32_t TRIS_PER_SIDE = GRID_SIZE * GRID_SIZE * 2;
			const uint32_t VERT_PER_SIZE = 3 * TRIS_PER_SIDE;

			modelPtr->name = "UNIT_SPHERE_FLAT";
			//modelPtr->box = engine::BoundingBox::empty();

			//modelPtr->getMeshOcTrees().resize(1);
			Mesh& mesh = modelPtr->getMeshes().emplace_back();

			mesh.name = "UNIT_SPHERE_FLAT";
			mesh.box = geom::BoundingBox::unit();
			mesh.meshToModel = XMMatrixIdentity();
			mesh.meshToModelInv = XMMatrixIdentity();

			mesh.vertices.resize(VERT_PER_SIZE * SIDES);
			//mesh.triangles.resize(TRIS_PER_SIDE * SIDES);
			Vertex* vertex = mesh.vertices.data();

			int sideMasks[6][3] =
			{
				{ 2, 1, 0 },
				{ 0, 1, 2 },
				{ 2, 1, 0 },
				{ 0, 1, 2 },
				{ 0, 2, 1 },
				{ 0, 2, 1 }
			};

			float sideSigns[6][3] =
			{
				{ +1, +1, +1 },
				{ -1, +1, +1 },
				{ -1, +1, -1 },
				{ +1, +1, -1 },
				{ +1, -1, -1 },
				{ +1, +1, +1 }
			};

			int vertexCount = 0;
			for (int side = 0; side < SIDES; ++side)
			{
				for (int row = 0; row < GRID_SIZE; ++row)
				{
					for (int col = 0; col < GRID_SIZE; ++col)
					{
						float left = (col + 0) / float(GRID_SIZE) * 2.f - 1.f;
						float right = (col + 1) / float(GRID_SIZE) * 2.f - 1.f;
						float bottom = (row + 0) / float(GRID_SIZE) * 2.f - 1.f;
						float top = (row + 1) / float(GRID_SIZE) * 2.f - 1.f;

						XMFLOAT3 quad[4] =
						{
							{ left, bottom, 1.f },
							{ right, bottom, 1.f },
							{ left, top, 1.f },
							{ right, top, 1.f }
						};

						vertex[0] = vertex[1] = vertex[2] = vertex[3] = Vertex{};

						auto setXMFLOATdataFromIdx = [](Vertex& dst, int idx, float value) {
							switch (idx) {
							case 0:
								dst.pos.x = value;
								break;
							case 1:
								dst.pos.y = value;
								break;
							case 2:
								dst.pos.z = value;
								break;
							}
						};

						auto setPos = [sideMasks, sideSigns, setXMFLOATdataFromIdx](int side, Vertex& dst, const XMFLOAT3& pos)
						{
							/*dst.pos[sideMasks[side][0]] = pos.x * sideSigns[side][0];
							dst.pos[sideMasks[side][1]] = pos.y * sideSigns[side][1];
							dst.pos[sideMasks[side][2]] = pos.z * sideSigns[side][2];
							dst.pos = dst.pos.normalized();*/

							setXMFLOATdataFromIdx(dst, sideMasks[side][0], pos.x * sideSigns[side][0]);
							setXMFLOATdataFromIdx(dst, sideMasks[side][1], pos.x * sideSigns[side][1]);
							setXMFLOATdataFromIdx(dst, sideMasks[side][2], pos.x * sideSigns[side][2]);

							XMVECTOR dstXM = XMLoadFloat3(&dst.pos);
							XMVECTOR normDstXM = XMVector3Normalize(dstXM);

							XMStoreFloat3(&dst.pos, normDstXM);
						};

						setPos(side, vertex[0], quad[0]);
						setPos(side, vertex[1], quad[1]);
						setPos(side, vertex[2], quad[2]);

						{
							XMVECTOR vertex0Pos = XMLoadFloat3(&vertex[0].pos);
							XMVECTOR vertex1Pos = XMLoadFloat3(&vertex[1].pos);
							XMVECTOR vertex2Pos = XMLoadFloat3(&vertex[2].pos);

							//Vec3f AB = vertex[1].pos - vertex[0].pos;
							//Vec3f AC = vertex[2].pos - vertex[0].pos;
							//vertex[0].normal = vertex[1].normal = vertex[2].normal = AC.cross(AB).normalized();

							XMVECTOR AB = vertex1Pos - vertex0Pos;
							XMVECTOR AC = vertex2Pos - vertex0Pos;
							XMVECTOR crossNorm = XMVector3Normalize(XMVector3Cross(AC, AB));
							XMFLOAT3 crossNormF3;
							XMStoreFloat3(&crossNormF3, crossNorm);
							vertex[0].normal = vertex[1].normal = vertex[2].normal = crossNormF3;
						}

						Mesh::Triangle t{vertexCount, vertexCount + 1, vertexCount + 2};
						mesh.triangles.push_back(t);

						vertex += 3;
						vertexCount += 3;

						setPos(side, vertex[0], quad[1]);
						setPos(side, vertex[1], quad[3]);
						setPos(side, vertex[2], quad[2]);

						{
							/*Vec3f AB = vertex[1].pos - vertex[0].pos;
							Vec3f AC = vertex[2].pos - vertex[0].pos;
							vertex[0].normal = vertex[1].normal = vertex[2].normal = AC.cross(AB).normalized();*/
							XMVECTOR vertex0Pos = XMLoadFloat3(&vertex[0].pos);
							XMVECTOR vertex1Pos = XMLoadFloat3(&vertex[1].pos);
							XMVECTOR vertex2Pos = XMLoadFloat3(&vertex[2].pos);

							XMVECTOR AB = vertex1Pos - vertex0Pos;
							XMVECTOR AC = vertex2Pos - vertex0Pos;
							XMVECTOR crossNorm = XMVector3Normalize(XMVector3Cross(AC, AB));
							XMFLOAT3 crossNormF3;
							XMStoreFloat3(&crossNormF3, crossNorm);
							vertex[0].normal = vertex[1].normal = vertex[2].normal = crossNormF3;
						}
						Mesh::Triangle t2{ vertexCount, vertexCount + 1, vertexCount + 2 };
						mesh.triangles.push_back(t2);

						vertex += 3;
						vertexCount += 3;
					}
				}
			}

			Model::MeshRange boxMeshRange{ 0, 0, VERT_PER_SIZE * SIDES, VERT_PER_SIZE * SIDES };
			modelPtr->getRanges().push_back(boxMeshRange);

			// Initialize OcTree
			//util::TriangleOctree boxOcTree;
			//boxOcTree.initialize(modelPtr->getMeshes()[0]);
			//modelPtr->getMeshOcTrees().push_back(std::move(boxOcTree));

			modelPtr->fillBuffersFromMeshes();

			m_loadedModels["UNIT_SPHERE_FLAT"] = modelPtr;

			return modelPtr;
		}

		bool ModelManager::loadModel(const std::string& filename)
		{
			Assimp::Importer importer;
			const aiScene* assimpScene = importer.ReadFile(filename, IMPORT_FLAGS);
			if (!assimpScene) {
				Logger::instance().logInfo("ModelManager::Failed loading model. Location: " + filename);
				return false;
			}

			uint32_t numMeshes = assimpScene->mNumMeshes;

			// Load vertex data

			std::shared_ptr<mdl::Model> modelPtr = std::shared_ptr<mdl::Model>(new mdl::Model{});
			modelPtr->name = filename;
			//model.box = {};
			modelPtr->getMeshes().resize(numMeshes);
			modelPtr->getMeshOcTrees().resize(numMeshes);
			
			XMFLOAT3 xyzBoxMax = {
				assimpScene->mMeshes[0]->mAABB.mMax.x,
				assimpScene->mMeshes[0]->mAABB.mMax.y,
				assimpScene->mMeshes[0]->mAABB.mMax.z,
			};
			XMFLOAT3 xyzBoxMin = {
				assimpScene->mMeshes[0]->mAABB.mMin.x,
				assimpScene->mMeshes[0]->mAABB.mMin.y,
				assimpScene->mMeshes[0]->mAABB.mMin.z,
			};

			// Load all meshes t
			for (uint32_t i = 0; i < numMeshes; ++i) {
				auto& assimpMesh = assimpScene->mMeshes[i];
				auto& modelMesh = modelPtr->getMeshes()[i];

				modelMesh.name = assimpMesh->mName.C_Str();

				auto maxVec = assimpMesh->mAABB.mMax;
				auto minVec = assimpMesh->mAABB.mMin;

				if (minVec.x < xyzBoxMin.x) xyzBoxMin.x = minVec.x;
				if (minVec.y < xyzBoxMin.y) xyzBoxMin.y = minVec.y;
				if (minVec.z < xyzBoxMin.z) xyzBoxMin.z = minVec.z;

				if (maxVec.x > xyzBoxMax.x) xyzBoxMax.x = maxVec.x;
				if (maxVec.y > xyzBoxMax.y) xyzBoxMax.y = maxVec.y;
				if (maxVec.z > xyzBoxMax.z) xyzBoxMax.z = maxVec.z;

				modelMesh.box.setMin(util::aiVector3DtoXMVECTOR(minVec));
				modelMesh.box.setMax(util::aiVector3DtoXMVECTOR(maxVec));

				modelMesh.vertices.resize(assimpMesh->mNumVertices);
				modelMesh.triangles.resize(assimpMesh->mNumFaces);

				// Triangle vertices
				for (uint32_t v = 0; v < assimpMesh->mNumVertices; ++v)
				{
					Vertex& vertex = modelMesh.vertices[v];
					vertex.pos = util::aiVector3DtoXMFLOAT3(assimpMesh->mVertices[v]);
					//! WARNING! IS THIS RIGHT???? WE WILL SEEEE
					auto tcVec3 = util::aiVector3DtoXMFLOAT3(assimpMesh->mTextureCoords[0][v]);
					vertex.tc = XMFLOAT2{ tcVec3.x, tcVec3.y };
					vertex.normal = util::aiVector3DtoXMFLOAT3(assimpMesh->mNormals[v]);
					vertex.tangent = util::aiVector3DtoXMFLOAT3(assimpMesh->mTangents[v]);

					auto bitanOrig = util::aiVector3DtoXMFLOAT3((assimpMesh->mBitangents[v]));
					vertex.bitangent = XMFLOAT3{ bitanOrig.x * -1.f, bitanOrig.y * -1.f, bitanOrig.z * -1.f }; // Flip V
				}
				// Triangle faces
				for (uint32_t f = 0; f < assimpMesh->mNumFaces; ++f)
				{
					const auto& face = assimpMesh->mFaces[f];
					for (uint32_t j = 0; j < face.mNumIndices; ++j) {
						modelMesh.triangles[f].indices[j] = face.mIndices[j];
					}
				}

				// Initialize ocTree
				modelPtr->getMeshOcTrees()[i].initialize(modelPtr->getMeshes()[i]);
			}

			modelPtr->box.setMin(xyzBoxMin);
			modelPtr->box.setMax(xyzBoxMax);
			

			// Load the textures
			loadTextures(assimpScene, modelPtr, aiTextureType_DIFFUSE, filename);
			loadTextures(assimpScene, modelPtr, aiTextureType_NORMALS, filename);
			loadTextures(assimpScene, modelPtr, aiTextureType_SHININESS, filename);
			loadTextures(assimpScene, modelPtr, aiTextureType_METALNESS, filename);

			std::function<void(aiNode*)> loadInstances = [&loadInstances, &modelPtr](aiNode* node)
			{
				const XMMATRIX nodeToParent = util::aiMatrix4x4toXMMATRIX(node->mTransformation.Transpose());

				const XMMATRIX parentToNode = XMMatrixInverse(nullptr, nodeToParent);

				// The same node may contain multiple meshes in its space, referring to them by indices
				for (uint32_t i = 0; i < node->mNumMeshes; ++i)
				{
					uint32_t meshIndex = node->mMeshes[i];
					modelPtr->getMeshes()[meshIndex].meshToModel = nodeToParent;
					modelPtr->getMeshes()[meshIndex].meshToModelInv = parentToNode;
				}

				for (uint32_t i = 0; i < node->mNumChildren; ++i) {
					loadInstances(node->mChildren[i]);
				}
			};

			loadInstances(assimpScene->mRootNode);

			modelPtr->fillBuffersFromMeshes();

			m_loadedModels[filename] = modelPtr;
			return true;
		}
		void ModelManager::loadTextures(const aiScene* pScene, std::shared_ptr<mdl::Model> modelPtr, aiTextureType textureType, const std::string& filename)
		{
			uint32_t numMeshes = pScene->mNumMeshes;

			// Load all meshes textures
			for (uint32_t i = 0; i < numMeshes; ++i) {
				auto& assimpMesh = pScene->mMeshes[i];
				auto& modelMesh = modelPtr->getMeshes()[i];
				// Load textures
				aiMaterial* material = pScene->mMaterials[assimpMesh->mMaterialIndex];
				uint32_t textureCount = material->GetTextureCount(textureType);

				for (uint32_t t = 0; t < textureCount; ++t) {
					aiString path;
					material->GetTexture(textureType, t, &path);
					std::string fullTexturePath = util::getDirectoryFromPath(filename) + util::changeFileExt(path.C_Str(), ".dds");
					// Load texture by full path and save the full path
					tex::TextureManager::getInstance().loadTextureDDS(fullTexturePath);
					modelMesh.texturePaths[aiTextureTypeToString(textureType)] = fullTexturePath;
				}
			}
		}
		void ModelManager::printAllTexturesPath(const aiScene* pScene)
		{
			uint32_t numMeshes = pScene->mNumMeshes;

			for (uint32_t i = 0; i < numMeshes; ++i) {
				auto& pMesh = pScene->mMeshes[i];
				aiMaterial* material = pScene->mMaterials[pMesh->mMaterialIndex];
				for (auto texType : TEXTURE_TYPES) {
					uint32_t textureCount = material->GetTextureCount(texType);
					std::cout << "Type: " << aiTextureTypeToString(texType) << " : Count : " << textureCount << std::endl;
					for (uint32_t t = 0; t < textureCount; ++t) {
						aiString path;
						material->GetTexture(texType, t, &path);
						std::cout << "path: " << path.C_Str() << std::endl;
					}
				}
			}
		}
	} // mdl
} // engn