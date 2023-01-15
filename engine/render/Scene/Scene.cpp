#include "Scene.h"


namespace engn {

    void Scene::m_castRay(math::ray& r, COLORREF* pixel) {

        // Find the object closest to the ray
        math::HitEntry closestEntry{};
        ObjRef closestObj;

        //! Check object collision
        // plane
        for (auto& plane : m_renderPlanes) {
            plane->hit(r, closestEntry, closestObj);
        }
        // sphere
        for (auto& sphere : m_renderSpheres) {
            sphere->hit(r, closestEntry, closestObj);
        }
        // mesh
        for (auto& mesh : m_renderMeshes) {
            mesh->hit(r, closestEntry, closestObj);
        }
        // pointlight
        for (auto& light : m_pointLights) {
            light->hit(r, closestEntry, closestObj);
        }
        // spotlight
        for (auto& light : m_spotLights) {
            light->hit(r, closestEntry, closestObj);
        }

        m_getObjectColor(closestObj, closestEntry, pixel);
    }


    void Scene::m_getObjectColor(const ObjRef& closestObj, const math::HitEntry& hitEntry, COLORREF* pixel) {
        glm::vec3 lightColor{ 255.0f };

        switch (closestObj.type) {
        // pointlight
        case RenderType::POINTLIGHT: {
            RenderPointLightObj* obj = static_cast<RenderPointLightObj*>(closestObj.object);
            lightColor *= obj->getLight()->properties.specular;
            break;
        }
        // spotlight
        case RenderType::SPOTLIGHT: {
            RenderSpotLightObj* obj = static_cast<RenderSpotLightObj*>(closestObj.object);
            lightColor *= obj->getLight()->properties.specular;
            break;
        }
        // Render Objects
        case RenderType::SPHERE: case RenderType::PLANE: case RenderType::MESH: {
            lightColor *= m_getObjectLighting(closestObj, hitEntry);
            break;
        }
        // sky
        default:
            lightColor *= light::SKY_COLOR;
            break;
        }

        *pixel = vecToCOLORREF(lightColor);
    }


    glm::vec3 Scene::m_getObjectLighting(const ObjRef& closestObj, const math::HitEntry& hitEntry) {
        glm::vec3 totalLight{ 0.0f };

        bool isInShadow = m_isFragmentInDirectionShadow(hitEntry, glm::normalize(-m_direcLight->direction));

        // Directional
        if (!isInShadow) {
            totalLight += light::calculateDirLight(
                m_direcLight.get(),
                closestObj.material,
                hitEntry.hitNormal,
                glm::normalize(m_camPos - hitEntry.hitPoint)
            );
        }
        // Point
        for (size_t i = 0; i < m_pointLights.size(); ++i) {
            isInShadow = m_isFragmentInPointShadow(hitEntry, m_pointLights[i]->getLight()->position);
            if (!isInShadow) {
                totalLight += light::calculatePointLight(
                    m_pointLights[i]->getLight(),
                    closestObj.material,
                    hitEntry.hitNormal,
                    glm::normalize(m_camPos - hitEntry.hitPoint),
                    hitEntry.hitPoint
                );
            }
            isInShadow = false;
        }
        // Spot
        for (size_t i = 0; i < m_spotLights.size(); ++i) {
            isInShadow = m_isFragmentInPointShadow(hitEntry, m_spotLights[i]->getLight()->position);
            if (!isInShadow) {
                totalLight += light::calculateSpotLight(
                    m_spotLights[i]->getLight(),
                    closestObj.material,
                    hitEntry.hitNormal,
                    glm::normalize(m_camPos - hitEntry.hitPoint),
                    hitEntry.hitPoint
                );
            }
        }

        totalLight = glm::clamp(totalLight, 0.0f, 1.0f);

        return totalLight;
    }


    bool Scene::m_isFragmentInDirectionShadow(const math::HitEntry& hitEntry, const glm::vec3& lightDir) {
        math::ray toLight{ hitEntry.hitPoint + 0.001f * (hitEntry.hitNormal), lightDir };

        ObjRef propRef;
        math::HitEntry prop{};

        // plane
        for (auto& plane : m_renderPlanes) {
            if (plane->hit(toLight, prop, propRef)) { return true; }
        }
        // sphere
        for (auto& sphere : m_renderSpheres) {
            if (sphere->hit(toLight, prop, propRef)) { return true; }
        }
        // mesh
        for (auto& mesh : m_renderMeshes) {
            if (mesh->hit(toLight, prop, propRef)) { return true; }
        }
        return false;
    }

