#pragma once

#include <glm/glm.hpp>

#include "render/RenderObject/RenderObject.h"
#include "source/math/geometry/sphere.h"
#include "source/mesh/Mesh.h"

namespace engn {
	namespace math {

		//! Abstract dragger decorator
		class IDragger {
		public:
			virtual void move(const glm::vec3& offset) = 0;
		};

		//! Sphere Dragger
		class ISphereDragger: public IDragger {
		public:
			ISphereDragger() = delete;
			//! Takes in pointer to object, int's hitentry and a normal to a plane to create a plane in which the hitpoint is
			ISphereDragger(RenderSphereObj* sphereObj, math::HitEntry* hitEntry) {
				sphere = sphereObj;
				centerOffset = sphere->getPosition() - hitEntry->hitPoint;
			}

			void move(const glm::vec3& newPos) override {
				sphere->setPosition(newPos + centerOffset);
			}

			RenderSphereObj* sphere;
			glm::vec3 centerOffset;
		};

		//! Mesh Dragger
		class IMeshDragger: public IDragger {
		public:
			IMeshDragger(RenderMeshObj* meshObj, math::HitEntry* hitEntry) {
				mesh = meshObj;
				centerOffset = mesh->getPosition() - hitEntry->hitPoint;
			}

			void move(const glm::vec3& newPos) override {
				mesh->setPosition(newPos + centerOffset);
			}

			RenderMeshObj* mesh;
			glm::vec3 centerOffset;
		};
	} // math
} // engn