#include "Scene.h"


Scene::Scene(float width, float height):
	m_sphere{ glm::vec3{400, 300, 20}, 75 },
	m_plane{glm::normalize(glm::vec3(0.0f, -0.99f, 0.01f)), m_sphere.m_center + m_sphere.m_radius},
    m_lightPosition{ 0, 0, 0 }
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


void Scene::render(const WindowRenderData& winData) 

{
    // Don't render the minimized window
    if (winData.screenWidth == 0 || winData.screenHeight == 0) {
        return;
    }

    m_getRaycastOriginPaceData(winData.screenWidth, winData.screenHeight);

    // TODO: Temporary for test
    //glm::vec3 direction = glm::vec3{0, 0, 1};

    glm::vec3 lightDirection = glm::normalize(glm::vec3(1.0f, 1.0f, -1.0f));

    float pixelWidth = winData.screenWidth / glm::length(m_BRNearClipInWorld);
    float pixelHeight = winData.screenHeight / glm::length(m_TLNearClipInWorld);

    //std::cout << "Pixel width: " << pixel_width << " Pixel height: " << pixel_height << std::endl;

    //size_t raysHit = 0;
    //size_t raysMissed = 0;

    // Cast rays for each pixel
    int strideX = winData.screenWidth / winData.bufferWidth;
    int strideY = winData.screenHeight / winData.bufferHeight;

    auto* pixel = static_cast<COLORREF*>(winData.screenBuffer);
    for (int y = 0; y < winData.bufferHeight; ++y)
    {
        for (int x = 0; x < winData.bufferWidth; ++x)
        {
            glm::vec3 rayOrigin = glm::vec3{
            	m_BLNearClipInWorld.x + pixelWidth / 2.f + (x * strideX) * pixelWidth,
            	m_BLNearClipInWorld.y + pixelHeight / 2.0f + pixelHeight * (y * strideY),
            	m_BLNearClipInWorld.z };
            glm::vec3 rayDirection = glm::normalize(m_cameraLookAt - m_cameraPos);

            // Create ray object - temporary camera data gets passed
            ray r{ rayOrigin, rayDirection };

            auto collisionLog = m_sphere.hit(r);
        	if (collisionLog.isHit)
            {
                auto collisionLogInside = m_plane.hit(r);
                collisionLog = m_sphere.hit(r);
                glm::vec3 directionToLight = -lightDirection;

                float lightPower = glm::clamp(glm::dot(collisionLog.hitNormal, glm::normalize(m_lightPosition - m_sphere.m_center)), 0.0f, 1.0f);

                //if (255 * glm::dot(normal, glm::normalize(collisionHit)) < 0) std::cout << "Die" << std::endl;
                COLORREF hit_color = RGBtoBE(RGB(GetRValue(SPHERE_COLOR) * lightPower, GetGValue(SPHERE_COLOR) * lightPower, GetBValue(SPHERE_COLOR) * lightPower));
                *pixel = hit_color;
                pixel++;
                // Debug
                //raysHit++;
                continue;
            }

            //! Plane collision and shadow
            collisionLog = m_plane.hit(r);
            if (collisionLog.isHit)
            {
                // Cast shadow
                ray toLight{ collisionLog.hitPoint, m_lightPosition - collisionLog.hitPoint };
                COLORREF hit_color = RGBtoBE(PLANE_COLOR);
                if (m_sphere.hit(toLight).isHit)
                {
                    hit_color = RGBtoBE(PLANE_SHADE_COLOR);
                }

                *pixel = hit_color;
                pixel++;
                // Debug
                //raysMissed++;
                continue;
            }

            // Sky
            COLORREF hit_color = RGBtoBE(SKY_COLOR);
            *pixel = hit_color;
            pixel++;
            // Debug
            //raysMissed++;
        }
    }
    
    // std::cout << "Hit: " << raysHit << " Missed: " << raysMissed << std::endl;
}

