#pragma once

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "render/RenderData/RenderData.h"

#include "source/math/ray.h"
#include "source/math/geometry/sphere.h"
#include "source/math/geometry/plane.h"
#include "windows/Window.h"
#include "render/Camera/Camera.h"

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

    Scene();

    void render(const WindowRenderData& winData, std::unique_ptr<Camera>& camPtr);

    [[nodiscard]] sphere& getSphere() { return dynamic_cast<sphere&>(*m_objects[0]); }

    void addSphere(const glm::vec3& center, float radius, COLORREF color) {
        m_objects.emplace_back(new sphere{ center, radius, color });
    }

    void addPlane(const glm::vec3& normal, glm::vec3 point, COLORREF color) {
        m_objects.emplace_back(new plane{ normal, point, color });
    }

private:
    void m_getRaycastOriginPaceData(float screenWidth, float screenHeight);
    //! Cast a single ray and fill a single ray entry
    void m_castRay(const ray& r, COLORREF* pixel);
    //! Calculate lighting and materials and shadows
    void m_calculateLight(int objIdx, const HitEntry& hitEntry, COLORREF* pixel);

private:
    std::vector<std::unique_ptr<hitable>> m_objects;

    glm::vec3 m_lightPosition;
};
