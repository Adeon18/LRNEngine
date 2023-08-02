#include "Caster.hpp"

#include "input/Mouse.hpp"

namespace engn {
	namespace cast {
		Caster::CollectedData Caster::castRayAtMouse(std::unique_ptr<rend::EngineCamera>& camPtr) const
		{
			CollectedData out;

			// Get ray from mouse pos
			auto& mosPos = inp::Mouse::getInstance().getMoveData();
			geom::Ray atMouse = camPtr->castRay(XMVectorGetX(mosPos.mousePos), XMVectorGetY(mosPos.mousePos));

			// Capture the mesh
			out.insHit = geom::MeshIntersection::empty();
			auto collisionRes = rend::MeshSystem::getInstance().getClosestMesh(atMouse, out.insHit);

			out.hit = collisionRes.first;
			out.insProps = collisionRes.second;

			return out;
		}
	} // cast
} // engn