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
			//! Drag the captured mesh along the plane
			void drag(std::unique_ptr<rend::EngineCamera>& camPtr);
			void release();

			[[nodiscard]] bool isMeshCaptured() const { return m_meshCaptured; }
		private:
			// The data that identifies which mesh was captured
			rend::InstanceProperties m_capturedMeshData;
			// The intersection data, aquired on capture, edited during drag to keep track of the offset
			mdl::MeshIntersection m_capturedMeshIntersection;
			DragPlane m_dragPlane;
			// The mouse pos to check if we moved from previous frame MAYBE TODO: MOVE LOGIC TO MOUSE
			XMVECTOR m_oldMosPos;
			bool m_meshCaptured = false;
		};
	} // drag
} // engn