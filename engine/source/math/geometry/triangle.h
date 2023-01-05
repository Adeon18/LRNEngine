#pragma once


#include <glm/glm/glm.hpp>

#include "source/math/geometry/hitable.h"

namespace engn {

	namespace math {

#define TRIANGLE_CULL 1

		class triangle : public hitable {
			static constexpr float EPS = 0.00001f;
		public:
			triangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {
				vertices[0] = v0;
				vertices[1] = v1;
				vertices[2] = v2;
				edges[0] = vertices[1] - vertices[0];
				edges[1] = vertices[2] - vertices[0];
				normal = glm::normalize(glm::cross(edges[0], edges[1]));
			}
			triangle(const triangle& other) = default;
			triangle& operator=(const triangle& other) = default;
			~triangle() = default;

			// Algorith taken from https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
			[[nodiscard]] bool hit(const ray& r, HitEntry& closestHit) const {

				glm::vec3 pv = glm::cross(r.direction, edges[1]);

				float det = glm::dot(edges[0], pv);

#ifdef TRIANGLE_CULL
				// If ray is paralell to the triangle
				if (det < EPS) {
					return false;
				}

				float invDet = 1.0f / det;

				glm::vec3 toRayOrigin = r.origin - vertices[0];

				float uParam = glm::dot(toRayOrigin, pv);
				if (uParam < 0.0f || uParam > det) {
					return false;
				}

				glm::vec3 vParamTestVec = glm::cross(toRayOrigin, edges[0]);

				float vParam = glm::dot(r.direction, vParamTestVec);
				if (vParam < 0.0f || uParam + vParam > det) {
					return false;
				}

				float t = glm::dot(edges[1], vParamTestVec) * invDet;
#else
				// If ray is paralell to the triangle
				if (det < EPS && det > -EPS) {
					return false;
				}

				float invDet = 1.0f / det;

				glm::vec3 toRayOrigin = r.origin - vertices[0];

				float uParam = glm::dot(toRayOrigin, pv) * invDet;
				if (uParam < 0.0f || uParam > 1.0f) {
					return false;
				}

				glm::vec3 vParamTestVec = glm::cross(toRayOrigin, edges[0]);

				float vParam = glm::dot(r.direction, vParamTestVec) * invDet;
				if (vParam < 0.0f || uParam + vParam > 1.0f) {
					return false;
				}

				float t = glm::dot(edges[1], vParamTestVec) * invDet;

#endif
				if (t <= 0.0f || t >= closestHit.rayT) { return false; }

				closestHit.rayT = t;
				closestHit.hitPoint = r.getPointAt(t);
				closestHit.hitNormal = (glm::dot(toRayOrigin, normal) > 0.0f) ? normal : -normal;

				return true;
			}
		public:
			glm::vec3 vertices[3];
			glm::vec3 edges[2];
			glm::vec3 normal;
		};

	} // math

} // engn