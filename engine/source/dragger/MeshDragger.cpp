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
			auto collisionRes = rend::MeshSystem::getInstance().getClosestMesh(atMouse, closest);
			std::cout << "Collision Happened: " << collisionRes.first << std::endl;
			std::cout << "Collision Pos: " << closest.pos << std::endl;
			std::cout << "Collision T: " << closest.t << std::endl;
			// Save the data to the next frame if the collision happened
			if (collisionRes.first) {
				m_meshCaptured = true;
				m_capturedMeshData = collisionRes.second;
				m_capturedMeshIntersection = closest;
				// The plane points at us
				m_dragPlane = { -camPtr->getCamForward(), closest.pos };
			}

			return m_meshCaptured;
		}
		void MeshDragger::drag(std::unique_ptr<rend::EngineCamera>& camPtr)
		{
			auto& mosPos = inp::Mouse::getInstance().getMoveData();
			if (!XMVectorGetX(XMVectorEqual(mosPos.mousePos, m_oldMosPos))) {
				m_oldMosPos = mosPos.mousePos;
				geom::Ray atMousePos = camPtr->castRay(XMVectorGetX(mosPos.mousePos), XMVectorGetY(mosPos.mousePos));

				geom::BasicRayIntersection closest{ {}, {}, 1000.0f };
				atMousePos.intersect(closest, m_dragPlane.normal, m_dragPlane.pos);

				rend::MeshSystem::getInstance().addInstanceOffset(m_capturedMeshData, closest.pos - m_capturedMeshIntersection.pos);
				m_capturedMeshIntersection.pos = closest.pos;
			}
		}
		void MeshDragger::release()
		{
			m_meshCaptured = false;
		}
	} // drag
} // engn