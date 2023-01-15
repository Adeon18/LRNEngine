#include "Lights.h"


namespace engn {

	namespace light {

		DirectionalLight::DirectionalLight(const glm::vec3& dir, const LightProperties& prop, const glm::vec3& color) :
			direction{ dir }
		{
			properties.diffuse = color * prop.diffuse;
			properties.ambient = properties.diffuse * prop.ambient;
			properties.specular = prop.specular;
		}


		PointLight::PointLight(const glm::vec3& pos, const LightProperties& prop, const glm::vec3& attenuation) :
			position{ pos },
			properties{ prop }
		{
			constant = attenuation.x;
			linear = attenuation.y;
			quadratic = attenuation.z;
		}

		PointLight::PointLight(const glm::vec3& pos, const LightProperties& prop, const glm::vec3& attenuation, const glm::vec3& color) :
			position{ pos }
		{
			properties.diffuse = color * prop.diffuse;
			properties.ambient = properties.diffuse * prop.ambient;
			properties.specular = color * prop.specular;

			constant = attenuation.x;
			linear = attenuation.y;
			quadratic = attenuation.z;
		}


		SpotLight::SpotLight(const glm::vec3& dir, const glm::vec3& pos, const glm::vec2& range, const LightProperties& prop) :
			direction{ dir },
			position{ pos },
			properties{ prop }
		{
			cutOffInner = range.x;
			cutOffOuter = range.y;
		}

		SpotLight::SpotLight(const glm::vec3& dir, const glm::vec3& pos, const glm::vec2& range, const LightProperties& prop, const glm::vec3& color) :
			direction{ dir },
			position{ pos }
		{
			properties.diffuse = color * prop.diffuse;
			properties.ambient = properties.diffuse * prop.ambient;
			properties.specular = color * prop.specular;

			cutOffInner = range.x;
			cutOffOuter = range.y;
		}

		glm::vec3 calculateDirLight(DirectionalLight* lightPtr, mtrl::Material* matPtr, const glm::vec3& norm, const glm::vec3& viewDir) {
			glm::vec3 lightDir = glm::normalize(-lightPtr->direction);
			// Diffuse shading
			float diff = (std::max)(glm::dot(norm, lightDir), 0.0f);

			// specular shading bling vs phong
			float spec = 0.0f;
			if (MODEL_BLING_PHONG) {
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


		glm::vec3 calculatePointLight(PointLight* lightPtr, mtrl::Material* matPtr, const glm::vec3& norm, const glm::vec3& viewDir, const glm::vec3& fragPos) {
			glm::vec3 lightDir = glm::normalize(lightPtr->position - fragPos);

			// Diffuse shading
			float diff = (std::max)(glm::dot(norm, lightDir), 0.0f);

			// specular shading bling vs phong
			float spec = 0.0f;
			if (MODEL_BLING_PHONG) {
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


		glm::vec3 calculateSpotLight(SpotLight* lightPtr, mtrl::Material* matPtr, const glm::vec3& norm, const glm::vec3& viewDir, const glm::vec3& fragPos) {

			glm::vec3 lightDir = glm::normalize(lightPtr->position - fragPos);
			float theta = glm::dot(lightDir, glm::normalize(-lightPtr->direction));

			glm::vec3 res{};

			float diff = (std::max)(glm::dot(norm, lightDir), 0.0f);

			// specular shading bling vs phong
			float spec = 0.0f;
			if (MODEL_BLING_PHONG) {
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
	
	} // light

} // engn