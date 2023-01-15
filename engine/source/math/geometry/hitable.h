#pragma once

#include <glm/glm/glm.hpp>

#include "source/math/ray.h"


namespace engn {

	namespace math {

		//! An abstract class for thr hitable object
		class hitable
		{
		public:
			static constexpr float MAX_DIST = 1000.0f;
		};

struct HitEntry
{
			HitEntry() {
				rayT = hitable::MAX_DIST;
			}

	float rayT;
	glm::vec3 hitPoint;
	glm::vec3 hitNormal;
};

	} // math

} // engn
