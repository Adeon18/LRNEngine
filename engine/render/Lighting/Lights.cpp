#include "Lights.h"


namespace engn {

	namespace light {

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