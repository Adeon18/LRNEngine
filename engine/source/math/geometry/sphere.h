#pragma once

#include <utility>
#include <glm/glm.hpp>

#include "hitable.h"

#include "../ray.h"

class sphere: public hitable{
public:
	sphere(const glm::vec3& center, float r) : m_center{ center }, m_radius{r} {}

	//! Hit function which determines if a ray hit a sphere
	[[nodiscard]] HitEntry hit(const ray& r) const override {
		HitEntry collisionRes{};

		const glm::vec3 to_r = r.getOrigin() - m_center;

		const float a = glm::dot(r.getDirection(), r.getDirection());
		const float b = 2.f * glm::dot(to_r, r.getDirection());
		const float c = glm::dot(to_r, to_r) - m_radius * m_radius;

		const float discriminant = b * b - 4 * a * c;

		if (discriminant >= 0)
		{
			collisionRes.isHit = true;
			collisionRes.hitPoint = r.cast((-b - glm::sqrt(discriminant)) / 2.0f * a);
			collisionRes.hitNormal = glm::normalize(collisionRes.hitPoint - m_center);
		} else
		{
			collisionRes.isHit = false;
		}

		return collisionRes;
	}
	glm::vec3 m_center;
	float m_radius;
};