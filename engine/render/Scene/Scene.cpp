#include "Scene.h"


Scene::Scene():
    m_lightPosition{ 0, 0, -10 }
{
}


void Scene::m_calculateLight(int objIdx, const HitEntry& hitEntry, COLORREF* pixel) {

    COLORREF hit_color;

    if (hitEntry.isHit) {
        // Check if shadowed
        bool isInShadow = false;
        ray toLight{ hitEntry.hitPoint + 0.001f * (m_lightPosition - hitEntry.hitPoint), m_lightPosition - hitEntry.hitPoint};

        for (size_t i = 0; i < m_objects.size() - 1; ++i) {
            auto collisionLog = m_objects[i]->hit(toLight);
            if (collisionLog.isHit && collisionLog.rayT > 0) {
                isInShadow = true;
                break;
            }
        }
        // Compute light if not
        if (!isInShadow) {
            float lightPower = glm::clamp(
                glm::dot(
                    hitEntry.hitNormal,
                    glm::normalize(m_lightPosition - hitEntry.hitPoint)
                ),
                0.2f, 1.0f
            );
            hit_color = RGBtoBE(RGB(
                GetRValue(m_objects[objIdx]->m_color) * lightPower,
                GetGValue(m_objects[objIdx]->m_color) * lightPower,
                GetBValue(m_objects[objIdx]->m_color) * lightPower)
            );
        }
        //! If shadow => shade
        else {
            hit_color = RGBtoBE(RGB(
                GetRValue(m_objects[objIdx]->m_color) * 0.1f,
                GetGValue(m_objects[objIdx]->m_color) * 0.1f,
                GetBValue(m_objects[objIdx]->m_color) * 0.1f)
            );
        }
    } else {
        hit_color = SKY_COLOR;
    }

    *pixel = hit_color;
}


void Scene::m_castRay(ray r, COLORREF* pixel) {
 
    // Find the object closest to the ray
    HitEntry closestEntry = m_objects[0]->hit(r);

    int closestObjIdx = 0;
    for (size_t i = 1; i < m_objects.size(); ++i) {
        //std::cout << glm::to_string(r.getOrigin()) << " " << glm::to_string(r.getDirection()) << std::endl;
        auto collisionLog = m_objects[i]->hit(r);
        if (collisionLog.isHit && collisionLog.rayT > 0 && collisionLog.rayT < closestEntry.rayT) {
            closestEntry = collisionLog;
            closestObjIdx = i;
        }
    }

    m_calculateLight(closestObjIdx, closestEntry, pixel);
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

    //std::cout << "Pixel W/H: " << rayCastData.pixelWidth << " " << rayCastData.pixelWidth << std::endl;
    //std::cout << "Stride W/H: " << rayCastData.strideX << " " << rayCastData.strideY << std::endl;

    camPtr->setRayCastData(rayCastData);

    auto* pixel = static_cast<COLORREF*>(winData.screenBuffer);
    for (int y = 0; y < winData.bufferHeight; ++y)
    {
        for (int x = 0; x < winData.bufferWidth; ++x)
        {
            ray r = camPtr->castRay(x, y);
            m_castRay(r, pixel);
            pixel++;
        }
    }
}

