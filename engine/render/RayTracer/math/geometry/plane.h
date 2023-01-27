#pragma once

#include <utility>
#include <glm/glm/glm.hpp>

#include "hitable.h"
#include "render/RayTracer/math/ray.h"


namespace engn {

	namespace math {

		class plane : public hitable {
			static constexpr float MIN_ANGLE_DOT = 1e-6f;
		public:
			plane(const glm::vec3& normal, const glm::vec3& point) : m_normal{ glm::normalize(normal) }, m_point{ point } {}
			plane(const plane& other) = default;
			plane& operator=(const plane& other) = default;
			~plane() = default;

			//! Hit function which determines if a ray hit a plane
			[[nodiscard]] bool hit(const ray& r, HitEntry& closestHit) const;
		private:
			glm::vec3 m_normal;
			glm::vec3 m_point;
		};

	} // math

} // engn