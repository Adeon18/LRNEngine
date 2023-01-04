#pragma once

#include <utility>
#include <glm/glm.hpp>

#include "hitable.h"
#include "../ray.h"


namespace engn {

namespace math {

class plane : public hitable {
	static constexpr float MIN_ANGLE_DOT = 1e-6;
public:
	plane(const glm::vec3& normal, const glm::vec3& point) : m_normal{ glm::normalize(normal) }, m_point{ point } {}
	plane(const plane& other) = default;
	plane& operator=(const plane & other) = default;
	~plane() = default;

	//! Hit function which determines if a ray hit a plane
	[[nodiscard]] bool hit(const ray& r, HitEntry& closestHit) const {
		float res = glm::dot(r.direction, m_normal);

		if (res > MIN_ANGLE_DOT || res < -MIN_ANGLE_DOT)
		{
			float dist = glm::dot(m_point - r.origin, m_normal) / res;

			if (dist > 0.0f && dist < closestHit.rayT)
			{
				closestHit.rayT = dist;
				closestHit.hitPoint = r.getPointAt(dist);
				// I don't have any clue how this normal works but it does and I'm happy
				closestHit.hitNormal = glm::normalize(m_normal * -res);
				return true;
			}
		}

		return false;
	}
private:
	glm::vec3 m_normal;
	glm::vec3 m_point;
};

} // math

} // engn