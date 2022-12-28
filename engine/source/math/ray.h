#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

namespace engn {

namespace math {

class ray {
public:
	ray() : m_A{}, m_B{} {}
	ray(const glm::vec3& origin, const glm::vec3& direction) : m_A{ origin }, m_B{ direction } {}

	glm::vec3 getOrigin() const { return m_A; }
	glm::vec3 getDirection() const { return m_B; }

	void setOrigin(const glm::vec3& o) { m_A = o; }
	void setDirection(const glm::vec3& d) { m_B = d; }

	void transform(const glm::mat4& transMat) {
		glm::vec4 oVec4 = glm::vec4(m_A, 1.0f);

		oVec4 = transMat * oVec4;

		m_A = glm::vec3(oVec4);
	}

	glm::vec3 cast(float t) const { return m_A + t * m_B; }
private:
	glm::vec3 m_A;
	glm::vec3 m_B;
};

} // math

} // eng