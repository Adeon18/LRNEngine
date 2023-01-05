#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

namespace engn {

	namespace math {

		class ray {
		public:
			ray() : origin{}, direction{} {}
			ray(const glm::vec3& o, const glm::vec3& d) : origin{ o }, direction{ d } {}

			//! Transform a ray by specified matrix
			void transform(const glm::mat4& transMat) {
				glm::vec4 oVec4 = glm::vec4(origin, 1.0f);

				oVec4 = transMat * oVec4;

				origin = glm::vec3(oVec4);
			}

			//! Cast a ray in the direction by t
			glm::vec3 getPointAt(float t) const { return origin + t * direction; }

			glm::vec3 origin;
			glm::vec3 direction;
		};

	} // math

} // eng