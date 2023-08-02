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
			//! Build the unit sphere model and cache it: DOES NOT WORK!!!!!!!!!!
			std::shared_ptr<Model> getUnitSphereModel();
		private:
			ModelManager() {}

			//! Load and cache the model into a map
			bool loadModel(const std::string& filename);
			//! Load the textures via TextureManager and store their path in meshes, to later get them
			//! From TextureManager via getTexture
			//! Also, we load ONLY .DDS TEXTURES!
			void loadTextures(const aiScene* pScene, std::shared_ptr<mdl::Model> modelPtr, aiTextureType textureType, const std::string& filename);
			//! Print all the textures paths for each mesh given the scene
			void printAllTexturesPath(const aiScene* pScene);
		private:
			std::unordered_map<std::string, std::shared_ptr<Model>> m_loadedModels;

			const std::vector<aiTextureType> TEXTURE_TYPES{
				aiTextureType_NONE,
				aiTextureType_DIFFUSE,
				aiTextureType_SPECULAR,
				aiTextureType_AMBIENT,
				aiTextureType_EMISSIVE,
				aiTextureType_HEIGHT,
				aiTextureType_NORMALS,
				aiTextureType_SHININESS,
				aiTextureType_OPACITY,
				aiTextureType_DISPLACEMENT,
				aiTextureType_LIGHTMAP,
				aiTextureType_REFLECTION,
				aiTextureType_BASE_COLOR,
				aiTextureType_NORMAL_CAMERA,
				aiTextureType_EMISSION_COLOR,
				aiTextureType_METALNESS,
				aiTextureType_DIFFUSE_ROUGHNESS,
				aiTextureType_AMBIENT_OCCLUSION,
				aiTextureType_UNKNOWN
			};
		};
	} // mdl
} // engn