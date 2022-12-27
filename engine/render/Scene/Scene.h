#pragma once

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "render/RenderData/RenderData.h"

#include "render/RenderObject/RenderObject.h"
#include "render/Materials/Material.h"
#include "render/Lighting/LightSources.h"

#include "source/math/ray.h"
#include "source/math/geometry/sphere.h"
#include "source/math/geometry/plane.h"
#include "windows/Window.h"
#include "render/Camera/Camera.h"

namespace engn {

static COLORREF RGBtoBE(const COLORREF& rgb)
{
    return RGB(GetBValue(rgb), GetGValue(rgb), GetRValue(rgb));
}


class Scene {
public:
    const COLORREF SPHERE_COLOR = RGB(255, 0, 0);
    const COLORREF PLANE_COLOR = RGB(50, 50, 50);
    const COLORREF PLANE_SHADE_COLOR = RGB(20, 20, 20);
    const COLORREF SKY_COLOR = RGB(10, 10, 10);
    const COLORREF LIGHT_COLOR = RGB(200, 200, 200);

    Scene();

    void render(const WindowRenderData& winData, std::unique_ptr<Camera>& camPtr);

    [[nodiscard]] math::sphere& getSphere() { return dynamic_cast<math::sphere&>(*m_objects[0]); }

    void addSphere(const glm::vec3& center, float radius) {
        m_objects.emplace_back(new math::sphere{ center, radius });
    }

    void addPlane(const glm::vec3& normal, glm::vec3 point) {
        m_objects.emplace_back(new math::plane{ normal, point });
    }

    void addRenderObject(math::hitable* o, const mtrl::Material& m) {
        m_renderObjects.emplace_back(new RenderObject{ o, m });
    }

    void addPointLight(const glm::vec3& pos, const light::LightProperties& prop, const glm::vec3& attenuation, const glm::vec3& color = glm::vec3{ 1.0f }) {
        m_pointLights.emplace_back(new light::PointLight(pos, prop, attenuation, color));
    }

    void setDirectionalLight(const glm::vec3& dir, const light::LightProperties& prop) {
        m_direcLight = std::make_unique<light::DirectionalLight>(dir, prop);
    }

    void setSpotLight(const glm::vec3& dir, const glm::vec3& pos, const glm::vec2& range, const light::LightProperties& prop, const glm::vec3& color = glm::vec3{ 1.0f }) {
        m_spotLight = std::make_unique<light::SpotLight>(dir, pos, range, prop, color);
    }

private:
    void m_getRaycastOriginPaceData(float screenWidth, float screenHeight);
    //! Cast a single ray and fill a single ray entry
    void m_castRay(const math::ray& r, COLORREF* pixel, const glm::vec3& camPos);
    //! Calculate lighting and materials and shadows
    void m_getObjectColor(int objIdx, const math::HitEntry& hitEntry, COLORREF* pixel, const glm::vec3& camPos);
    //! Get the combined lighing color on an object
    glm::vec3 m_getObjectLighting(int objIdx, const math::HitEntry& hitEntry, const glm::vec3& camPos);

    bool m_isFragmentInDirectionShadow(const math::HitEntry& hitEntry, const glm::vec3& lightDir);

    bool m_isFragmentInPointShadow(const math::HitEntry& hitEntry, const glm::vec3& pointPos);
private:
    std::unique_ptr<light::DirectionalLight> m_direcLight;
    std::unique_ptr<light::SpotLight> m_spotLight;
    // obsolete
    std::vector<std::unique_ptr<math::hitable>> m_objects;

    std::vector<std::unique_ptr<RenderObject>> m_renderObjects;
    std::vector<std::unique_ptr<light::PointLight>> m_pointLights;

    glm::vec3 m_lightPosition;
};

} // engn