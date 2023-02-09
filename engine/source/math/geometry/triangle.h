#pragma once


#include <glm/glm/glm.hpp>

#include "source/math/geometry/hitable.h"

namespace engn {

	namespace math {

#define TRIANGLE_CULL 1

		class triangle : public hitable {
			static constexpr float EPS = 0.00001f;
		public:
			triangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2);
			triangle(const triangle& other) = default;
			triangle& operator=(const triangle& other) = default;
			~triangle() = default;

			// Algorith taken from https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
			[[nodiscard]] bool hit(const ray& r, HitEntry& closestHit) const;
		public:
			glm::vec3 vertices[3];
			glm::vec3 edges[2];
			glm::vec3 normal;
			glm::vec3 center;
		};

	} // math

} // engn