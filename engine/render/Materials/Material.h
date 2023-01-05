#pragma once

#include <glm/glm/glm.hpp>


namespace engn {
	namespace mtrl {
		
		//! A wrapper class that contains Material properties and gets binded to a RenderObject
		struct Material {
			Material(const glm::vec3& amb, const glm::vec3& diff, const glm::vec3& spec, float shine) :
				ambient{ amb },
				diffuse{ diff },
				specular{ spec },
				shininess{ shine }
			{}

			Material(const Material& other) = default;
			Material& operator=(const Material& other) = default;
			~Material() = default;

			glm::vec3 ambient;
			glm::vec3 diffuse;
			glm::vec3 specular;
			float shininess;
		};

		//! Constant predefined materials
		// Metals
		const Material EMERALD{ glm::vec3{0.0215f, 0.1745f, 0.0215f}, glm::vec3{0.07568f, 0.61424f, 0.07568f}, glm::vec3{0.633f, 0.727811f, 0.633f}, 0.6f * 128 };
		const Material CHROME{ glm::vec3{0.25f, 0.25f, 0.25f}, glm::vec3{0.4f, 0.4f, 0.4f}, glm::vec3{0.633f, 0.727811f, 0.633f}, 0.6f * 128 };
		const Material GOLD{ glm::vec3{0.24725f, 0.1995f, 0.0745f}, glm::vec3{0.75164f, 0.60648f, 0.22648f}, glm::vec3{0.628281f, 0.555802f, 0.366065f}, 0.4f * 128 };
		const Material SILVER{ glm::vec3{0.19225f, 0.19225f, 0.19225f}, glm::vec3{0.50754f, 0.50754f, 0.50754f}, glm::vec3{0.508273f, 0.508273f, 0.508273f}, 0.4f * 128 };
		// Plastic
		const Material WHITE_PLASTIC{ glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.55f, 0.55f, 0.55f}, glm::vec3{0.7f, 0.7f, 0.7f}, 0.25f * 128 };
		const Material GREEN_PLASTIC{ glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.1f, 0.35f, 0.1f}, glm::vec3{0.45f, 0.55f, 0.45f}, 0.25f * 128 };
		const Material RED_PLASTIC{ glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.5f, 0.0f, 0.0f}, glm::vec3{0.7f, 0.6f, 0.6f}, 0.25f * 128 };
		const Material YELLOW_PLASTIC{ glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.5f, 0.5f, 0.0f}, glm::vec3{0.6f, 0.6f, 0.5f}, 0.25f * 128 };
		// Rubber
		const Material WHITE_RUBBER{ glm::vec3{0.05f, 0.05f, 0.05f}, glm::vec3{0.5f, 0.5f, 0.5f}, glm::vec3{0.7f, 0.7f, 0.7f}, 0.078125f * 128 };
		const Material YELLOW_RUBBER{ glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.5f, 0.5f, 0.0f}, glm::vec3{0.6f, 0.6f, 0.5f}, 0.078125f * 128 };
		const Material RED_RUBBER{ glm::vec3{0.05f, 0.0f, 0.0f}, glm::vec3{0.5f, 0.4f, 0.4f}, glm::vec3{0.7f, 0.04f, 0.04f}, 0.078125f * 128 };
		const Material GREEN_RUBBER{ glm::vec3{0.0f, 0.05f, 0.0f}, glm::vec3{0.4f, 0.5f, 0.4f}, glm::vec3{0.04f, 0.7f, 0.04f}, 0.078125f * 128 };
	}
} // engn