#pragma once

namespace engn {
	namespace rend {
		//! Flags that tell the renderer and systems, how to render the scene(with normals/with wireframe)
		//! For now used for debug
		struct RenderModeFlags {
			bool renderFaceNormals = false;
			bool renderWireframes = false;
		};
	}
} // engn