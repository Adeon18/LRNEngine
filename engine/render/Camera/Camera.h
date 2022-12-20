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

    //! Add ofset to camera without taking rotations into account
    void addWorldOffset(const glm::vec3& offset);

    void addRelativeOffset(const glm::vec3& offset);

    void addWorldRotation(const glm::vec3& angles);
    void addRelativeRotation(const glm::vec3& angles);

    //! Update all the iternal matrices and near plane data
    void updateMatrices();
    //! Turn the rotation quaternion to view matrix
    void updateBasis();
    //! Cast a single ray and return it
    ray castRay(int x, int y);
    //! Set the projection matrix
    void setProjectionMatrix(float fov, int width, int height) {
        m_projection = glm::perspective(glm::radians(fov), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
    }
    //! Set the raycastdata based on which the rays are being casted
    void setRayCastData(const RayCastData& castData) {
        m_rayCastData = castData;
    }
    //! Same but move setter
    void setRayCastData(RayCastData&& castData) {
        m_rayCastData = std::move(castData);
    }

    //! Getters for the near plane vectors
    [[nodiscard]] glm::vec3& getBRVec() { return m_BRNearClipInWorld; }
    [[nodiscard]] glm::vec3& getTLVec() { return m_TLNearClipInWorld; }
    [[nodiscard]] glm::vec4& getCamRight() { return m_view[0]; }
    [[nodiscard]] glm::vec4& getCamUp() { return m_view[1]; }
    [[nodiscard]] glm::vec4& getCamForward() { return m_view[2]; }
    glm::vec4& getCamPosition() { return m_view[3]; }


    void invertInvViewPos() {

    }
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

    //! Rotation data
    glm::quat m_rotationQuat = glm::quat{1.0f, 0.0f, 0.0f, 0.0f};

    bool m_basisUpdated = false;
    bool m_matricesUpdated = false;

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