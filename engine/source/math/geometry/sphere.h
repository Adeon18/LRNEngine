#pragma once

#include <utility>
#include <glm/glm.hpp>

#include "hitable.h"

#include "../ray.h"

namespace engn {

namespace math {

class sphere : public hitable {
	static constexpr float MAX_DIST = 1000.0f;
public:
	sphere(const glm::vec3& center, float r) : center{ center }, radius{ r }, radiusSq{ r * r } {}

	//! Hit function which determines if a ray hit a sphere
	[[nodiscard]] HitEntry hit(const ray& r) const override {
		HitEntry collisionRes;

		const glm::vec3 to_r = r.origin - center;

		const float a = glm::dot(r.direction, r.direction);
		const float b = 2.0f * glm::dot(to_r, r.direction);
		const float c = glm::dot(to_r, to_r) - radiusSq;

		const float discriminant = b * b - 4.0f * a * c;

		if (discriminant >= 0.0f)
		{
			// Code duplication but is more optimized than iteration because result computing is quite expensive because sqrt
			float res = (-b - glm::sqrt(discriminant)) / (2.0f * a);
			if (res > 0.0f && res < MAX_DIST) {
				collisionRes.isHit = true;
				collisionRes.rayT = res;
				collisionRes.hitPoint = r.getPointAt(collisionRes.rayT);
				collisionRes.hitNormal = glm::normalize(collisionRes.hitPoint - center);
				return collisionRes;
			}
			res = (-b + glm::sqrt(discriminant)) / (2.0f * a);
			if (res > 0.0f && res < MAX_DIST) {
				collisionRes.isHit = true;
				collisionRes.rayT = res;
				collisionRes.hitPoint = r.getPointAt(collisionRes.rayT);
				collisionRes.hitNormal = glm::normalize(collisionRes.hitPoint - center);
				return collisionRes;
			}
		}
		collisionRes.isHit = false;
		collisionRes.rayT = MAX_DIST;

		return collisionRes;
	}
public:
	glm::vec3 center;
	float radius;
	float radiusSq;
};

} // math

} // engn