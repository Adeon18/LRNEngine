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
			ISphereDragger(RenderSphereObj* sphereObj, math::HitEntry* hitEntry) {
				sphere = sphereObj;
				centerOffset = sphere->shape->center - hitEntry->hitPoint;
			}

			void move(const glm::vec3& offset) override {
			
			}

			RenderSphereObj* sphere;
			glm::vec3 centerOffset;
		};

		//! Mesh Dragger
		class IMeshDragger: public IDragger {
		public:
			IMeshDragger(RenderMeshObj* meshObj) {
				mesh = meshObj;
			}

			void move(const glm::vec3& offset) override {

			}

			RenderMeshObj* mesh;
		};
	} // math
} // engn