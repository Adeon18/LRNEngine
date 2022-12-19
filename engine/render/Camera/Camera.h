#pragma once


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "render/RenderData/RenderData.h"
#include "source/math/ray.h"
#include "windows/Window.h"

class Camera {

public:
    Camera(float fov, int screenWidth, int screenHeight, glm::vec3 position);

    void moveCamera(glm::vec3 offset) { m_position += offset; }

    void updateMatrices();

    ray castRay(int x, int y);

    void setProjectionMatrix(float fov, int width, int height) {
        m_projection = glm::perspective(glm::radians(fov), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
    }

    void setRayCastData(const RayCastData& castData) {
        m_rayCastData = castData;
    }

    void setRayCastData(RayCastData&& castData) {
        m_rayCastData = std::move(castData);
    }

    [[nodiscard]] glm::vec3& getBRVec() { return m_BRNearClipInWorld; }
    [[nodiscard]] glm::vec3& getTLVec() { return m_TLNearClipInWorld; }
private:
    //! Position Data
	glm::vec3 m_position;
    glm::vec3 m_direction = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);

    //! Projection data
	glm::mat4 m_view = glm::mat4(1.0f);
	glm::mat4 m_projection = glm::mat4(1.0f);

	glm::mat4 m_viewInv = glm::mat4(1.0f);
	glm::mat4 m_projectionInv = glm::mat4(1.0f);

    glm::mat4 m_viewProjInv = glm::mat4(1.0f);

    //! Raycasting data
    glm::vec4 m_viewingFrustumNearPlaneInWorldSpace[4];
    glm::vec4 m_viewingFrustumNearPlane[4] =
    {
        {-1.0f, -1.0f, -1.0f, 1.0f},
        {-1.0f,  1.0f, -1.0f, 1.0f},
        { 1.0f,  1.0f, -1.0f, 1.0f},
        { 1.0f, -1.0f, -1.0f, 1.0f},
    };

    glm::vec3 m_BLNearClipInWorld;
    glm::vec3 m_TLNearClipInWorld;
    glm::vec3 m_BRNearClipInWorld;

    RayCastData m_rayCastData;
};