#include "sphere.h"

namespace engn {

	namespace math {
		//! Hit function which determines if a ray hit a sphere
		[[nodiscard]] bool sphere::hit(const ray& r, HitEntry& closestHit) const {
			const glm::vec3 to_r = r.origin - center;

			const float a = glm::dot(r.direction, r.direction);
			const float b = 2.0f * glm::dot(to_r, r.direction);
			const float c = glm::dot(to_r, to_r) - radiusSq;

			const float discriminant = b * b - 4.0f * a * c;

			if (discriminant >= 0.0f)
			{
				// Code duplication but is more optimized than iteration because result computing is quite expensive because sqrt
				float res = (-b - glm::sqrt(discriminant)) / (2.0f * a);
				if (res > 0.0f && res < closestHit.rayT) {
					closestHit.rayT = res;
					closestHit.hitPoint = r.getPointAt(closestHit.rayT);
					closestHit.hitNormal = glm::normalize(closestHit.hitPoint - center);
					return true;
				}
				res = (-b + glm::sqrt(discriminant)) / (2.0f * a);
				if (res > 0.0f && res < closestHit.rayT) {
					closestHit.rayT = res;
					closestHit.hitPoint = r.getPointAt(closestHit.rayT);
					closestHit.hitNormal = glm::normalize(closestHit.hitPoint - center);
					return true;
				}
			}

			return false;
		}
	} // math
} // engn