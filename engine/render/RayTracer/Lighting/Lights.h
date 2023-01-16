#pragma once

#include <cmath>
#include <utility>

#include <glm/glm/glm.hpp>

#include "render/RayTracer/Materials/Material.h"


namespace engn {

namespace light {
	constexpr bool MODEL_BLING_PHONG = true;

	//! Basic constant attenuation configs for
	static constexpr glm::vec3 PLIGHT_DIST_7{ 1.0f, 0.7f, 1.8f };
	static constexpr glm::vec3 PLIGHT_DIST_20{ 1.0f, 0.22f, 0.2f };
	static constexpr glm::vec3 PLIGHT_DIST_50{ 1.0f, 0.09, 0.032f };
	static constexpr glm::vec3 PLIGHT_DIST_65{ 1.0f, 0.07f, 0.017f };
	static constexpr glm::vec3 PLIGHT_DIST_100{ 1.0f, 0.045f, 0.0075f };

	//! colors
	static constexpr glm::vec3 RED{ 1.0f, 0.0f, 0.0f };
	static constexpr glm::vec3 GREEN{ 0.0f, 1.0f, 0.0f };
	static constexpr glm::vec3 BLUE{ 0.0f, 0.0f, 1.0f };
	static constexpr glm::vec3 LIGHTPURPLE{ 0.5f, 0.2f, 0.9f };
	static constexpr glm::vec3 CYAN{0.0f, 1.0f, 1.0f};
	static constexpr glm::vec3 SKY_COLOR = glm::vec3(0.05f, 0.05f, 0.2f);

	//! Basic Light data that is needed for each light type
	struct LightProperties {
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};

	//! Directional Light that only depends on it's direction
	struct DirectionalLight {
		DirectionalLight(const glm::vec3& dir, const LightProperties& prop) :
			direction{ dir },
			properties{ prop }
		{}

		DirectionalLight(const glm::vec3& dir, const LightProperties& prop, const glm::vec3& color);

		glm::vec3 direction;

		LightProperties properties;
	};

	//! Basic pointlight that spits light in all directions
	struct PointLight {
		PointLight(const glm::vec3& pos, const LightProperties& prop, const glm::vec3& attenuation);

		PointLight(const glm::vec3& pos, const LightProperties& prop, const glm::vec3& attenuation, const glm::vec3& color);

		glm::vec3 position;

		LightProperties properties;

		float constant;
		float linear;
		float quadratic;
	};

	//! Light type that works like a flashlight and has a cutoff angle
	struct SpotLight {
		SpotLight(const glm::vec3& dir, const glm::vec3& pos, const glm::vec2& range, const LightProperties& prop);

		SpotLight(const glm::vec3& dir, const glm::vec3& pos, const glm::vec2& range, const LightProperties& prop, const glm::vec3& color);

		glm::vec3 position;
		glm::vec3 direction;

		float cutOffInner;
		float cutOffOuter;

		LightProperties properties;
	};

	//! Return the calculated Directional Light => Basically a fragment shader
	glm::vec3 calculateDirLight(DirectionalLight* lightPtr, mtrl::Material* matPtr, const glm::vec3& norm, const glm::vec3& viewDir);
	//! Return the calculated Point Light => Basically a fragment shader
	glm::vec3 calculatePointLight(PointLight* lightPtr, mtrl::Material* matPtr, const glm::vec3& norm, const glm::vec3& viewDir, const glm::vec3& fragPos);
	//! Return the calculated Spot Light => Basically a fragment shader
	glm::vec3 calculateSpotLight(SpotLight* lightPtr, mtrl::Material* matPtr, const glm::vec3& norm, const glm::vec3& viewDir, const glm::vec3& fragPos);

} // light

} // engn