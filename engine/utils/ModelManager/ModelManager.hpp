#pragma once

#include <unordered_map>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "utils/Logger/Logger.hpp"

#include "source/mesh/Mesh.hpp"
#include "render/Objects/Model.hpp"

namespace engn {
	namespace mdl {
		class ModelManager {
		public:
			static constexpr uint32_t IMPORT_FLAGS = uint32_t(
				aiProcess_Triangulate |
				aiProcess_GenBoundingBoxes |
				aiProcess_ConvertToLeftHanded |
				aiProcess_CalcTangentSpace
			);
		public:
			static ModelManager& getInstance() {
				static ModelManager m;
				return m;
			}

			ModelManager(const ModelManager& other) = delete;
			ModelManager& operator=(const ModelManager& other) = delete;

			//! Loads models with a specified filename, caches it and returns a shared_ptr to it
			//! If model was cached, just returns the shared_ptr
			std::shared_ptr<Model> getModel(const std::string& filename);
			//! Get the prebuilt cube model, if there is no availible, build it and cache it
			std::shared_ptr<Model> getCubeModel();
		private:
			ModelManager() {}

			//! Load and cache the model into a map
			bool loadModel(const std::string& filename);
		private:
			std::unordered_map<std::string, std::shared_ptr<Model>> m_loadedModels;
		};
	} // mdl
} // engn