#include "Scene.h"


namespace engn {

Scene::Scene() :
    m_lightPosition{ 0, 0, 10 }
{
}


void Scene::m_getObjectColor(const ClosestObj &closestObj, const math::HitEntry& hitEntry, COLORREF* pixel, const glm::vec3& camPos) {

    COLORREF hit_color;

    if (hitEntry.isHit) {
        auto objColor = m_getObjectLighting(closestObj, hitEntry, camPos);
        objColor *= 255;

        hit_color = RGBtoBE(RGB(
            objColor.x,
            objColor.y,
            objColor.z));

    }
    else {
        hit_color = SKY_COLOR;
    }

    *pixel = hit_color;
}


glm::vec3 Scene::m_getObjectLighting(const ClosestObj& closestObj, const math::HitEntry& hitEntry, const glm::vec3& camPos) {
    glm::vec3 totalLight{ 0.0f };

    bool isInShadow = m_isFragmentInDirectionShadow(hitEntry, glm::normalize(-m_direcLight->direction));

    // Directional
    if (!isInShadow) {
        totalLight += light::calculateDirLight(
            m_direcLight.get(),
            (closestObj.isMesh) ? &(m_renderMeshObjects[closestObj.objIdx]->material): &(m_renderMathObjects[closestObj.objIdx]->material),
            hitEntry.hitNormal,
            glm::normalize(camPos - hitEntry.hitPoint)
        );
    }
    // Point
    for (size_t i = 0; i < m_pointLights.size(); ++i) {
        isInShadow = m_isFragmentInPointShadow(hitEntry, m_pointLights[i]->position);
        if (!isInShadow) {
            totalLight += light::calculatePointLight(
                m_pointLights[i].get(),
                (closestObj.isMesh) ? &(m_renderMeshObjects[closestObj.objIdx]->material) : &(m_renderMathObjects[closestObj.objIdx]->material),
                hitEntry.hitNormal,
                glm::normalize(camPos - hitEntry.hitPoint),
                hitEntry.hitPoint
            );
        }
        isInShadow = false;
    }
    // Spot
    for (size_t i = 0; i < m_spotLights.size(); ++i) {
        isInShadow = m_isFragmentInPointShadow(hitEntry, m_spotLights[i]->position);
        if (!isInShadow) {
            totalLight += light::calculateSpotLight(
                m_spotLights[i].get(),
                (closestObj.isMesh) ? &(m_renderMeshObjects[closestObj.objIdx]->material) : &(m_renderMathObjects[closestObj.objIdx]->material),
                hitEntry.hitNormal,
                glm::normalize(camPos - hitEntry.hitPoint),
                hitEntry.hitPoint
            );
        }
    }

    totalLight = glm::clamp(totalLight, 0.0f, 1.0f);

    return totalLight;
}


bool Scene::m_isFragmentInDirectionShadow(const math::HitEntry& hitEntry, const glm::vec3& lightDir) {
    math::ray toLight{ hitEntry.hitPoint + 0.001f * (hitEntry.hitNormal), lightDir };

    math::HitEntry prop{false, math::hitable::MAX_DIST};
    // Math objects
    for (size_t i = 0; i < m_renderMathObjects.size(); ++i) {
        if (m_renderMathObjects[i]->shape->hit(toLight, prop)) {
            return true;
        }
    }
    // Meshes
    for (size_t i = 0; i < m_renderMeshObjects.size(); ++i) {
        auto prevOrigin = toLight.origin;
        toLight.transform(m_renderMeshObjects[i]->modelMatrixInv);

        auto collisionLog = m_renderMeshObjects[i]->mesh->hit(toLight);
        if (m_renderMeshObjects[i]->collideOcTree.intersect(toLight, prop)) {
            return true;
        }

        toLight.origin = prevOrigin;
    }
    return false;
}


bool Scene::m_isFragmentInPointShadow(const math::HitEntry& hitEntry, const glm::vec3& pointPos) {
    glm::vec3 distToLight = pointPos - hitEntry.hitPoint;
    math::ray toLight{ hitEntry.hitPoint + 0.001f * (hitEntry.hitNormal), glm::normalize(distToLight) };

    math::HitEntry prop{ false, math::hitable::MAX_DIST };
    // Math objects
    for (size_t i = 0; i < m_renderMathObjects.size(); ++i) {
        if (m_renderMathObjects[i]->shape->hit(toLight, prop) && glm::length(prop.hitPoint - hitEntry.hitPoint) < glm::length(distToLight)) {
            return true;
        }
    }
    // Meshes
    for (size_t i = 0; i < m_renderMeshObjects.size(); ++i) {
        auto prevOrigin = toLight.origin;
        toLight.transform(m_renderMeshObjects[i]->modelMatrixInv);

        if (m_renderMeshObjects[i]->collideOcTree.intersect(toLight, prop) && glm::length(prop.hitPoint - hitEntry.hitPoint) < glm::length(distToLight)) {
            return true;
        }
        toLight.origin = prevOrigin;
    }

    return false;
}


void Scene::m_castRay(math::ray& r, COLORREF* pixel, const glm::vec3& camPos) {

    // Find the object closest to the ray
    math::HitEntry closestEntry{false, math::hitable::MAX_DIST};

    ClosestObj closestObj{0, false};

    bool isClosestHitALight = false;
    //! Check object collision
    // Math objects
    for (size_t i = 0; i < m_renderMathObjects.size(); ++i) {
        if (m_renderMathObjects[i]->shape->hit(r, closestEntry)) {
            closestObj.objIdx = i;
        }
    }

    // Check mesh intersection -> translate vector in mesh modelspace
    for (size_t i = 0; i < m_renderMeshObjects.size(); ++i) {
        auto prevRayOrigin = r.origin;
        r.transform(m_renderMeshObjects[i]->modelMatrixInv);

        math::HitEntry collisionLog;
        if (m_renderMeshObjects[i]->collideOcTree.intersect(r, closestEntry)) {
            // Yes
            closestEntry.hitPoint = glm::vec3(m_renderMeshObjects[i]->modelMatrix * glm::vec4(closestEntry.hitPoint, 1.0f));
            closestObj.objIdx = i;
            closestObj.isMesh = true;
        }

        r.origin = prevRayOrigin;
    }

    //! Check light collision(just so we can draw positions)
    // Begin with spotlight
    bool closestHitSpecular = false;
    for (size_t i = 0; i < m_spotLights.size(); ++i) {
        if (m_spotLights[i]->shape->hit(r, closestEntry)) {
            if (!isClosestHitALight) { isClosestHitALight = true; }
            closestObj.objIdx = i;
            closestHitSpecular = true;
        }
    }
    // Point lights after
    for (size_t i = 0; i < m_pointLights.size(); ++i) {
        if (m_pointLights[i]->shape->hit(r, closestEntry)) {
            if (!isClosestHitALight) { isClosestHitALight = true; }
            closestObj.objIdx = i;
        }
    }

    if (isClosestHitALight) {
        glm::vec3 lightColor{ 255.0f };
        if (closestHitSpecular) {
            lightColor *= m_spotLights[closestObj.objIdx]->properties.specular;
        }
        else {
            lightColor *= m_pointLights[closestObj.objIdx]->properties.specular;
        }
        *pixel = RGBtoBE(RGB(
            lightColor.x,
            lightColor.y,
            lightColor.z));
        return;
    }

    m_getObjectColor(closestObj, closestEntry, pixel, camPos);
}


void Scene::render(const WindowRenderData& winData, std::unique_ptr<Camera>& camPtr)

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
        winData.screenWidth / winData.bufferWidth,
        winData.screenHeight / winData.bufferHeight
    };

    camPtr->setRayCastData(std::move(rayCastData));

    auto* pixel = static_cast<COLORREF*>(winData.screenBuffer);
    auto* st_p = pixel;
    for (int y = 0; y < winData.bufferHeight; ++y)
    {
        for (int x = 0; x <= winData.bufferWidth; ++x)
        {
            math::ray r = camPtr->castRay(x, y);
            m_castRay(r, pixel, camPtr->getCamPosition());
            pixel = st_p + y * winData.bufferWidth + x;
        }
    }
}

} // engn