    bool Scene::m_isFragmentInPointShadow(const math::HitEntry& hitEntry, const glm::vec3& pointPos) {
        glm::vec3 distToLight = pointPos - hitEntry.hitPoint;
        math::ray toLight{ hitEntry.hitPoint + 0.001f * (hitEntry.hitNormal), glm::normalize(distToLight) };

        ObjRef propRef;
        math::HitEntry prop{};

        // plane
        for (auto& plane : m_renderPlanes) {
            if (plane->hit(toLight, prop, propRef) && glm::length(prop.hitPoint - hitEntry.hitPoint) < glm::length(distToLight)) { return true; }
        }
        // sphere
        for (auto& sphere : m_renderSpheres) {
            if (sphere->hit(toLight, prop, propRef) && glm::length(prop.hitPoint - hitEntry.hitPoint) < glm::length(distToLight)) { return true; }
        }
        // mesh
        for (auto& mesh : m_renderMeshes) {
            if (mesh->hit(toLight, prop, propRef) && glm::length(prop.hitPoint - hitEntry.hitPoint) < glm::length(distToLight)) { return true; }
        }

        return false;
    }


    bool Scene::findDraggable(const glm::vec2& rayCastTo, std::unique_ptr<Camera>& camPtr) {
        math::ray r = camPtr->castRay(rayCastTo.x, rayCastTo.y);
        m_dragBindedObject.free();

        // plane
        for (auto& plane : m_renderPlanes) {
            plane->hit(r, m_dragBindedObject.hitEntry, m_dragBindedObject.objRef);
        }
        // sphere
        for (auto& sphere : m_renderSpheres) {
            sphere->hit(r, m_dragBindedObject.hitEntry, m_dragBindedObject.objRef);
        }
        // mesh
        for (auto& mesh : m_renderMeshes) {
            mesh->hit(r, m_dragBindedObject.hitEntry, m_dragBindedObject.objRef);
        }

        switch (m_dragBindedObject.objRef.type) {
        case RenderType::SPHERE:
        {
            RenderSphereObj* sphere = static_cast<RenderSphereObj*>(m_dragBindedObject.objRef.object);
            m_dragBindedObject.dragger = std::unique_ptr<math::IDragger>(
                new math::ISphereDragger(sphere, &m_dragBindedObject.hitEntry)
                );
            return true;
        }

        case RenderType::MESH:
        {
            RenderMeshObj* mesh = static_cast<RenderMeshObj*>(m_dragBindedObject.objRef.object);
            m_dragBindedObject.dragger = std::unique_ptr<math::IDragger>(new math::IMeshDragger(mesh, &m_dragBindedObject.hitEntry));
            return true;
        }
        }

        return false;
    }

    void Scene::moveDraggable(const glm::vec2& rayCastTo, std::unique_ptr<Camera>& camPtr) {
        math::ray newR = camPtr->castRay(rayCastTo.x, rayCastTo.y);
        math::plane planeOfPoint{ camPtr->getCamForward(), m_dragBindedObject.hitEntry.hitPoint };

        // Get the intersection point of the new ray and a plane in which the old point lies
        math::HitEntry newPointEntry{};
        if (planeOfPoint.hit(newR, newPointEntry)) {
            m_dragBindedObject.dragger->move(newPointEntry.hitPoint);
        }
    }

    void Scene::render(const WindowRenderData& winData, std::unique_ptr<Camera>& camPtr, std::unique_ptr<ParallelExecutor>& executor)

    {
        // Don't render the minimized window
        if (winData.screenWidth == 0 || winData.screenHeight == 0) {
            return;
        }

        float pixelWidth = glm::length(camPtr->getBRVec()) / static_cast<float>(winData.screenWidth);
        float pixelHeight = glm::length(camPtr->getTLVec()) / static_cast<float>(winData.screenHeight);
        RayCastData rayCastData{
            pixelWidth,
            pixelHeight,
            pixelWidth / 2.0f,
            pixelHeight / 2.0f,
            static_cast<float>(winData.screenWidth) / static_cast<float>(winData.bufferWidth),
            static_cast<float>(winData.screenHeight) / static_cast<float>(winData.bufferHeight)
        };

        camPtr->setRayCastData(std::move(rayCastData));
        m_setCameraPos(camPtr->getCamPosition());

        auto* pixel = static_cast<COLORREF*>(winData.screenBuffer);
        auto funcToExecute = [this, &winData, &pixel, &camPtr](uint32_t threadIndex, uint32_t taskIndex)
        {
            m_computePixelColor(taskIndex / winData.bufferWidth, taskIndex % winData.bufferWidth, pixel, winData, camPtr);
        };

        executor->execute(funcToExecute, winData.bufferWidth * winData.bufferHeight, 20);
    }

    void Scene::m_computePixelColor(int y, int x, COLORREF* pixel, const WindowRenderData& winData, std::unique_ptr<Camera>& camPtr) {
        math::ray r = camPtr->castRay(x, y);
        m_castRay(r, pixel + y * winData.bufferWidth + x);
    }

} // engn