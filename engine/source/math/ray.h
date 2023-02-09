#pragma once

#include <glm/glm/glm.hpp>
#include <glm/glm/gtx/string_cast.hpp>

namespace engn {

	namespace math {

		class ray {
		public:
			ray() : origin{}, direction{} {}
			ray(const glm::vec3& o, const glm::vec3& d) : origin{ o }, direction{ d } {}

			//! Transform a ray by specified matrix
			void transform(const glm::mat4& transMat);

			//! Cast a ray in the direction by t
			glm::vec3 getPointAt(float t) const;

			glm::vec3 origin;
			glm::vec3 direction;
		};

	} // math

} // engn