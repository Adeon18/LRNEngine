#include <iostream>

#include <windows.h>

#include "EngineCamera.hpp"


namespace engn {
    namespace rend {
        EngineCamera::EngineCamera(float fov, int screenWidth, int screenHeight, const XMFLOAT3& position) :
            m_position{ position },
            m_screenWidth{ screenWidth },
            m_screenHeight{ screenHeight }
        {
            m_positionVec = XMLoadFloat3(&m_position);
            m_positionVec = XMVectorSetW(m_positionVec, 1.0f);
            setProjectionMatrix(fov, screenWidth, screenHeight);
            updateViewMatrix();
        }

        void EngineCamera::setNewScreenSize(int width, int height) {
            m_screenWidth = width;
            m_screenHeight = height;
            setProjectionMatrix(m_fov, width, height);
        }

        void EngineCamera::setProjectionMatrix(float fov, int width, int height) {
            // TODO: Arguments can be removed + reversed matrix
            m_fov = fov;
            m_projection = XMMatrixPerspectiveFovLH(
                XMConvertToRadians(m_fov),
                static_cast<float>(width) / static_cast<float>(height),
                1000.0f, 0.1f);
            m_projectionReversed = XMMatrixPerspectiveFovLH(
                XMConvertToRadians(m_fov),
                static_cast<float>(width) / static_cast<float>(height),
                0.1f, 1000.0f);
        }

        void EngineCamera::setPosition(const XMVECTOR& pos) {
            XMStoreFloat3(&m_position, pos);
            m_positionVec = pos;
            updateViewMatrixPos();
        }

        void EngineCamera::addWorldOffset(const XMVECTOR& offset) {
            m_positionVec += offset;
            m_positionVec = XMVectorSetW(m_positionVec, 1.0f);
            XMStoreFloat3(&m_position, m_positionVec);
            updateViewMatrixPos();
        }

        void EngineCamera::addRelativeOffset(const XMVECTOR& offset) {
            m_positionVec +=
                XMVectorGetX(offset) * getCamRight() +
                XMVectorGetY(offset) * getCamUp() +
                XMVectorGetZ(offset) * getCamForward();
            m_positionVec = XMVectorSetW(m_positionVec, 1.0f);
            XMStoreFloat3(&m_position, m_positionVec);
            updateViewMatrixPos();
        }

        void EngineCamera::addWorldRotationQuat(const XMVECTOR& angles) {
            m_rotationQuat = XMQuaternionMultiply(m_rotationQuat, XMQuaternionRotationNormal(DEF_UP_VECTOR, XMConvertToRadians(XMVectorGetY(angles))));
            m_rotationQuat = XMQuaternionMultiply(m_rotationQuat, XMQuaternionRotationNormal(DEF_RIGHT_VECTOR, XMConvertToRadians(XMVectorGetX(angles))));
            m_rotationQuat = XMQuaternionMultiply(m_rotationQuat, XMQuaternionRotationNormal(DEF_FORWARD_VECTOR, XMConvertToRadians(XMVectorGetZ(angles))));
            m_rotationQuat = XMQuaternionNormalize(m_rotationQuat);
            updateViewMatrix();
        }

        void EngineCamera::addRelativeRotationQuat(const XMVECTOR& angles) {
            m_rotationQuat = XMQuaternionMultiply(m_rotationQuat, XMQuaternionRotationAxis(getCamUp(), XMConvertToRadians(XMVectorGetY(angles))));
            m_rotationQuat = XMQuaternionMultiply(m_rotationQuat, XMQuaternionRotationAxis(getCamRight(), XMConvertToRadians(XMVectorGetX(angles))));
            m_rotationQuat = XMQuaternionMultiply(m_rotationQuat, XMQuaternionRotationAxis(getCamForward(), XMConvertToRadians(XMVectorGetZ(angles))));
            m_rotationQuat = XMQuaternionNormalize(m_rotationQuat);
            updateViewMatrix();
        }

        void EngineCamera::updateViewMatrix() {
            m_viewInv = XMMatrixRotationQuaternion(m_rotationQuat);
            m_viewInv.r[3] = m_positionVec;
            m_view = XMMatrixInverse(nullptr, m_viewInv);
        }

        void EngineCamera::updateViewMatrixPos() {
            m_viewInv.r[3] = m_positionVec;
            m_view = XMMatrixInverse(nullptr, m_viewInv);
        }

        // TODO: Clean, maybe vars to mems
        geom::Ray EngineCamera::castRay(float x, float y) {

            XMMATRIX viewProjInv = XMMatrixInverse(nullptr, m_view * m_projectionReversed);
            for (uint32_t i = 0; i < 4; ++i) {
                m_viewingFrustumNearPlaneWorldSpace[i] = XMVector3Transform(m_viewingFrustumNearPlane[i], viewProjInv);
                m_viewingFrustumNearPlaneWorldSpace[i] /= XMVectorGetW(m_viewingFrustumNearPlaneWorldSpace[i]);
            }
            XMVECTOR BLPlanePos = m_viewingFrustumNearPlaneWorldSpace[0];
            XMVECTOR BLToTL = XMVector3Normalize(m_viewingFrustumNearPlaneWorldSpace[1] - BLPlanePos);
            XMVECTOR BLToBR = XMVector3Normalize(m_viewingFrustumNearPlaneWorldSpace[3] - BLPlanePos);
            //std::cout << "BLPlanePos: " << BLPlanePos << " BLToTL: " << BLToTL << " BLToBR: " << BLToBR << std::endl;

            float nearPlaneWidth = std::abs(XMVectorGetX(XMVector3Length(m_viewingFrustumNearPlaneWorldSpace[3] - m_viewingFrustumNearPlaneWorldSpace[0])));
            float nearPlaneHeight = std::abs(XMVectorGetY(XMVector3Length(m_viewingFrustumNearPlaneWorldSpace[2] - m_viewingFrustumNearPlaneWorldSpace[3])));

            float pixelWidth = nearPlaneWidth / m_screenWidth;
            float pixelHeight = nearPlaneHeight / m_screenHeight;

            //std::cout << "ScreenWidth: " << nearPlaneWidth << " ScreenHeight: " << nearPlaneHeight << std::endl;
            //std::cout << "PixelWidth: " << pixelWidth << " PixelHeight: " << pixelHeight << std::endl;

            XMVECTOR rayPos = getCamPosition();
            XMVECTOR rayTo = BLPlanePos +
                BLToTL * (pixelHeight / 2.0f) + BLToTL * pixelHeight * (m_screenHeight - y) +
                BLToBR * (pixelWidth / 2.0f) + BLToBR * pixelWidth * x;
            XMVECTOR rayDir = XMVector3Normalize(rayTo - rayPos);
            //std::cout << "RayPos: " << rayPos << " RayTo: " << rayTo << " RayDir: " << rayDir << std::endl;

            return geom::Ray{ rayPos, rayDir };
        }
    } // rend
} // engn