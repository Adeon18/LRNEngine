#include "dragger.h"

namespace engn {
	namespace math {
		ISphereDragger::ISphereDragger(RenderSphereObj* sphereObj, math::HitEntry* hitEntry) {
			sphere = sphereObj;
			centerOffset = sphere->getPosition() - hitEntry->hitPoint;
		}

		void ISphereDragger::move(const glm::vec3& newPos) {
			sphere->setPosition(newPos + centerOffset);
		}


		IMeshDragger::IMeshDragger(RenderMeshObj* meshObj, math::HitEntry* hitEntry) {
			mesh = meshObj;
			centerOffset = mesh->getPosition() - hitEntry->hitPoint;
		}

		void IMeshDragger::move(const glm::vec3& newPos) {
			mesh->setPosition(newPos + centerOffset);
		}
	} // math
} // engn