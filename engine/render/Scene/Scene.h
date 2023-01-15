#pragma once

#include <vector>
#include <iostream>
#include <unordered_map>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtx/string_cast.hpp>

#include "render/Camera/Camera.h"
#include "render/Lighting/Lights.h"
#include "render/Materials/Material.h"
#include "render/RenderData/RenderData.h"
#include "render/RenderObject/RenderObject.h"

#include "source/math/ray.h"
#include "source/math/geometry/plane.h"
#include "source/math/geometry/sphere.h"

#include "source/draggers/dragger.h"

#include "windows/Window.h"
#include "utils/paralell_executor/parallel_executor.h"


namespace engn {

    //! Convert vector to colorref(it's in reverse)
    static COLORREF vecToCOLORREF(const glm::vec3& rgb)
    {
        return RGB(rgb.z, rgb.y, rgb.x);
    }


    class Scene {
    public:
        //! The hit query struct that is utilized for object operations
        struct HitDraggerQuery {
            ObjRef objRef;
            math::HitEntry hitEntry{};
            std::unique_ptr<math::IDragger> dragger;

            void free() {
                dragger.reset(nullptr);
                objRef.clear();
                hitEntry.rayT = math::hitable::MAX_DIST;
            }
        };
    public:
        Scene() = default;

        //! Responsible for rendering the entire scene
        void render(const WindowRenderData& winData, std::unique_ptr<Camera>& camPtr, std::unique_ptr<ParallelExecutor>& executor);

        //! Add sphere
        void addRenderObject(math::sphere* s, const mtrl::Material& m);
        //! Add plane
        void addRenderObject(math::plane* p, const mtrl::Material& m);
        //! Add Mesh
        void addRenderObject(mesh::Mesh* msh, const mtrl::Material& m, const glm::vec3 pos);

        void addMesh(const std::string& name, const mesh::Mesh& mesh);

        void addPointLight(const glm::vec3& pos, const light::LightProperties& prop, const glm::vec3& attenuation, const glm::vec3& color = glm::vec3{ 1.0f });

        void setDirectionalLight(const glm::vec3& dir, const light::LightProperties& prop);

        void addSpotLight(const glm::vec3& dir, const glm::vec3& pos, const glm::vec2& range, const light::LightProperties& prop, const glm::vec3& color = glm::vec3{ 1.0f });

        //! Get the pointer to the shared mesh object by name
        [[nodiscard]] mesh::Mesh* getMeshPtr(const std::string& name);

        //! Find if there is an object we can drag, if there is => initialize query with a dragger
        //! Mouse coords should be already transformed to buffer coords
        bool findDraggable(const glm::vec2& rayCastTo, std::unique_ptr<Camera>& camPtr);

        //! Move the binded draggable object to the new position
        void moveDraggable(const glm::vec2& rayCastTo, std::unique_ptr<Camera>& camPtr);
    
    private:
        //! Setter for camera pos, private because pos should only be set from the scene
        void m_setCameraPos(const glm::vec3& camPos) { m_camPos = camPos; }
        //! Compute the color of the pixel by the x and y coordinates
        void m_computePixelColor(int y, int x, COLORREF* pixel, const WindowRenderData& winData, std::unique_ptr<Camera>& camPtr);

        //! Cast a single ray and fill a single ray entry
        void m_castRay(math::ray& r, COLORREF* pixel);
        //! Calculate lighting and materials and shadows
        void m_getObjectColor(const ObjRef& closestObj, const math::HitEntry& hitEntry, COLORREF* pixel);
        //! Get the combined lighing color on an object
        glm::vec3 m_getObjectLighting(const ObjRef& closestObj, const math::HitEntry& hitEntry);

        //! Chck if fragment is in the direction light shadow
        bool m_isFragmentInDirectionShadow(const math::HitEntry& hitEntry, const glm::vec3& lightDir);
        //! Chck if fragment is in the point/spotlight shadow
        bool m_isFragmentInPointShadow(const math::HitEntry& hitEntry, const glm::vec3& pointPos);
    private:
        // Directional Light
        std::unique_ptr<light::DirectionalLight> m_direcLight;

        // RenderObjects
        std::vector<std::unique_ptr<RenderPlaneObj>> m_renderPlanes;
        std::vector<std::unique_ptr<RenderSphereObj>> m_renderSpheres;
        std::vector<std::unique_ptr<RenderMeshObj>> m_renderMeshes;
        std::vector<std::unique_ptr<RenderPointLightObj>> m_pointLights;
        std::vector<std::unique_ptr<RenderSpotLightObj>> m_spotLights;

        // Shared mesh storage as follows: name, mesh
        std::unordered_map<std::string, mesh::Mesh> m_meshes;
        // Current object data that is dragged
        HitDraggerQuery m_dragBindedObject;
        // Camera position(useful for shadow calculation)
        glm::vec3 m_camPos;
    };

} // engn