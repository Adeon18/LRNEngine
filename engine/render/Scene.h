#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "../source/math/ray.h"
#include "../source/math/geometry/sphere.h"
#include "../source/math/geometry/plane.h"
#include "../windows/Window.h"

static COLORREF RGBtoBE(const COLORREF &rgb)
{
    return RGB(GetBValue(rgb), GetGValue(rgb), GetRValue(rgb));
}

class Scene {
public:
    const COLORREF SPHERE_COLOR = RGB(255, 0, 0);
    const COLORREF PLANE_COLOR = RGB(50, 50, 50);
    const COLORREF PLANE_SHADE_COLOR = RGB(20, 20, 20);
    const COLORREF SKY_COLOR = RGB(255, 200, 239);

    Scene(float width, float height);

    void render(WindowData winData);

    [[nodiscard]] sphere& getSphere() { return m_sphere; }

private:
    void m_getRaycastOriginPaceData(float screenWidth, float screenHeight);
private:
    glm::mat4 m_worldMatrix = glm::mat4{ 1.0f };
	glm::mat4 m_viewMatrix = glm::mat4{ 1.0f };
	glm::mat4 m_projectionMatrix;

    glm::mat4 m_clipToWorld;
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

    // Temporary camera data
    glm::vec3 m_cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_cameraLookAt = glm::vec3(0.0f, 0.0f, 1.0f);

    float m_screenWidth, m_screenHeight;

    sphere m_sphere;
    plane m_plane;
    glm::vec3 m_lightPosition;
};
