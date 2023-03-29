#pragma once

#include <string>

namespace engn {
	namespace rend {
		//! Flags that tell the renderer and systems, how to render the scene(with normals/with wireframe)
		//! For now used for debug
		struct RenderModeFlags {
			bool renderFaceNormals = false;
			bool renderWireframes = false;
			bool bindFlashlight = true;
		};

		struct MaterialTexturePaths {
			std::string albedo;
			std::string normalMap;
			std::string roughness;
			std::string metallic;
		};
	}
} // engn