#include "ray.h"

namespace engn {

	namespace math {

		void ray::transform(const glm::mat4& transMat) {
			glm::vec4 oVec4 = glm::vec4(origin, 1.0f);

			oVec4 = transMat * oVec4;

			origin = glm::vec3(oVec4);
		}

		glm::vec3 ray::getPointAt(float t) const { return origin + t * direction; }

	} // math

} // engn