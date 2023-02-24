#include "plane.h"

namespace engn {

	namespace math {
		[[nodiscard]] bool plane::hit(const ray& r, HitEntry& closestHit) const {
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
	} // math

} // engn