#pragma once

#include <iostream>
#include <vector>

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
    //! A struct that storeal all data needed for ray cast
    struct RayCastData {
        int pixelWidth;
        int pixelHeight;

        int strideX;
        int strideY;
    };

public:
    const COLORREF SPHERE_COLOR = RGB(255, 0, 0);
    const COLORREF PLANE_COLOR = RGB(50, 50, 50);
    const COLORREF PLANE_SHADE_COLOR = RGB(20, 20, 20);
    const COLORREF SKY_COLOR = RGB(255, 200, 239);

    Scene();

    void render(const WindowRenderData& winData);

    [[nodiscard]] sphere& getSphere() { return dynamic_cast<sphere&>(*m_objects[0]); }

    void addSphere(glm::vec3 center, float radius, COLORREF color) {
        m_objects.emplace_back(new sphere{ center, radius, color });
    }

    void addPlane(glm::vec3 normal, glm::vec3 point, COLORREF color) {
        m_objects.emplace_back(new plane{ normal, point, color });
    }

private:
    void m_getRaycastOriginPaceData(float screenWidth, float screenHeight);
    //! Cast a single ray and fill a single ray entry
    void m_castRay(int x, int y, COLORREF* pixel, const RayCastData& rayCastData);
    //! Calculate lighting and materials and shadows
    void m_calculateLight(int objIdx, const HitEntry& hitEntry, COLORREF* pixel);

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
    glm::vec3 m_cameraPos = glm::vec3(0.0f, 0.0f, -10.0f);
    glm::vec3 m_cameraLookAt = glm::vec3(0.0f, 0.0f, 1.0f);


    std::vector<std::unique_ptr<hitable>> m_objects;

    glm::vec3 m_lightPosition;
};
