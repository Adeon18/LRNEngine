#include <iostream>

#include <glm/gtx/string_cast.hpp>

#include "Camera.h"


Camera::Camera(float fov, int screenWidth, int screenHeight, glm::vec3 position): m_position{position} {
    setProjectionMatrix(fov, screenWidth, screenHeight);
    //std::cout << glm::to_string(m_projection) << std::endl;
    updateMatrices();
}


void Camera::updateMatrices() {
    m_view = glm::lookAt(m_position, m_position + m_direction, m_up);
    //std::cout << glm::to_string(m_view) << std::endl;
    // Very slow
    m_viewInv = glm::inverse(m_view);
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

    //std::cout << "BL: " << glm::to_string(m_BLNearClipInWorld) << std::endl;
    //std::cout << "TL: " << glm::to_string(m_TLNearClipInWorld) << std::endl;
    //std::cout << "BR: " << glm::to_string(m_BRNearClipInWorld) << std::endl;
}

ray Camera::castRay(int x, int y) {
    glm::vec3 rayTo = glm::vec3{
                m_BLNearClipInWorld.x + m_rayCastData.pixelWidth / 2.0f + (x * m_rayCastData.strideX) * m_rayCastData.pixelWidth,
                m_BLNearClipInWorld.y + m_rayCastData.pixelHeight / 2.0f + (y * m_rayCastData.strideY) * m_rayCastData.pixelHeight,
                m_position.z + 0.1f };

    glm::vec3 rayDirection = glm::normalize(rayTo - m_position);

    //std::cout << "Ray Origin: " << glm::to_string(m_position) << std::endl;
    //std::cout << "Ray Direction: " << glm::to_string(rayDirection) << std::endl;
    return ray{ m_position, rayDirection };
}