#include "MeshDragger.hpp"


namespace engn {
	namespace drag {
		bool MeshDragger::capture(std::unique_ptr<rend::EngineCamera>& camPtr)
		{
			// Get ray from mouse pos
			auto& mosPos = inp::Mouse::getInstance().getMoveData();
			m_oldMosPos = mosPos.mousePos;
			geom::Ray atMouse = camPtr->castRay(XMVectorGetX(mosPos.mousePos), XMVectorGetY(mosPos.mousePos));

			// Capture the mesh
			mdl::MeshIntersection closest{ {}, {}, 1000.0f, 0 };
			auto collisionRes = rend::MeshSystem::getInstance().getClosestNormalMesh(atMouse, closest);

			if (collisionRes.first) {
				m_meshCaptured = true;
				m_capturedMeshData = collisionRes.second;
				// The plane points at us
				m_dragPlane = { -camPtr->getCamForward(), closest.pos };
			}

			return m_meshCaptured;
		}
		void MeshDragger::drag(std::unique_ptr<rend::EngineCamera>& camPtr)
		{
			if (m_meshCaptured) {
				auto& mosPos = inp::Mouse::getInstance().getMoveData();
				if (!XMVectorGetX(XMVectorEqual(mosPos.mousePos, m_oldMosPos))) {
					m_oldMosPos = mosPos.mousePos;
					geom::Ray atMousePos = camPtr->castRay(XMVectorGetX(mosPos.mousePos), XMVectorGetY(mosPos.mousePos));

					geom::BasicRayIntersection closest{ {}, {}, 1000.0f };
					atMousePos.intersect(closest, m_dragPlane.normal, m_dragPlane.pos);

					std::cout << "New pos: " << closest.pos << std::endl;

					rend::MeshSystem::getInstance().setNormalInstancePosition(m_capturedMeshData.model, m_capturedMeshData.materialIdx, m_capturedMeshData.instanceIdx, closest.pos);
				}
			}
		}
		void MeshDragger::release()
		{
			m_meshCaptured = false;
		}
	} // drag
} // engn