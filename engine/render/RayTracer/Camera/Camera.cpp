#include <iostream>

#include <glm/glm/gtx/quaternion.hpp>
#include <glm/glm/gtx/string_cast.hpp>

#include "Camera.h"


namespace engn {

    Camera::Camera(float fov, int screenWidth, int screenHeight, const glm::vec3& position) : m_position{ position } {
        setProjectionMatrix(fov, screenWidth, screenHeight);
        updateMatrices();
    }

    void Camera::setNewScreenSize(int width, int height) {
        setProjectionMatrix(m_fov, width, height);
    }

    void Camera::setProjectionMatrix(float fov, int width, int height) {
        m_fov = fov;
        m_projection = glm::perspective(glm::radians(m_fov), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
        m_projectionInv = glm::inverse(m_projection);
    }

    void Camera::setRayCastData(const RayCastData& castData) {
        m_rayCastData = castData;
    }


    void Camera::addWorldOffset(const glm::vec3& offset) {
        m_position += offset;

        m_matricesUpdated = false;
    }


    void Camera::addRelativeOffset(const glm::vec3& offset) {
        m_matricesUpdated = false;
        // TODO: May be slow
        m_position += offset[0] * glm::vec3(getCamRight()) +
            offset[1] * glm::vec3(getCamUp()) +
            offset[2] * glm::vec3(getCamForward());
    }


    void Camera::addWorldRotation(const glm::vec3& angles) {
        m_basisUpdated = false;
        m_matricesUpdated = false;

        // This fix of roll accumulation is incredibly stupid but will do for now: update - it breaks the camera
        /*if (angles.z == 0.0 && glm::abs(m_rotationQuat.z) < 0.05f) {
            m_rotationQuat.z = m_rotationQuat.z / 2.0f;
        }*/
        // roll, pitch, yaw
        m_rotationQuat *= glm::angleAxis(glm::radians(angles.z), glm::vec3(0.0f, 0.0f, 1.0f));
        m_rotationQuat *= glm::angleAxis(glm::radians(angles.x), glm::vec3(1.0f, 0.0f, 0.0f));
        m_rotationQuat *= glm::angleAxis(glm::radians(angles.y), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::normalize(m_rotationQuat);
    }

    void Camera::addRelativeRotation(const glm::vec3& angles) {
        m_basisUpdated = false;
        m_matricesUpdated = false;

        // roll, pitch, yaw
        m_rotationQuat *= glm::angleAxis(glm::radians(angles.z), glm::vec3{ getCamForward() });
        m_rotationQuat *= glm::angleAxis(glm::radians(angles.x), glm::vec3{ getCamRight() });
        m_rotationQuat *= glm::angleAxis(glm::radians(angles.y), glm::vec3{ getCamUp() });

        glm::normalize(m_rotationQuat);
    }


    void Camera::updateBasis() {
        if (m_basisUpdated) { return; }
        m_basisUpdated = true;

        m_viewInv = glm::toMat4(m_rotationQuat);
    }


    void Camera::updateMatrices() {

        if (m_matricesUpdated) { return; }
        m_matricesUpdated = true;

        updateBasis();

        // Add the offset(posiiton) to view inverse
        m_viewInv[3] = glm::vec4(m_position, 1.0f);

        // The inverse of product is the product of inverses, but the product order is reversed
        m_viewProjInv = m_viewInv * m_projectionInv;

        // Update viewing frustum coordinates
        for (size_t i = 0; i < 4; ++i) {
            m_viewingFrustumNearPlaneInWorldSpace[i] = m_viewProjInv * m_viewingFrustumNearPlane[i];
            m_viewingFrustumNearPlaneInWorldSpace[i] /= m_viewingFrustumNearPlaneInWorldSpace[i].w;
        }

        // Update out frustum corners
        m_BLNearClipInWorld = m_viewingFrustumNearPlaneInWorldSpace[0];
        m_TLNearClipInWorld = m_viewingFrustumNearPlaneInWorldSpace[1] - m_viewingFrustumNearPlaneInWorldSpace[0];
        m_BRNearClipInWorld = m_viewingFrustumNearPlaneInWorldSpace[3] - m_viewingFrustumNearPlaneInWorldSpace[0];

        m_TLDirection = glm::normalize(m_TLNearClipInWorld);
        m_BRDirection = glm::normalize(m_BRNearClipInWorld);
    }

    math::ray Camera::castRay(int x, int y) {

        glm::vec3 rayTo = m_BLNearClipInWorld +
            (m_rayCastData.halfPixelWidth) * m_BRDirection + (m_rayCastData.pixelWidth * x * m_rayCastData.strideX) * m_BRDirection +
            (m_rayCastData.halfPixelHeight) * m_TLDirection + (m_rayCastData.pixelHeight * y * m_rayCastData.strideY) * m_TLDirection;

        glm::vec3 rayDirection = glm::normalize(rayTo - m_position);

        return math::ray{ m_position, rayDirection };
    }

} // engn