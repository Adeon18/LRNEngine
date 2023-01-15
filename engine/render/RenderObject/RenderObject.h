#pragma once

#include "source/math/ray.h"
#include "source/math/geometry/plane.h"
#include "source/math/geometry/sphere.h"
#include "source/math/geometry/hitable.h"

#include "source/mesh/Mesh.h"
#include "source/mesh/TriangleOcTree.h"

#include "render/Lighting/Lights.h"
#include "render/Materials/Material.h"


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

		RenderSphereObj(math::sphere* shapePtr, const mtrl::Material& mat);

		RenderSphereObj(const RenderSphereObj& other);

		RenderSphereObj& operator=(const RenderSphereObj& other);

		~RenderSphereObj();
		//! Check for collision and fill the data
		bool hit(const math::ray& ray, math::HitEntry& nearest, ObjRef& objRef);

		void setPosition(const glm::vec3& newPos);
		void setMaterial(const mtrl::Material& mat);
		[[nodiscard]] glm::vec3& getPosition();
		[[nodiscard]] mtrl::Material& getMaterial();

	private:
		math::sphere* m_shape;
		mtrl::Material m_material;
	};

	//! A RAII wrapper for the plane object
	class RenderPlaneObj {
	public:
		RenderPlaneObj(math::plane* shapePtr, const mtrl::Material& mat);

		RenderPlaneObj(const RenderPlaneObj& other);

		RenderPlaneObj& operator=(const RenderPlaneObj& other);

		~RenderPlaneObj();
		//! Check for collision and fill the data
		bool hit(const math::ray& ray, math::HitEntry& nearest, ObjRef& objRef);

		void setMaterial(const mtrl::Material& mat);
		[[nodiscard]] mtrl::Material& getMaterial();
	private:
		math::plane* m_shape;
		mtrl::Material m_material;
	};

	//! A simple RAII wrapper for rendering and wrapping sped up intersection for a mesh object
	class RenderMeshObj {
	public:
		RenderMeshObj(mesh::Mesh* msh, const mtrl::Material& mat, const glm::vec3& mshPos);

		RenderMeshObj(const RenderMeshObj& other);

		RenderMeshObj& operator=(const RenderMeshObj& other);

		~RenderMeshObj() = default;

		//! Check for collision and fill the data
		bool hit(math::ray& ray, math::HitEntry& nearest, ObjRef& objRef);

		void setPosition(const glm::vec3& newPos);

		void setMaterial(const mtrl::Material& mat);
		[[nodiscard]] glm::vec3& getPosition();
		[[nodiscard]] mtrl::Material& getMaterial();
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
		void m_init();
		//! Update the model transformation matrix => should be done after every transform operation on an object
		void m_updateMatrices();
	};

	//! A RAII wrapper for the PointLight object - for visualization
	class RenderPointLightObj {
	public:
		RenderPointLightObj(light::PointLight* lightPtr);

		RenderPointLightObj(const RenderPointLightObj& other);

		RenderPointLightObj& operator=(const RenderPointLightObj& other);

		~RenderPointLightObj();
		//! Check for collision and fill the data
		bool hit(const math::ray& ray, math::HitEntry& nearest, ObjRef& objRef);

		[[nodiscard]] light::PointLight* getLight();

	private:
		light::PointLight* m_light;
		math::sphere* m_shape;
	};

	//! A RAII wrapper for the PointLight object - for visualization
	class RenderSpotLightObj {
	public:
		RenderSpotLightObj(light::SpotLight* lightPtr);

		RenderSpotLightObj(const RenderSpotLightObj& other);

		RenderSpotLightObj& operator=(const RenderSpotLightObj& other);

		~RenderSpotLightObj();
		//! Check for collision and fill the data
		bool hit(const math::ray& ray, math::HitEntry& nearest, ObjRef& objRef);

		[[nodiscard]] light::SpotLight* getLight();
	private:
		light::SpotLight* m_light;
		math::sphere* m_shape;
	};

} // engn