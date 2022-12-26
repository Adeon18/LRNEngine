#pragma once

#include <utility>
#include <glm/glm.hpp>

#include "hitable.h"
#include "../ray.h"


namespace engn {

namespace math {

class plane : public hitable {
	const int MAX_DIST = 1000;
	const float MIN_ANGLE_DOT = 1e-6;
public:
	plane(const glm::vec3& normal, const glm::vec3& point) : m_normal{ normal }, m_point{ point } {}

	//! Hit function which determines if a ray hit a plane
	[[nodiscard]] HitEntry hit(const ray& r) const override {
		HitEntry collisionRes{};

		float res = glm::dot(m_normal, r.getDirection());
		// TODO: Replace constant
		if (res > MIN_ANGLE_DOT)
		{
			auto dist = glm::dot(m_point - r.getOrigin(), m_normal) / res;
			if (dist < MAX_DIST)
			{
				collisionRes.isHit = true;
				collisionRes.rayT = dist;
				collisionRes.hitPoint = r.cast(dist);
				collisionRes.hitNormal = -m_normal;
			}
			else {
				collisionRes.isHit = false;
				collisionRes.rayT = MAX_DIST;
			}
		}
		else {
			collisionRes.isHit = false;
			collisionRes.rayT = MAX_DIST;
		}

		return collisionRes;
	}
private:
	glm::vec3 m_normal;
	glm::vec3 m_point;
};

} // math

} // engn