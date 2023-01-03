#pragma once

#include "render/Materials/Material.h"
#include "source/math/geometry/hitable.h"
#include "source/math/geometry/sphere.h"
#include "source/math/geometry/plane.h"
#include "render/Lighting/LightSources.h"
#include "source/math/ray.h"
#include "source/mesh/Mesh.h"
#include "source/mesh/TriangleOcTree.h"


namespace engn {

	enum class RenderType {
		SPHERE,
		PLANE,
		MESH,
		// Even though Point and SpotLight are visualized by spheres,
		// this will help us tell the difference when not to process shadows for example
		POINTLIGHT,
		SPOTLIGHT,
		NONE
	};

	struct ObjRef {
		void* object;
		mtrl::Material* material;
		RenderType type;
	};

	//! A RAII wrapper foe the math sphere class
	struct RenderSphereObj {

		RenderSphereObj(math::sphere* shapePtr, const mtrl::Material& mat) : material{ mat } {
			shape = shapePtr;
		}

		~RenderSphereObj() {
			delete shape;
		}

		bool hit(const math::ray& ray, math::HitEntry& nearest, ObjRef& objRef) {
			if (shape->hit(ray, nearest)) {
				objRef.object = this;
				objRef.material = &material;
				objRef.type = RenderType::SPHERE;
				return true;
			}
			return false;
		}

		math::sphere* shape;
		mtrl::Material material;
	};

	//! A RAII wrapper for the plane object
	struct RenderPlaneObj {

		RenderPlaneObj(math::plane* shapePtr, const mtrl::Material& mat) : material{ mat } {
			shape = shapePtr;
		}

		~RenderPlaneObj() {
			delete shape;
		}

		bool hit(const math::ray& ray, math::HitEntry& nearest, ObjRef& objRef) {
			if (shape->hit(ray, nearest)) {
				objRef.object = this;
				objRef.material = &material;
				objRef.type = RenderType::PLANE;
				return true;
			}
			return false;
		}

		math::plane* shape;
		mtrl::Material material;
	};

	//! A simple RAII wrapper for rendering and wrapping sped up intersection for a mesh object
	struct RenderMeshObj {
		RenderMeshObj(mesh::Mesh* msh, const mtrl::Material& mat, const glm::vec3& mshPos) :
			material{ mat }
		{
			// Mesh data
			mesh = msh;
			collideOcTree.initialize(*mesh);
			// Precalculate matrices
			modelMatrix = glm::translate(modelMatrixInv, mshPos);
			modelMatrixInv = glm::inverse(modelMatrix);
		}

		bool hit(math::ray& ray, math::HitEntry& nearest, ObjRef& objRef) {
			auto prevRayOrigin = ray.origin;
			ray.transform(modelMatrixInv);

			if (collideOcTree.intersect(ray, nearest)) {
				// Yes
				nearest.hitPoint = modelMatrix * glm::vec4(nearest.hitPoint, 1.0f);
				
				objRef.object = this;
				objRef.material = &material;
				objRef.type = RenderType::MESH;

				ray.origin = prevRayOrigin;

				return true;
			}
			ray.origin = prevRayOrigin;

			return false;
		}

		mesh::Mesh* mesh;

		glm::mat4 modelMatrix = glm::mat4(1.0f);
		glm::mat4 modelMatrixInv = glm::mat4(1.0f);
		// Speed up collison
		mesh::TriangleOctree collideOcTree;
		mtrl::Material material;
	};

	//! A RAII wrapper for the PointLight object - for visualization
	struct RenderPointLightObj {

		RenderPointLightObj(light::PointLight* lightPtr) {
			light = lightPtr;
			shape = new math::sphere{ light->position, 0.4f };
		}

		~RenderPointLightObj() {
			delete light;
			delete shape;
		}

		bool hit(const math::ray& ray, math::HitEntry& nearest, ObjRef& objRef) {
			if (shape->hit(ray, nearest)) {
				objRef.object = this;
				objRef.material = nullptr;
				objRef.type = RenderType::POINTLIGHT;
				return true;
			}
			return false;
		}

		light::PointLight* light;
		math::sphere* shape;
	};

	//! A RAII wrapper for the PointLight object - for visualization
	struct RenderSpotLightObj {

		RenderSpotLightObj(light::SpotLight* lightPtr) {
			light = lightPtr;
			shape = new math::sphere{ light->position, 0.3f };
		}

		~RenderSpotLightObj() {
			delete light;
			delete shape;
		}

		bool hit(const math::ray& ray, math::HitEntry& nearest, ObjRef& objRef) {
			if (shape->hit(ray, nearest)) {
				objRef.object = this;
				objRef.material = nullptr;
				objRef.type = RenderType::SPOTLIGHT;
				return true;
			}
			return false;
		}

		light::SpotLight* light;
		math::sphere* shape;
	};


struct RenderMathObject {
	RenderMathObject(math::hitable* shapePtr, const mtrl::Material& mat) :
		material{ mat }
	{
		shape = shapePtr;
	}

	~RenderMathObject() {
		delete shape;
	}

	math::hitable* shape;
	mtrl::Material material;
};

} // engn