#include "Scene.h"


Scene::Scene():
    m_lightPosition{ 0, 0, -300 }
{
    // TODO: hardcoded and works strange, should be changed when I add camera
    m_viewMatrix = glm::lookAt(
        m_cameraPos,
        m_cameraLookAt,
        glm::vec3{ 0.0f, 1.0f, 0.0f });

    //m_viewMatrix = glm::mat4(1.0f);
}


void Scene::m_getRaycastOriginPaceData(float screenWidth, float screenHeight)
{
    m_projectionMatrix = glm::ortho(0.0f, screenWidth, 0.0f, screenHeight);

    m_clipToWorld = glm::inverse(m_projectionMatrix * m_viewMatrix);

    // Calculate viewing frustum near plane in world space - plane coordinates start at BottomLeft and proceed clockwise
    for (size_t i = 0; i < 4; ++i)
    {
        m_viewingFrustumNearPlaneInWorldSpace[i] = m_clipToWorld * m_viewingFrustumNearPlane[i];
        m_viewingFrustumNearPlaneInWorldSpace[i] /= m_viewingFrustumNearPlaneInWorldSpace[i].w;
    }

    // Calculate 3 Edges of viewing frustum near plane
    m_BLNearClipInWorld = m_viewingFrustumNearPlaneInWorldSpace[0];
    m_TLNearClipInWorld = m_viewingFrustumNearPlaneInWorldSpace[1] - m_viewingFrustumNearPlaneInWorldSpace[0];
    m_BRNearClipInWorld = m_viewingFrustumNearPlaneInWorldSpace[3] - m_viewingFrustumNearPlaneInWorldSpace[0];

}


void Scene::m_calculateLight(int objIdx, const HitEntry& hitEntry, COLORREF* pixel) {

    COLORREF hit_color;

    if (hitEntry.isHit) {
        // Check if shadowed
        bool isInShadow = false;
        ray toLight{ hitEntry.hitPoint + 0.001f * (m_lightPosition - hitEntry.hitPoint), m_lightPosition - hitEntry.hitPoint};

        for (auto const& objPtr : m_objects) {
            auto collisionLog = objPtr->hit(toLight);
            if (collisionLog.isHit && collisionLog.rayT > 0) {
                isInShadow = true;
                break;
            }
        }
        // Compute light if not
        if (!isInShadow) {
            float lightPower = glm::clamp(
                glm::dot(
                    hitEntry.hitNormal,
                    glm::normalize(m_lightPosition - hitEntry.hitPoint)
                ),
                0.2f, 1.0f
            );
            hit_color = RGBtoBE(RGB(
                GetRValue(m_objects[objIdx]->m_color) * lightPower,
                GetGValue(m_objects[objIdx]->m_color) * lightPower,
                GetBValue(m_objects[objIdx]->m_color) * lightPower)
            );
        }
        //! If shadow => shade
        else {
            hit_color = RGBtoBE(RGB(
                GetRValue(m_objects[objIdx]->m_color) * 0.1f,
                GetGValue(m_objects[objIdx]->m_color) * 0.1f,
                GetBValue(m_objects[objIdx]->m_color) * 0.1f)
            );
        }
    } else {
        hit_color = SKY_COLOR;
    }

    *pixel = hit_color;
}


void Scene::m_castRay(int x, int y, COLORREF* pixel, const RayCastData& rayCastData) {
    glm::vec3 rayOrigin = glm::vec3{
                m_BLNearClipInWorld.x + rayCastData.pixelWidth / 2.0f + (x * rayCastData.strideX) * rayCastData.pixelWidth,
                m_BLNearClipInWorld.y + rayCastData.pixelHeight / 2.0f + (y * rayCastData.strideY) * rayCastData.pixelHeight,
                m_BLNearClipInWorld.z };
    glm::vec3 rayDirection = glm::normalize(m_cameraLookAt - m_cameraPos);

    // Create ray object - temporary camera data gets passed
    ray r{ rayOrigin, rayDirection };
 
    // Find the object closest to the ray
    HitEntry closestEntry = m_objects[0]->hit(r);

    int closestObjIdx = 0;
    for (size_t i = 1; i < m_objects.size(); ++i) {
        auto collisionLog = m_objects[i]->hit(r);
        if (collisionLog.isHit && collisionLog.rayT < closestEntry.rayT) {
            closestEntry = collisionLog;
            closestObjIdx = i;
        }
    }

    m_calculateLight(closestObjIdx, closestEntry, pixel);
}


void Scene::render(const WindowRenderData& winData) 

{
    // Don't render the minimized window
    if (winData.screenWidth == 0 || winData.screenHeight == 0) {
        return;
    }

    m_getRaycastOriginPaceData(winData.screenWidth, winData.screenHeight);

    RayCastData rayCastData{
        winData.screenWidth / glm::length(m_BRNearClipInWorld),
        winData.screenHeight / glm::length(m_TLNearClipInWorld),
        winData.screenWidth / winData.bufferWidth,
        winData.screenHeight / winData.bufferHeight
    };

    auto* pixel = static_cast<COLORREF*>(winData.screenBuffer);
    for (int y = 0; y < winData.bufferHeight; ++y)
    {
        for (int x = 0; x < winData.bufferWidth; ++x)
        {
            m_castRay(x, y, pixel, rayCastData);
            pixel++;
        }
    }
}

