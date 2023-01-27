#include "ModelManager.hpp"

namespace engn {
	namespace util {
		std::shared_ptr<model::Model> ModelManager::getModel(const std::string& filename)
		{
			try {
				return std::shared_ptr<model::Model>(m_loadedModels.at(filename));
			}
			catch (std::out_of_range& e) {
				Logger::instance().logInfo("ModelManager::Model is not cached, will perform load. Location: " + filename);
				return nullptr;
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

			model::Model model;
			//model.name = path;
			//model.box = {};
			model.getMeshes().resize(numMeshes);

			for (uint32_t i = 0; i < numMeshes; ++i) {

			}

		}
	} // util
} // engn