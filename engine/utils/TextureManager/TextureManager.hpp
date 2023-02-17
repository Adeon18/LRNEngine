#pragma once

#include <unordered_map>

#include "render/Objects/Texture.hpp"

namespace engn {
	namespace tex {
		class TextureManager {
		public:
			static TextureManager& getInstance() {
				static TextureManager tm;
				return tm;
			}

			TextureManager(const TextureManager& t) = delete;
			TextureManager& operator=(const TextureManager& t) = delete;
			//! Load the texture that is in the path
			bool loadTexture(const std::string& path);
			std::shared_ptr<Texture> getTexture(const std::string& path);
		private:
			TextureManager() {}

			std::unordered_map<std::string, std::shared_ptr<Texture>> m_loadedTextures;
		};
	} // tex
} // engn