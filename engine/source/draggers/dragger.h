#pragma once

#include <glm/glm.hpp>

#include "render/RenderObject/RenderObject.h"
#include "source/math/geometry/sphere.h"
#include "source/mesh/Mesh.h"

namespace engn {
	namespace math {

		//! Abstract dragger decorator
		class IDragger {
			virtual void move(const glm::vec3& offset) = 0;
		};

		//! Sphere Dragger
		class ISphereDragger: public IDragger {
			ISphereDragger(RenderSphereObj* sphereObj) {
				sphere = sphereObj;
			}

			void move(const glm::vec3& offset) override {
			
			}

			RenderSphereObj* sphere;
		};

		//! Mesh Dragger
		class IMeshDragger: public IDragger {
			IMeshDragger(RenderMeshObj* meshObj) {
				mesh = meshObj;
			}

			void move(const glm::vec3& offset) override {

			}

			RenderMeshObj* mesh;
		};
	} // math
} // engn