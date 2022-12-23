#include "Scene.h"


Scene::Scene():
    m_lightPosition{ 0, 0, 10 }
{
}


void Scene::m_getObjectColor(int objIdx, const HitEntry& hitEntry, COLORREF* pixel, const glm::vec3& camPos) {

    COLORREF hit_color;

    if (hitEntry.isHit) {
        auto objColor = m_getObjectLighting(objIdx, hitEntry, camPos);
        objColor *= 255;
        //glm::clamp(objColor, 0.0f, 255.0f);
        //if (objColor.y < 10 && objColor.y < objColor.x)
        //    std::cout << glm::to_string(objColor) << std::endl;
        hit_color = RGBtoBE(RGB(
                    objColor.x,
                    objColor.y,
                    objColor.z));
        //// Check if shadowed
        //bool isInShadow = false;
        //ray toLight{ hitEntry.hitPoint + 0.001f * (m_lightPosition - hitEntry.hitPoint), m_lightPosition - hitEntry.hitPoint};

        //for (size_t i = 0; i < m_objects.size() - 1; ++i) {
        //    auto collisionLog = m_objects[i]->hit(toLight);
        //    if (collisionLog.isHit && collisionLog.rayT > 0) {
        //        isInShadow = true;
        //        break;
        //    }
        //}
        //// Compute light if not
        //if (!isInShadow) {
        //    float lightPower = glm::clamp(
        //        glm::dot(
        //            hitEntry.hitNormal,
        //            glm::normalize(m_lightPosition - hitEntry.hitPoint)
        //        ),
        //        0.2f, 1.0f
        //    );
        //    hit_color = RGBtoBE(RGB(
        //        GetRValue(m_objects[objIdx]->m_color) * lightPower,
        //        GetGValue(m_objects[objIdx]->m_color) * lightPower,
        //        GetBValue(m_objects[objIdx]->m_color) * lightPower)
        //    );
        //}
        ////! If shadow => shade
        //else {
        //    hit_color = RGBtoBE(RGB(
        //        GetRValue(m_objects[objIdx]->m_color) * 0.1f,
        //        GetGValue(m_objects[objIdx]->m_color) * 0.1f,
        //        GetBValue(m_objects[objIdx]->m_color) * 0.1f)
        //    );
        //}
    } else {
        hit_color = SKY_COLOR;
    }

    *pixel = hit_color;
}


glm::vec3 Scene::m_getObjectLighting(int objIdx, const HitEntry& hitEntry, const glm::vec3& camPos) {
    glm::vec3 totalLight{0.0f};

    totalLight += light::calculateDirLight(m_direcLight.get(), &(m_renderObjects[objIdx]->material), hitEntry.hitNormal, glm::normalize(camPos - hitEntry.hitPoint));

    return totalLight;
}


void Scene::m_castRay(const ray& r, COLORREF* pixel, const glm::vec3& camPos) {
 
    // Find the object closest to the ray
    HitEntry closestEntry = m_renderObjects[0]->shape->hit(r);

    int closestObjIdx = 0;
    for (size_t i = 1; i < m_renderObjects.size(); ++i) {
        //std::cout << glm::to_string(r.getOrigin()) << " " << glm::to_string(r.getDirection()) << std::endl;
        auto collisionLog = m_renderObjects[i]->shape->hit(r);
        if (collisionLog.isHit && collisionLog.rayT > 0 && collisionLog.rayT < closestEntry.rayT) {
            closestEntry = collisionLog;
            closestObjIdx = i;
        }
    }
    // Here should be light object rendeer and an if statement(we don't shade light objects)
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
            ray r = camPtr->castRay(x, y);
            m_castRay(r, pixel, camPtr->getCamPosition());
            pixel = st_p + y * winData.bufferWidth + x;
        }
    }
}

