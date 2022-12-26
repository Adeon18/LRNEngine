#pragma once

#include <glm/glm.hpp>


namespace engn {

struct Material {
	Material(const glm::vec3& amb, const glm::vec3& diff, const glm::vec3& spec, float shine) :
		ambient{ amb },
		diffuse{ diff },
		specular{ spec },
		shininess{ shine }
	{}

	// TODO: Add color constructor possibility

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

} // engn