#include <iostream>

#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Camera.h"


Camera::Camera(float fov, int screenWidth, int screenHeight, glm::vec3 position): m_position{position} {
    setProjectionMatrix(fov, screenWidth, screenHeight);
    //std::cout << glm::to_string(m_projection) << std::endl;
    updateMatrices();
}


void Camera::addWorldOffset(const glm::vec3& offset) {
    m_position += offset;

    m_matricesUpdated = false;

    updateMatrices();
}


void Camera::addRelativeOffset(const glm::vec3& offset) {
    updateBasis();

    m_matricesUpdated = false;
    m_position += offset[0] * glm::vec3(getCamRight()) + offset[1] * glm::vec3(getCamUp()) + offset[2] * glm::vec3(getCamForward().x, -getCamForward().y, getCamForward().z);

    updateMatrices();
}


void Camera::addWorldRotation(const glm::vec3& angles) {
    m_basisUpdated = false;
    m_matricesUpdated = false;
    // roll, pitch, yaw
    m_rotationQuat *= glm::angleAxis(glm::radians(angles.z), glm::vec3(0.0f, 0.0f, 1.0f));
    m_rotationQuat *= glm::angleAxis(glm::radians(angles.x), glm::vec3(1.0f, 0.0f, 0.0f));
    m_rotationQuat *= glm::angleAxis(glm::radians(angles.y), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::normalize(m_rotationQuat);

    updateMatrices();
}

void Camera::addRelativeRotation(const glm::vec3& angles) {
    m_basisUpdated = false;
    m_matricesUpdated = false;

    // roll, pitch, yaw
    m_rotationQuat *= glm::angleAxis(glm::radians(angles.z), glm::vec3{ getCamForward() });
    m_rotationQuat *= glm::angleAxis(glm::radians(angles.x), glm::vec3{ getCamRight() });
    m_rotationQuat *= glm::angleAxis(glm::radians(angles.y), glm::vec3{ getCamUp() });

    glm::normalize(m_rotationQuat);
    std::cout << glm::to_string(m_rotationQuat) << std::endl;

    updateMatrices();
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

    //m_view = glm::lookAt(m_position, m_position + m_direction, m_up);

    //for (int i = 0; i < 4; ++i) {
    //    std::cout << glm::to_string(m_viewInv[i]) << std::endl;
    //}
    //std::cout << "--------------" << std::endl;

    m_viewInv[3] = glm::vec4(m_position, 1.0f);
    //for (int i = 0; i < 4; ++i) {
    //    std::cout << glm::to_string(m_viewInv[i]) << std::endl;
    //}
    std::cout << "--------------" << std::endl;
    m_projectionInv = glm::inverse(m_projection);

    // The inverse of product is the product of inverses, but the product order is reversed
    m_viewProjInv = m_viewInv * m_projectionInv;

    // Update viewing frustum coordinates
    for (size_t i = 0; i < 4; ++i) {
        m_viewingFrustumNearPlaneInWorldSpace[i] = m_viewProjInv * m_viewingFrustumNearPlane[i];
        m_viewingFrustumNearPlaneInWorldSpace[i] /= m_viewingFrustumNearPlaneInWorldSpace[i].w;
        // std::cout << glm::to_string(m_viewingFrustumNearPlaneInWorldSpace[i]) << std::endl;
    }
    // Update out frustum corners
    m_BLNearClipInWorld = m_viewingFrustumNearPlaneInWorldSpace[0];
    m_TLNearClipInWorld = m_viewingFrustumNearPlaneInWorldSpace[1] - m_viewingFrustumNearPlaneInWorldSpace[0];
    m_BRNearClipInWorld = m_viewingFrustumNearPlaneInWorldSpace[3] - m_viewingFrustumNearPlaneInWorldSpace[0];

    m_TLDirection = glm::normalize(m_TLNearClipInWorld);
    m_BRDirection = glm::normalize(m_BRNearClipInWorld);
    std::cout << glm::to_string(m_BRDirection) << std::endl;

    //std::cout << "BL: " << glm::to_string(m_BLNearClipInWorld) << std::endl;
    //std::cout << "TL: " << glm::to_string(m_TLNearClipInWorld) << std::endl;
    //std::cout << "BR: " << glm::to_string(m_BRNearClipInWorld) << std::endl;
}

ray Camera::castRay(int x, int y) {
    /*glm::vec3 rayTo = glm::vec3{
                m_BLNearClipInWorld.x + m_rayCastData.pixelWidth / 2.0f + (x * m_rayCastData.strideX) * m_rayCastData.pixelWidth,
                m_BLNearClipInWorld.y + m_rayCastData.pixelHeight / 2.0f + (y * m_rayCastData.strideY) * m_rayCastData.pixelHeight,
                m_position.z + 0.1f };*/

    glm::vec3 rayTo = m_BLNearClipInWorld + (m_rayCastData.pixelWidth / 2.0f) * m_BRDirection + (m_rayCastData.pixelWidth * x * m_rayCastData.strideX) * m_BRDirection;
    rayTo += (m_rayCastData.pixelHeight / 2.0f) * m_TLDirection + (m_rayCastData.pixelHeight * y * m_rayCastData.strideY) * m_TLDirection;
    //rayTo.z = m_position.z + 0.1f /** getCamForward()*/;
    //std::cout << glm::to_string(0.1f * glm::normalize(glm::vec3(getCamForward()))) << std::endl;
    rayTo += 0.2f * glm::normalize(glm::vec3(getCamForward()));
    //std::cout << rayTo.z << std::endl;
    glm::vec3 rayDirection = glm::normalize(rayTo - m_position);

    //std::cout << "Ray Origin: " << glm::to_string(m_position) << std::endl;
    //std::cout << "Ray Direction: " << glm::to_string(rayDirection) << std::endl;
    return ray{ m_position, rayDirection };
}