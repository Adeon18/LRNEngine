#pragma once

#include <cmath>
#include "source/math/geometry/sphere.h"

#include <glm/glm.hpp>


namespace light {

//! Basic Light data that is needed for each light type
struct LightProperties {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

//! Directional Light that only depends on it's direction
struct DirectionalLight {
	DirectionalLight(const glm::vec3& dir, const LightProperties& prop):
		direction{ dir },
		properties{ prop }
	{}

	DirectionalLight(const glm::vec3& dir, const LightProperties& prop, const glm::vec3& color) :
		direction{ dir }
	{
		properties.diffuse = color * prop.diffuse;
		properties.ambient = properties.diffuse * prop.ambient;
		properties.specular = prop.specular;
	}

	glm::vec3 direction;

	LightProperties properties;
};

//! Basic pointlight that spits light in all directions
struct PointLight {
	PointLight(const glm::vec3& pos, const LightProperties& prop, const glm::vec3& attenuation) :
		position{ pos },
		properties{ prop }
	{
		constant = attenuation.x;
		linear = attenuation.y;
		quadratic = attenuation.z;

		shape = new sphere{ pos, 0.3f };
	}

	PointLight(const glm::vec3& pos, const LightProperties& prop, const glm::vec3& attenuation, const glm::vec3& color) :
		position{ pos }
	{
		properties.diffuse = color * prop.diffuse;
		properties.ambient = properties.diffuse * prop.ambient;
		properties.specular = color * prop.specular;

		constant = attenuation.x;
		linear = attenuation.y;
		quadratic = attenuation.z;

		shape = new sphere{ pos, 0.3f };
	}

	~PointLight() {
		delete shape;
	}
	hitable* shape = nullptr;

	glm::vec3 position;

	LightProperties properties;

	float constant;
	float linear;
	float quadratic;
};

//! Light type that works like a flashlight and has a cutoff angle
struct SpotLight {
	SpotLight(const glm::vec3& dir, const glm::vec3& pos, const glm::vec2& range, const LightProperties& prop) :
		direction{ dir },
		position{ pos },
		properties{ prop }
	{
		cutOffInner = range.x;
		cutOffOuter = range.y;

		shape = new sphere{ pos, 0.3f };
	}

	SpotLight(const glm::vec3& dir, const glm::vec3& pos, const glm::vec2& range, const LightProperties& prop, const glm::vec3& color) :
		direction{ dir },
		position{ pos }
	{
		properties.diffuse = color * prop.diffuse;
		properties.ambient = properties.diffuse * prop.ambient;
		properties.specular = color * prop.specular;

		cutOffInner = range.x;
		cutOffOuter = range.y;

		shape = new sphere{ pos, 0.3f };
	}

	~SpotLight() {
		delete shape;
	}

	hitable* shape = nullptr;

	glm::vec3 position;
	glm::vec3 direction;

	float cutOffInner;
	float cutOffOuter;

	LightProperties properties;
};

inline glm::vec3 calculateDirLight(DirectionalLight* lightPtr, Material* matPtr, const glm::vec3& norm, const glm::vec3& viewDir) {
	glm::vec3 lightDir = glm::normalize(-lightPtr->direction);
	// Diffuse shading
	float diff = (std::max)(glm::dot(norm, lightDir), 0.0f);

	// specular shading bling vs phong
	float spec = 0.0f;
	if (1) {
		glm::vec3 halfWayDir = glm::normalize(lightDir + viewDir);
		spec = pow((std::max)(glm::dot(norm, halfWayDir), 0.0f), matPtr->shininess);
	}
	else {
		glm::vec3 reflectDir = glm::reflect(-lightDir, norm);
		spec = pow((std::max)(glm::dot(viewDir, reflectDir), 0.0f), matPtr->shininess);
	}
	
	// combine results
	glm::vec3 ambient = lightPtr->properties.ambient * matPtr->ambient;
	glm::vec3 diffuse = lightPtr->properties.diffuse * diff * matPtr->diffuse;
	glm::vec3 specular = lightPtr->properties.specular * spec * matPtr->specular;

	glm::vec3 res = ambient + diffuse + specular;

	return res;
}

inline glm::vec3 calculatePointLight(PointLight* lightPtr, Material* matPtr, const glm::vec3& norm, const glm::vec3& viewDir, const glm::vec3& fragPos) {
	glm::vec3 lightDir = glm::normalize(lightPtr->position - fragPos);

	// Diffuse shading
	float diff = (std::max)(glm::dot(norm, lightDir), 0.0f);

	// specular shading bling vs phong
	float spec = 0.0f;
	if (1) {
		glm::vec3 halfWayDir = glm::normalize(lightDir + viewDir);
		spec = pow((std::max)(glm::dot(norm, halfWayDir), 0.0f), matPtr->shininess);
	}
	else {
		glm::vec3 reflectDir = glm::reflect(-lightDir, norm);
		spec = pow((std::max)(glm::dot(viewDir, reflectDir), 0.0f), matPtr->shininess);
	}

	float dist = glm::length(lightPtr->position - fragPos);
	float attenuation = 1.0f / (lightPtr->constant + lightPtr->linear * dist + lightPtr->quadratic * dist * dist);

	glm::vec3 ambient = lightPtr->properties.ambient * matPtr->ambient * attenuation;
	glm::vec3 diffuse = lightPtr->properties.diffuse * diff * matPtr->diffuse * attenuation;
	glm::vec3 specular = lightPtr->properties.specular * spec * matPtr->specular * attenuation;

	glm::vec3 res = ambient + diffuse + specular;

	return res;
}

inline glm::vec3 calculateSpotLight(SpotLight* lightPtr, Material* matPtr, const glm::vec3& norm, const glm::vec3& viewDir, const glm::vec3& fragPos) {

	glm::vec3 lightDir = glm::normalize(lightPtr->position - fragPos);
	float theta = glm::dot(lightDir, glm::normalize(-lightPtr->direction));

	glm::vec3 res{};

	float diff = (std::max)(glm::dot(norm, lightDir), 0.0f);

	// specular shading bling vs phong
	float spec = 0.0f;
	if (1) {
		glm::vec3 halfWayDir = glm::normalize(lightDir + viewDir);
		spec = pow((std::max)(glm::dot(norm, halfWayDir), 0.0f), matPtr->shininess);
	}
	else {
		glm::vec3 reflectDir = glm::reflect(-lightDir, norm);
		spec = pow((std::max)(glm::dot(viewDir, reflectDir), 0.0f), matPtr->shininess);
	}

	glm::vec3 ambient = lightPtr->properties.ambient * matPtr->ambient;
	glm::vec3 diffuse = lightPtr->properties.diffuse * diff * matPtr->diffuse;
	glm::vec3 specular = lightPtr->properties.specular * spec * matPtr->specular;

	// Soft edges
	float epsilon = (lightPtr->cutOffInner - lightPtr->cutOffOuter);
	float intensity = glm::clamp((theta - lightPtr->cutOffOuter) / epsilon, 0.0f, 1.0f);

	diffuse *= intensity;
	specular *= intensity;

	res = ambient + diffuse + specular;

	return res;
}

}