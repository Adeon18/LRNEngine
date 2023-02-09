#include "RenderObject.h"

namespace engn {
	//----------------//
	// RenderSphereObj//
	//----------------//
	RenderSphereObj::RenderSphereObj(math::sphere* shapePtr, const mtrl::Material& mat) : m_material{ mat } { m_shape = shapePtr; }
	RenderSphereObj::RenderSphereObj(const RenderSphereObj& other) : m_material{ other.m_material } { m_shape = new math::sphere{ *other.m_shape }; }
	
	RenderSphereObj& RenderSphereObj::operator=(const RenderSphereObj& other) {
		m_shape = new math::sphere{ *other.m_shape };
		m_material = mtrl::Material{ other.m_material };
		return *this;
	}
	
	RenderSphereObj::~RenderSphereObj() {
		if (m_shape) { delete m_shape; };
	}
	
	bool RenderSphereObj::hit(const math::ray& ray, math::HitEntry& nearest, ObjRef& objRef) {
		if (m_shape->hit(ray, nearest)) {
			objRef.object = this;
			objRef.material = &m_material;
			objRef.type = RenderType::SPHERE;
			return true;
		}
		return false;
	
	}
	void RenderSphereObj::setPosition(const glm::vec3& newPos) { m_shape->center = newPos; }
	void RenderSphereObj::setMaterial(const mtrl::Material& mat) { m_material = mat; }
	[[nodiscard]] glm::vec3& RenderSphereObj::getPosition() { return m_shape->center; }
	[[nodiscard]] mtrl::Material& RenderSphereObj::getMaterial() { return m_material; }
	
	//----------------//
	// RenderPlaneObj //
	//----------------//
	RenderPlaneObj::RenderPlaneObj(math::plane* shapePtr, const mtrl::Material& mat) : m_material{ mat } {
		m_shape = shapePtr;
	}

	RenderPlaneObj::RenderPlaneObj(const RenderPlaneObj& other) : m_material{ other.m_material } {
		m_shape = new math::plane{ *other.m_shape };
	}

	RenderPlaneObj& RenderPlaneObj::operator=(const RenderPlaneObj& other) {
		m_shape = new math::plane{ *other.m_shape };
		m_material = mtrl::Material{ other.m_material };
		return *this;
	}

	RenderPlaneObj::~RenderPlaneObj() {
		if (m_shape) { delete m_shape; };
	}

	bool RenderPlaneObj::hit(const math::ray& ray, math::HitEntry& nearest, ObjRef& objRef) {
		if (m_shape->hit(ray, nearest)) {
			objRef.object = this;
			objRef.material = &m_material;
			objRef.type = RenderType::PLANE;
			return true;
		}
		return false;
	}

	void RenderPlaneObj::setMaterial(const mtrl::Material& mat) { m_material = mat; }
	[[nodiscard]] mtrl::Material& RenderPlaneObj::getMaterial() { return m_material; }

	//----------------//
	// RenderMeshObj
	//----------------//
	RenderMeshObj::RenderMeshObj(mesh::Mesh* msh, const mtrl::Material& mat, const glm::vec3& mshPos) :
		m_material{ mat },
		m_position{ mshPos }
	{
		// Mesh data
		m_mesh = msh;
		m_init();
	}

	RenderMeshObj::RenderMeshObj(const RenderMeshObj& other) : m_material{ other.m_material }, m_position{ other.m_position } {
		m_mesh = other.m_mesh;
		m_init();
	}

	RenderMeshObj& RenderMeshObj::operator=(const RenderMeshObj& other) {
		m_position = other.m_position;
		m_material = other.m_material;
		m_mesh = other.m_mesh;
		m_init();
		return *this;
	}

	bool RenderMeshObj::hit(math::ray& ray, math::HitEntry& nearest, ObjRef& objRef) {
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

	void RenderMeshObj::setPosition(const glm::vec3& newPos) {
		m_position = newPos;
		m_updateMatrices();
	}

	void RenderMeshObj::setMaterial(const mtrl::Material& mat) { m_material = mat; }
	[[nodiscard]] glm::vec3& RenderMeshObj::getPosition() { return m_position; }
	[[nodiscard]] mtrl::Material& RenderMeshObj::getMaterial() { return m_material; }

	void RenderMeshObj::m_init() {
		m_collideOcTree.initialize(*m_mesh);
		// Precalculate matrices
		m_updateMatrices();
	}

	void RenderMeshObj::m_updateMatrices() {
		m_modelMatrix = glm::mat4(1.0f);
		m_modelMatrix = glm::translate(m_modelMatrix, m_position);
		m_modelMatrixInv = glm::inverse(m_modelMatrix);
	}

	//----------------//
	// RenderPointLigthObj
	//----------------//
	RenderPointLightObj::RenderPointLightObj(light::PointLight* lightPtr) {
		m_light = lightPtr;
		m_shape = new math::sphere{ m_light->position, 0.4f };
	}

	RenderPointLightObj::RenderPointLightObj(const RenderPointLightObj& other) {
		m_light = new light::PointLight{ *other.m_light };
		m_shape = new math::sphere{ m_light->position, 0.4f };
	}

	RenderPointLightObj& RenderPointLightObj::operator=(const RenderPointLightObj& other) {
		m_light = new light::PointLight{ *other.m_light };
		m_shape = new math::sphere{ m_light->position, 0.4f };
		return *this;
	}

	RenderPointLightObj::~RenderPointLightObj() {
		delete m_light;
		delete m_shape;
	}

	bool RenderPointLightObj::hit(const math::ray& ray, math::HitEntry& nearest, ObjRef& objRef) {
		if (m_shape->hit(ray, nearest)) {
			objRef.object = this;
			objRef.material = nullptr;
			objRef.type = RenderType::POINTLIGHT;
			return true;
		}
		return false;
	}

	[[nodiscard]] light::PointLight* RenderPointLightObj::getLight() { return m_light; }

	//----------------//
	// RenderSpotLightObj
	//----------------//
	RenderSpotLightObj::RenderSpotLightObj(light::SpotLight* lightPtr) {
		m_light = lightPtr;
		m_shape = new math::sphere{ m_light->position, 0.3f };
	}

	RenderSpotLightObj::RenderSpotLightObj(const RenderSpotLightObj& other) {
		m_light = new light::SpotLight{ *other.m_light };
		m_shape = new math::sphere{ m_light->position, 0.4f };
	}

	RenderSpotLightObj& RenderSpotLightObj::operator=(const RenderSpotLightObj& other) {
		m_light = new light::SpotLight{ *other.m_light };
		m_shape = new math::sphere{ m_light->position, 0.4f };
		return *this;
	}

	RenderSpotLightObj::~RenderSpotLightObj() {
		delete m_light;
		delete m_shape;
	}

	bool RenderSpotLightObj::hit(const math::ray& ray, math::HitEntry& nearest, ObjRef& objRef) {
		if (m_shape->hit(ray, nearest)) {
			objRef.object = this;
			objRef.material = nullptr;
			objRef.type = RenderType::SPOTLIGHT;
			return true;
		}
		return false;
	}

	[[nodiscard]] light::SpotLight* RenderSpotLightObj::getLight() { return m_light; }
} // engn