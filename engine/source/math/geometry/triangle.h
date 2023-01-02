#pragma once


#include "glm/glm.hpp"

#include "source/math/geometry/hitable.h"


namespace engn {

namespace math {

	class triangle : public hitable {
		static constexpr float EPS = 0.00001f;
		static constexpr float MAX_DIST = 1000.0f;
	public:
		triangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {
			vertices[0] = v0;
			vertices[1] = v1;
			vertices[2] = v2;
			edges[0] = vertices[1] - vertices[0];
			edges[1] = vertices[2] - vertices[0];
			normal = glm::normalize(glm::cross(v0 - v1, v0 - v2));
		}

		triangle& operator=(const triangle& other) {
			vertices[0] = other.vertices[0];
			vertices[1] = other.vertices[1];
			vertices[2] = other.vertices[2];

			normal = other.normal;
			return *this;
		}

		[[nodiscard]] HitEntry hit(const ray& r) const override {
			HitEntry collisionRes{};
			collisionRes.isHit = false;
			collisionRes.rayT = MAX_DIST;

			glm::vec3 pv = glm::cross(r.direction, edges[1]);

			float det = glm::dot(edges[0], pv);

			// If ray is paralell to the triangle
			if (det < EPS && det > -EPS) {
				return collisionRes;
			}

			float invDet = 1.0f / det;

			glm::vec3 toRayOrigin = r.origin - vertices[0];
			
			float uParam = glm::dot(toRayOrigin, pv) * invDet;
			if (uParam < 0.0f || uParam > 1.0f) {
				return collisionRes;
			}

			glm::vec3 vParamTestVec = glm::cross(toRayOrigin, edges[0]);

			float vParam = glm::dot(r.direction, vParamTestVec) * invDet;
			if (vParam < 0.0f || uParam + vParam > 1.0f) {
				return collisionRes;
			}

			float t = glm::dot(edges[1], vParamTestVec) * invDet;

			collisionRes.isHit = true;
			collisionRes.rayT = t;
			collisionRes.hitPoint = r.getPointAt(t);
			collisionRes.hitNormal = (glm::dot(toRayOrigin, normal) > 0.0f) ? normal: -normal;

			return collisionRes;
		}
	public:
		glm::vec3 vertices[3];
		glm::vec3 edges[2];
		glm::vec3 normal;
	};

} // math

} // engn