#include <filesystem>

#include "DDSTextureLoader11.h"

#include "utils/Logger/Logger.hpp"

#include "render/D3D/d3d.hpp"

#include "TextureManager.hpp"

namespace engn {
	namespace tex {
		bool TextureManager::loadTexture(const std::string& path)
		{
			// Accept only bbs
			if (std::filesystem::path(path).extension() != ".bbs") {
				Logger::instance().logInfo("The specified path: " + path + " is not of .bbs format");
				return false;
			}
			// Read the texture
			std::shared_ptr<Texture> texPtr = std::shared_ptr<Texture>{ new Texture{} };
			HRESULT hr = DirectX::CreateDDSTextureFromFile(
				d3d::s_device,
				util::stringToWstring(path).c_str(),
				texPtr->texture.GetAddressOf(),
				texPtr->textureView.GetAddressOf()
			);
			// Failed to load
			if (FAILED(hr)) {
				Logger::instance().logInfo("Failed to load texture at " + path);
				return false;
			}
			// Put in map
			m_loadedTextures[path] = texPtr;

			return true;
		}
		std::shared_ptr<Texture> TextureManager::getTexture(const std::string& path)
		{
			try {
				return m_loadedTextures.at(path);
			}
			catch (std::out_of_range& e) {
				Logger::instance().logInfo("TextureManager: Texture is not cached, will perform load. Location: " + path);
			}
			catch (...) {
				Logger::instance().logInfo("TextureManager: Unknown exception at texture load.Location : " + path);
				return nullptr;
			}

			if (!loadTexture(path)) {
				return nullptr;
			}

			return m_loadedTextures.at(path);
		}
	} // tex
} // engn