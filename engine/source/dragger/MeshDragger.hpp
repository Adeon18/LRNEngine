#pragma once

#include <memory>

#include <DirectXMath.h>

#include "render/Systems/MeshSystem.hpp"
#include "input/Mouse.hpp"
#include "render/Graphics/EngineCamera.hpp"

namespace engn {
	namespace drag {
		using namespace DirectX;
		class MeshDragger {
		public:
			//! The plane along which you drag your mesh
			struct DragPlane {
				XMVECTOR normal;
				XMVECTOR pos;
			};
			//! Capture the data about the collided mesh and generate a plane for the drag-along
			bool capture(std::unique_ptr<rend::EngineCamera>& camPtr);
			//!
			void drag(std::unique_ptr<rend::EngineCamera>& camPtr);
			void release();

			[[nodiscard]] bool isMeshCaptured() const { return m_meshCaptured; }
		private:
			rend::InstanceToDrag m_capturedMeshData;
			DragPlane m_dragPlane;
			XMVECTOR m_oldMosPos;
			bool m_meshCaptured = false;
		};
	} // drag
} // engn