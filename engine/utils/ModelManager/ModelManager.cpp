#include "ModelManager.hpp"

#include "utility/utility.hpp"

namespace engn {
	namespace util {
		std::shared_ptr<model::Model> ModelManager::getModel(const std::string& filename)
		{
			try {
				return std::shared_ptr<model::Model>(m_loadedModels.at(filename));
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

			return std::shared_ptr<model::Model>(m_loadedModels.at(filename));
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

			std::shared_ptr<model::Model> modelPtr = std::shared_ptr<model::Model>(new model::Model{});
			//model.name = path;
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

				for (uint32_t v = 0; v < assimpMesh->mNumVertices; ++v)
				{
					Vertex& vertex = modelMesh.vertices[v];
					vertex.pos = util::aiVector3DtoXMFLOAT3(assimpMesh->mVertices[v]);
					//vertex.tc = (assimpMesh->mTextureCoords[0][v]);
					//vertex.normal = (assimpMesh->mNormals[v]);
					//vertex.tangent = (assimpMesh->mTangents[v]);
					//vertex.bitangent = (assimpMesh->mBitangents[v]) * -1.f; // Flip V
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
					modelPtr->getMeshes()[meshIndex].instances.push_back(nodeToParent);
					modelPtr->getMeshes()[meshIndex].instancesInv.push_back(parentToNode);
				}

				for (uint32_t i = 0; i < node->mNumChildren; ++i)
					loadInstances(node->mChildren[i]);
			};

			loadInstances(assimpScene->mRootNode);

			m_loadedModels[filename] = modelPtr;
			return true;
		}
	} // util
} // engn