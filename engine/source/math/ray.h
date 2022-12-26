#pragma once

#include <glm/glm.hpp>

namespace engn {

namespace math {

class ray {
public:
	ray() {}
	ray(const glm::vec3& origin, const glm::vec3& direction) : m_A{ origin }, m_B{ direction } {}

	glm::vec3 getOrigin() const { return m_A; }
	glm::vec3 getDirection() const { return m_B; }

	glm::vec3 cast(float t) const { return m_A + t * m_B; }
private:
	glm::vec3 m_A;
	glm::vec3 m_B;
};

} // math

} // engn