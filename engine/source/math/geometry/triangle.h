#pragma once


#include "glm/glm.hpp"

#include "source/math/geometry/hitable.h"


namespace engn {

namespace math {

	class triangle : public hitable {
		const float EPS = 0.00001;
	public:
		triangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {
			vertices[0] = v0;
			vertices[1] = v1;
			vertices[2] = v2;
			normal = glm::normalize(glm::cross(v0 - v1, v0 - v2));
		}

		triangle& operator=(const triangle& other) {
			vertices[0] = other.vertices[0];
			vertices[1] = other.vertices[1];
			vertices[2] = other.vertices[2];

			return *this;
		}

		[[nodiscard]] HitEntry hit(const ray& r) const override {
			HitEntry collisionRes{};
			collisionRes.isHit = false;
			collisionRes.rayT = 20000.0f;

			glm::vec3 e1 = vertices[1] - vertices[0];
			glm::vec3 e2 = vertices[2] - vertices[0];

			glm::vec3 pv = glm::cross(r.getDirection(), e2);

			float det = glm::dot(e1, pv);

			// If ray is paralell to the triangle
			if (det < EPS && det > -EPS) {
				return collisionRes;
			}

			float invDet = 1.0f / det;

			glm::vec3 toRayOrigin = r.getOrigin() - vertices[0];
			
			float uParam = glm::dot(toRayOrigin, pv) * invDet;
			if (uParam < 0.0f || uParam > 1.0f) {
				return collisionRes;
			}

			glm::vec3 vParamTestVec = glm::cross(toRayOrigin, e1);

			float vParam = glm::dot(r.getDirection(), vParamTestVec) * invDet;
			if (vParam < 0.0f || uParam + vParam > 1.0f) {
				return collisionRes;
			}

			float t = glm::dot(e2, vParamTestVec) * invDet;

			collisionRes.isHit = true;
			collisionRes.rayT = t;
			collisionRes.hitPoint = r.cast(t);
			collisionRes.hitNormal = (glm::dot(toRayOrigin, normal) > 0.0f) ? normal: -normal;

			return collisionRes;
		}

		glm::vec3 vertices[3];
		glm::vec3 normal;
	};

} // math

} // engn