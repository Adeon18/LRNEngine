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
	//! Specifies oject type
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

	//! Reference to the hit object
	struct ObjRef {
		void* object;
		mtrl::Material* material;
		RenderType type = RenderType::NONE;

		void clear() {
			object = nullptr;
			material = nullptr;
			type = RenderType::NONE;
		}
	};

	//! A RAII wrapper foe the math sphere class
	struct RenderSphereObj {

		RenderSphereObj(math::sphere* shapePtr, const mtrl::Material& mat) : m_material{ mat } {
			m_shape = shapePtr;
		}

		RenderSphereObj(const RenderSphereObj& other) : m_material{ other.m_material } {
			m_shape = new math::sphere{ *other.m_shape };
		}

		RenderSphereObj& operator=(const RenderSphereObj& other) {
			m_shape = new math::sphere{ *other.m_shape };
			m_material = mtrl::Material{ other.m_material };
			return *this;
		}

		~RenderSphereObj() {
			if (m_shape) { delete m_shape; };
		}

		bool hit(const math::ray& ray, math::HitEntry& nearest, ObjRef& objRef) {
			if (m_shape->hit(ray, nearest)) {
				objRef.object = this;
				objRef.material = &m_material;
				objRef.type = RenderType::SPHERE;
				return true;
			}
			return false;
		}

		void setPosition(const glm::vec3& newPos) { m_shape->center = newPos; }
		void setMaterial(const mtrl::Material& mat) { m_material = mat; }
		[[nodiscard]] glm::vec3& getPosition() { return m_shape->center; }
		[[nodiscard]] mtrl::Material& getMaterial() { return m_material; }

	private:
		math::sphere* m_shape;
		mtrl::Material m_material;
	};

	//! A RAII wrapper for the plane object
	class RenderPlaneObj {
	public:
		RenderPlaneObj(math::plane* shapePtr, const mtrl::Material& mat) : m_material{ mat } {
			m_shape = shapePtr;
		}

		RenderPlaneObj(const RenderPlaneObj& other) : m_material{ other.m_material } {
			m_shape = new math::plane{ *other.m_shape };
		}

		RenderPlaneObj& operator=(const RenderPlaneObj& other) {
			m_shape = new math::plane{ *other.m_shape };
			m_material = mtrl::Material{ other.m_material };
			return *this;
		}

		~RenderPlaneObj() {
			if (m_shape) { delete m_shape; };
		}

		bool hit(const math::ray& ray, math::HitEntry& nearest, ObjRef& objRef) {
			if (m_shape->hit(ray, nearest)) {
				objRef.object = this;
				objRef.material = &m_material;
				objRef.type = RenderType::PLANE;
				return true;
			}
			return false;
		}

		void setMaterial(const mtrl::Material& mat) { m_material = mat; }
		[[nodiscard]] mtrl::Material& getMaterial() { return m_material; }
	private:
		math::plane* m_shape;
		mtrl::Material m_material;
	};

	//! A simple RAII wrapper for rendering and wrapping sped up intersection for a mesh object
	class RenderMeshObj {
	public:
		RenderMeshObj(mesh::Mesh* msh, const mtrl::Material& mat, const glm::vec3& mshPos) :
			m_material{ mat },
			m_position{ mshPos }
		{
			// Mesh data
			m_mesh = msh;
			m_init();
		}

		RenderMeshObj(const RenderMeshObj& other): m_material{ other.m_material }, m_position{ other.m_position } {
			m_mesh = other.m_mesh;
			m_init();
		}

		RenderMeshObj& operator=(const RenderMeshObj& other) {
			m_position = other.m_position;
			m_material = other.m_material;
			m_mesh = other.m_mesh;
			m_init();
		}

		~RenderMeshObj() = default;

		bool hit(math::ray& ray, math::HitEntry& nearest, ObjRef& objRef) {
			auto prevRayOrigin = ray.origin;
			ray.transform(m_modelMatrixInv);

			if (m_collideOcTree.intersect(ray, nearest)) {
				// Yes
				nearest.hitPoint = m_modelMatrix * glm::vec4(nearest.hitPoint, 1.0f);
				
				objRef.object = this;
				objRef.material = &m_material;
				objRef.type = RenderType::MESH;

				ray.origin = prevRayOrigin;

				return true;
			}
			ray.origin = prevRayOrigin;

			return false;
		}

		void setPosition(const glm::vec3& newPos) {
			m_position = newPos;
			m_updateMatrices();
		}

		void setMaterial(const mtrl::Material& mat) { m_material = mat; }
		[[nodiscard]] glm::vec3& getPosition() { return m_position; }
		[[nodiscard]] mtrl::Material& getMaterial() { return m_material; }
	private:
		mesh::Mesh* m_mesh;

		glm::vec3 m_position;
		glm::mat4 m_modelMatrix = glm::mat4(1.0f);
		glm::mat4 m_modelMatrixInv = glm::mat4(1.0f);
		// Speed up collison
		mesh::TriangleOctree m_collideOcTree;
		mtrl::Material m_material;
	private:
		//! Initialize the octree and transformation matrices
		void m_init() {
			m_collideOcTree.initialize(*m_mesh);
			// Precalculate matrices
			m_updateMatrices();
		}

		void m_updateMatrices() {
			m_modelMatrix = glm::mat4(1.0f);
			m_modelMatrix = glm::translate(m_modelMatrix, m_position);
			m_modelMatrixInv = glm::inverse(m_modelMatrix);
		}
	};

	//! A RAII wrapper for the PointLight object - for visualization
	class RenderPointLightObj {
	public:
		RenderPointLightObj(light::PointLight* lightPtr) {
			m_light = lightPtr;
			m_shape = new math::sphere{ m_light->position, 0.4f };
		}

		RenderPointLightObj(const RenderPointLightObj& other) {
			m_light = new light::PointLight{ *other.m_light };
			m_shape = new math::sphere{ m_light->position, 0.4f };
		}

		RenderPointLightObj& operator=(const RenderPointLightObj& other) {
			m_light = new light::PointLight{ *other.m_light };
			m_shape = new math::sphere{ m_light->position, 0.4f };
			return *this;
		}

		~RenderPointLightObj() {
			delete m_light;
			delete m_shape;
		}

		bool hit(const math::ray& ray, math::HitEntry& nearest, ObjRef& objRef) {
			if (m_shape->hit(ray, nearest)) {
				objRef.object = this;
				objRef.material = nullptr;
				objRef.type = RenderType::POINTLIGHT;
				return true;
			}
			return false;
		}

		[[nodiscard]] light::PointLight* getLight() { return m_light; }

	private:
		light::PointLight* m_light;
		math::sphere* m_shape;
	};

	//! A RAII wrapper for the PointLight object - for visualization
	class RenderSpotLightObj {
	public:
		RenderSpotLightObj(light::SpotLight* lightPtr) {
			m_light = lightPtr;
			m_shape = new math::sphere{ m_light->position, 0.3f };
		}

		RenderSpotLightObj(const RenderSpotLightObj& other) {
			m_light = new light::SpotLight{ *other.m_light };
			m_shape = new math::sphere{ m_light->position, 0.4f };
		}

		RenderSpotLightObj& operator=(const RenderSpotLightObj& other) {
			m_light = new light::SpotLight{ *other.m_light };
			m_shape = new math::sphere{ m_light->position, 0.4f };
			return *this;
		}

		~RenderSpotLightObj() {
			delete m_light;
			delete m_shape;
		}

		bool hit(const math::ray& ray, math::HitEntry& nearest, ObjRef& objRef) {
			if (m_shape->hit(ray, nearest)) {
				objRef.object = this;
				objRef.material = nullptr;
				objRef.type = RenderType::SPOTLIGHT;
				return true;
			}
			return false;
		}

		[[nodiscard]] light::SpotLight* getLight() { return m_light; }
	private:
		light::SpotLight* m_light;
		math::sphere* m_shape;
	};

} // engn