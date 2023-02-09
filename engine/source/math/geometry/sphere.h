#pragma once

#include <utility>
#include <glm/glm/glm.hpp>

#include "hitable.h"

#include "source/math/ray.h"

namespace engn {

	namespace math {

		class sphere : public hitable {
		public:
			sphere(const glm::vec3& center, float r) : center{ center }, radius{ r }, radiusSq{ r * r } {}
			sphere(const sphere& other) = default;
			sphere& operator=(const sphere& other) = default;
			~sphere() = default;

			//! Hit function which determines if a ray hit a sphere
			[[nodiscard]] bool hit(const ray& r, HitEntry& closestHit) const;
		public:
			glm::vec3 center;
			float radius;
			float radiusSq;
		};

	} // math

} // engn