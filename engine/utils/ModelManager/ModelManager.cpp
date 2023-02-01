#include "ModelManager.hpp"

#include "utility/utility.hpp"

namespace engn {
	namespace mdl {
		std::shared_ptr<Model> ModelManager::getModel(const std::string& filename)
		{
			try {
				return m_loadedModels.at(filename);
			}
			catch (std::out_of_range& e) {
				Logger::instance().logInfo("ModelManager::Model is not cached, will perform load. Location: " + filename);
			}
			catch (...) {
				Logger::instance().logInfo("ModelManager::Unknown exception at model load. Location: " + filename);
				return nullptr;
			}

			if (!loadModel(filename)) {
				return nullptr;
			}
			
			return m_loadedModels.at(filename);
		}

		std::shared_ptr<Model> ModelManager::getCubeModel()
		{
			try {
				return m_loadedModels.at("unit_box");
			}
			catch (const std::out_of_range& e) {
				Logger::instance().logInfo("ModelManager::Model is not cached, will perform load. Location: unit_box");
			}

			m_loadedModels.insert({ "unit_box", std::make_shared<mdl::Model>() });
			std::vector vertices =
			{
				Vertex{{0.5f, 0.5f, -0.5f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f}}, // top-right-front
				Vertex{{-0.5f, 0.5f, -0.5f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f}}, // top-left-front
				Vertex{{0.5f, -0.5f, -0.5f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f}}, // bottom-right-front
				Vertex{{-0.5f, -0.5f, -0.5f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f}}, // bottom-left-front

				Vertex{{0.5f, 0.5f, 0.5f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f}}, // top-right-back
				Vertex{{-0.5f, 0.5f, 0.5f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f}}, // top-left-back
				Vertex{{0.5f, -0.5f, 0.5f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f}}, // bottom-right-back
				Vertex{{-0.5f, -0.5f, 0.5f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f}}, // bottom-left-back
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
			boxMesh.box = BoundingBox::unit();
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
			
			m_loadedModels["unit_box"]->fillBuffersFromMeshes();

			return m_loadedModels["unit_box"];
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

			// Load all meshes t
			for (uint32_t i = 0; i < numMeshes; ++i) {
				auto& assimpMesh = assimpScene->mMeshes[i];
				auto& modelMesh = modelPtr->getMeshes()[i];

				modelMesh.name = assimpMesh->mName.C_Str();
				modelMesh.box.setMin(util::aiVector3DtoXMVECTOR(assimpMesh->mAABB.mMin));
				modelMesh.box.setMax(util::aiVector3DtoXMVECTOR(assimpMesh->mAABB.mMax));

				modelMesh.vertices.resize(assimpMesh->mNumVertices);
				modelMesh.triangles.resize(assimpMesh->mNumFaces);

				// TODO: Beautify
				for (uint32_t v = 0; v < assimpMesh->mNumVertices; ++v)
				{
					Vertex& vertex = modelMesh.vertices[v];
					vertex.pos = util::aiVector3DtoXMFLOAT3(assimpMesh->mVertices[v]);
					auto tcc = util::aiVector3DtoXMFLOAT3(assimpMesh->mTextureCoords[0][v]);
					vertex.tc = XMFLOAT2{tcc.x, tcc.y};
					vertex.normal = util::aiVector3DtoXMFLOAT3(assimpMesh->mNormals[v]);
					vertex.tangent = util::aiVector3DtoXMFLOAT3(assimpMesh->mTangents[v]);
					auto btanOrig = util::aiVector3DtoXMFLOAT3((assimpMesh->mBitangents[v]));
					vertex.bitangent = XMFLOAT3{ btanOrig.x * -1.f, btanOrig.y * -1.f, btanOrig.z * -1.f }; // Flip V
				}

				for (uint32_t f = 0; f < assimpMesh->mNumFaces; ++f)
				{
					const auto& face = assimpMesh->mFaces[f];
					// TODO: Check for 3 indices
					for (uint32_t j = 0; j < face.mNumIndices; ++j) {
						modelMesh.triangles[f].indices[j] = face.mIndices[j];
					}
				}
			}

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
	} // mdl
} // engn