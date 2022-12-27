#include "Scene.h"


namespace engn {

Scene::Scene() :
    m_lightPosition{ 0, 0, 10 }
{
}


void Scene::m_getObjectColor(int objIdx, const math::HitEntry& hitEntry, COLORREF* pixel, const glm::vec3& camPos) {

    COLORREF hit_color;

    if (hitEntry.isHit) {
        auto objColor = m_getObjectLighting(objIdx, hitEntry, camPos);
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


glm::vec3 Scene::m_getObjectLighting(int objIdx, const math::HitEntry& hitEntry, const glm::vec3& camPos) {
    glm::vec3 totalLight{ 0.0f };

    bool isInShadow = m_isFragmentInDirectionShadow(hitEntry, glm::normalize(-m_direcLight->direction));

    if (!isInShadow) {
        totalLight += light::calculateDirLight(
            m_direcLight.get(),
            &(m_renderObjects[objIdx]->material),
            hitEntry.hitNormal,
            glm::normalize(camPos - hitEntry.hitPoint)
        );
    }

    for (size_t i = 0; i < m_pointLights.size(); ++i) {
        isInShadow = m_isFragmentInPointShadow(hitEntry, m_pointLights[i]->position);
        if (!isInShadow) {
            totalLight += light::calculatePointLight(
                m_pointLights[i].get(),
                &(m_renderObjects[objIdx]->material),
                hitEntry.hitNormal,
                glm::normalize(camPos - hitEntry.hitPoint),
                hitEntry.hitPoint
            );
        }
        isInShadow = false;
    }
    isInShadow = m_isFragmentInPointShadow(hitEntry, m_spotLight->position);
    if (!isInShadow) {
        totalLight += light::calculateSpotLight(
            m_spotLight.get(),
            &(m_renderObjects[objIdx]->material),
            hitEntry.hitNormal,
            glm::normalize(camPos - hitEntry.hitPoint),
            hitEntry.hitPoint
        );
    }

    totalLight = glm::clamp(totalLight, 0.0f, 1.0f);

    return totalLight;
}


bool Scene::m_isFragmentInDirectionShadow(const math::HitEntry& hitEntry, const glm::vec3& lightDir) {
    math::ray toLight{ hitEntry.hitPoint + 0.001f * (hitEntry.hitNormal), lightDir };

    for (size_t i = 0; i < m_renderObjects.size(); ++i) {
        auto collisionLog = m_renderObjects[i]->shape->hit(toLight);
        if (collisionLog.isHit && collisionLog.rayT > 0) {
            return true;
        }
    }
    return false;
}


bool Scene::m_isFragmentInPointShadow(const math::HitEntry& hitEntry, const glm::vec3& pointPos) {
    glm::vec3 distToLight = pointPos - hitEntry.hitPoint;
    math::ray toLight{ hitEntry.hitPoint + 0.001f * (hitEntry.hitNormal), glm::normalize(distToLight) };

    for (size_t i = 0; i < m_renderObjects.size(); ++i) {
        auto collisionLog = m_renderObjects[i]->shape->hit(toLight);
        if (collisionLog.isHit && collisionLog.rayT > 0 && glm::length(collisionLog.hitPoint - hitEntry.hitPoint) < glm::length(distToLight)) {
            return true;
        }
    }
    return false;
}


void Scene::m_castRay(const math::ray& r, COLORREF* pixel, const glm::vec3& camPos) {

    // Find the object closest to the ray
    math::HitEntry closestEntry = m_renderObjects[0]->shape->hit(r);

    bool isClosestHitALight = false;
    int closestObjIdx = 0;
    //! Check object collision
    for (size_t i = 1; i < m_renderObjects.size(); ++i) {
        //std::cout << glm::to_string(r.getOrigin()) << " " << glm::to_string(r.getDirection()) << std::endl;
        auto collisionLog = m_renderObjects[i]->shape->hit(r);
        if (collisionLog.isHit && collisionLog.rayT > 0 && collisionLog.rayT < closestEntry.rayT) {
            closestEntry = collisionLog;
            closestObjIdx = i;
        }
    }

    //! Check light collision(just so we can draw positions)
    // Begin with spotlight
    auto collisionLog = m_spotLight->shape->hit(r);
    if (collisionLog.isHit && collisionLog.rayT > 0 && collisionLog.rayT < closestEntry.rayT) {
        if (!isClosestHitALight) { isClosestHitALight = true; }
        closestObjIdx = -1;
    }
    // Point lights after
    for (size_t i = 0; i < m_pointLights.size(); ++i) {
        collisionLog = m_pointLights[i]->shape->hit(r);
        if (collisionLog.isHit && collisionLog.rayT > 0 && collisionLog.rayT < closestEntry.rayT) {
            if (!isClosestHitALight) { isClosestHitALight = true; }
            closestObjIdx = i;
        }
    }

    if (isClosestHitALight) {
        glm::vec3 lightColor{ 255.0f };
        if (closestObjIdx < 0) {
            lightColor *= m_spotLight->properties.specular;
        }
        else {
            lightColor *= m_pointLights[closestObjIdx]->properties.specular;
        }
        *pixel = RGBtoBE(RGB(
            lightColor.x,
            lightColor.y,
            lightColor.z));
        return;
    }

    m_getObjectColor(closestObjIdx, closestEntry, pixel, camPos);
}


void Scene::render(const WindowRenderData& winData, std::unique_ptr<Camera>& camPtr)

{
    // Don't render the minimized window
    if (winData.screenWidth == 0 || winData.screenHeight == 0) {
        return;
    }

    RayCastData rayCastData{
        static_cast<double>(glm::length(camPtr->getBRVec())) / static_cast<double>(winData.screenWidth),
        static_cast<double>(glm::length(camPtr->getTLVec())) / static_cast<double>(winData.screenHeight),
        winData.screenWidth / winData.bufferWidth,
        winData.screenHeight / winData.bufferHeight
    };

    camPtr->setRayCastData(std::move(rayCastData));

    auto* pixel = static_cast<COLORREF*>(winData.screenBuffer);
    auto* st_p = pixel;
    for (int y = 0; y < winData.bufferHeight; ++y)
    {
        for (int x = 0; x < winData.bufferWidth; ++x)
        {
            math::ray r = camPtr->castRay(x, y);
            m_castRay(r, pixel, camPtr->getCamPosition());
            pixel = st_p + y * winData.bufferWidth + x;
        }
    }
}

} // engn