#pragma once

#include <utility>
#include <glm/glm.hpp>

#include "hitable.h"

#include "../ray.h"

namespace engn {

namespace math {

class sphere : public hitable {
public:
	sphere(const glm::vec3& center, float r) : m_center{ center }, m_radius{ r } {}

	//! Hit function which determines if a ray hit a sphere
	[[nodiscard]] HitEntry hit(const ray& r) const override {
		HitEntry collisionRes{};

		const glm::vec3 to_r = r.origin - m_center;

		const float a = glm::dot(r.direction, r.direction);
		const float b = 2.f * glm::dot(to_r, r.direction);
		const float c = glm::dot(to_r, to_r) - m_radius * m_radius;

		const float discriminant = b * b - 4 * a * c;

		if (discriminant >= 0)
		{
			// Code duplication but is more optimized than iteration because result computing is quite expensive because sqrt
			float res = (-b - glm::sqrt(discriminant)) / (2.0f * a);
			if (res > 0) {
				collisionRes.isHit = true;
				collisionRes.rayT = res;
				collisionRes.hitPoint = r.cast(collisionRes.rayT);
				collisionRes.hitNormal = glm::normalize(collisionRes.hitPoint - m_center);
				return collisionRes;
			}
			res = (-b + glm::sqrt(discriminant)) / (2.0f * a);
			if (res > 0) {
				collisionRes.isHit = true;
				collisionRes.rayT = res;
				collisionRes.hitPoint = r.cast(collisionRes.rayT);
				collisionRes.hitNormal = glm::normalize(collisionRes.hitPoint - m_center);
				return collisionRes;
			}
		}
		collisionRes.isHit = false;
		collisionRes.rayT = 20000.0f;

		return collisionRes;
	}
public:
	glm::vec3 m_center;
	float m_radius;
};

} // math

} // engn