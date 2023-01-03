#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>

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
    static struct ClosestObj{
        int objIdx;
        bool isMesh;
    };
public:
    const COLORREF SPHERE_COLOR = RGB(255, 0, 0);
    const COLORREF PLANE_COLOR = RGB(50, 50, 50);
    const COLORREF PLANE_SHADE_COLOR = RGB(20, 20, 20);
    const COLORREF SKY_COLOR = RGB(10, 10, 10);
    const COLORREF LIGHT_COLOR = RGB(200, 200, 200);

    Scene();

    void render(const WindowRenderData& winData, std::unique_ptr<Camera>& camPtr);

    // legacy
    /*void addRenderObject(math::hitable* o, const mtrl::Material& m) {
        m_renderMathObjects.emplace_back(new RenderMathObject{ o, m });
    }*/

    //! Ass sphere
    void addRenderObject(math::sphere* s, const mtrl::Material& m) {
        m_renderSpheres.emplace_back(new RenderSphereObj{ s, m });
    }
    //! Add plane
    void addRenderObject(math::plane* p, const mtrl::Material& m) {
        m_renderPlanes.emplace_back(new RenderPlaneObj{ p, m });
    }
    //! Add Mesh
    void addRenderObject(mesh::Mesh* msh, const mtrl::Material& m, const glm::vec3 pos) {
        m_renderMeshes.emplace_back(new RenderMeshObj{ msh, m, pos});
    }

    void addMesh(const std::string& name, const mesh::Mesh& mesh) {
        m_meshes[name] = mesh;
    }

    void addPointLight(const glm::vec3& pos, const light::LightProperties& prop, const glm::vec3& attenuation, const glm::vec3& color = glm::vec3{ 1.0f }) {
        m_pointLights.emplace_back(new RenderPointLightObj{ new light::PointLight(pos, prop, attenuation, color) });
    }

    void setDirectionalLight(const glm::vec3& dir, const light::LightProperties& prop) {
        m_direcLight = std::make_unique<light::DirectionalLight>(dir, prop);
    }

    void addSpotLight(const glm::vec3& dir, const glm::vec3& pos, const glm::vec2& range, const light::LightProperties& prop, const glm::vec3& color = glm::vec3{ 1.0f }) {
        m_spotLights.emplace_back(new RenderSpotLightObj{ new light::SpotLight(dir, pos, range, prop, color) });
    }

    [[nodiscard]] mesh::Mesh* getMeshPtr(const std::string& name) { return &m_meshes[name]; }

public:
    
private:
    //! Setter for camera pos, private because pos should only be set from the scene
    void m_setCameraPos(const glm::vec3& camPos) { m_camPos = camPos; }

    void m_getRaycastOriginPaceData(float screenWidth, float screenHeight);
    //! Cast a single ray and fill a single ray entry
    void m_castRay(math::ray& r, COLORREF* pixel);
    //! Calculate lighting and materials and shadows
    void m_getObjectColor(const ObjRef& closestObj, const math::HitEntry& hitEntry, COLORREF* pixel);
    //! Get the combined lighing color on an object
    glm::vec3 m_getObjectLighting(const ObjRef& closestObj, const math::HitEntry& hitEntry);

    bool m_isFragmentInDirectionShadow(const math::HitEntry& hitEntry, const glm::vec3& lightDir);

    bool m_isFragmentInPointShadow(const math::HitEntry& hitEntry, const glm::vec3& pointPos);
private:
    std::unique_ptr<light::DirectionalLight> m_direcLight;

    // legacy
    std::vector<std::unique_ptr<RenderMathObject>> m_renderMathObjects;

    std::vector<std::unique_ptr<RenderPlaneObj>> m_renderPlanes;
    std::vector<std::unique_ptr<RenderSphereObj>> m_renderSpheres;
    std::vector<std::unique_ptr<RenderMeshObj>> m_renderMeshes;
    std::vector<std::unique_ptr<RenderPointLightObj>> m_pointLights;
    std::vector<std::unique_ptr<RenderSpotLightObj>> m_spotLights;

    // Shared mesh storage as follows: name, mesh
    std::unordered_map<std::string, mesh::Mesh> m_meshes;

    glm::vec3 m_camPos;
};

} // engn