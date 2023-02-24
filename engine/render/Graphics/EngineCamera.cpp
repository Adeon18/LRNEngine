#include <iostream>

#include <windows.h>

#include "EngineCamera.hpp"


namespace engn {
    namespace rend {
        EngineCamera::EngineCamera(float fov, int screenWidth, int screenHeight, const XMFLOAT3& position) :
            m_position{ position },
            m_rotation{ 0.0f, 0.0f, 0.0f },
            m_screenWidth{ screenWidth },
            m_screenHeight{ screenHeight }
        {
            m_positionVec = XMLoadFloat3(&m_position);
            m_rotationVec = XMLoadFloat3(&m_rotation);
            setProjectionMatrix(fov, screenWidth, screenHeight);
            updateViewMatrix();

        }

        void EngineCamera::setNewScreenSize(int width, int height) {
            m_screenWidth = width;
            m_screenHeight = height;
            setProjectionMatrix(m_fov, width, height);
        }

        void EngineCamera::setProjectionMatrix(float fov, int width, int height) {
            m_fov = fov;
            m_projection = XMMatrixPerspectiveFovLH(
                XMConvertToRadians(m_fov),
                static_cast<float>(width) / static_cast<float>(height),
                1000.0f, 0.1f);
        }

        void EngineCamera::setPosition(const XMVECTOR& pos) {
            XMStoreFloat3(&m_position, pos);
            m_positionVec = pos;
            updateViewMatrix();
        }

        void EngineCamera::addWorldOffset(const XMVECTOR& offset) {
            m_positionVec += offset;
            m_positionVec = XMVectorSetW(m_positionVec, 1.0f);
            XMStoreFloat3(&m_position, m_positionVec);
            updateViewMatrix();
        }

        void EngineCamera::addRelativeOffset(const XMVECTOR& offset) {
            m_positionVec +=
                XMVectorGetX(offset) * getCamRight() +
                XMVectorGetY(offset) * getCamUp() +
                XMVectorGetZ(offset) * getCamForward();
            m_positionVec = XMVectorSetW(m_positionVec, 1.0f);
            XMStoreFloat3(&m_position, m_positionVec);
            updateViewMatrix();
        }

        void EngineCamera::addWorldRotationMat(const XMVECTOR& angles) {
            m_rotationVec += angles;
            if (XMVectorGetX(m_rotationVec) > XM_PIDIV2) {
                m_rotationVec = XMVectorSetX(m_rotationVec, XM_PIDIV2);
            } else if (XMVectorGetX(m_rotationVec) < -XM_PIDIV2) {
                m_rotationVec = XMVectorSetX(m_rotationVec, -XM_PIDIV2);
            }

            XMStoreFloat3(&m_rotation, m_rotationVec);
            updateViewMatrix();
        }

        void EngineCamera::updateViewMatrix() {
            // Get the simple rotation matrix NOT via quaternion
            XMMATRIX rotMatrix = XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
            // Get the transformed forward vector by position and rotation
            XMVECTOR forwardVec = XMVector3Transform(DEF_FORWARD_VECTOR, rotMatrix);
            forwardVec += m_positionVec;

            m_upVec = XMVector3Transform(DEF_UP_VECTOR, rotMatrix);

            m_view = XMMatrixLookAtLH(m_positionVec, forwardVec, m_upVec);
            m_viewT = XMMatrixTranspose(m_view);
        }

        geom::Ray EngineCamera::castRay(float x, float y) {

            XMMATRIX viewProjInv = XMMatrixInverse(nullptr, m_view * m_projection);
            for (uint32_t i = 0; i < 4; ++i) {
                m_viewingFrustumNearPlaneWorldSpace[i] = XMVector3Transform(m_viewingFrustumNearPlane[i], viewProjInv);
                m_viewingFrustumNearPlaneWorldSpace[i] /= XMVectorGetW(m_viewingFrustumNearPlaneWorldSpace[i]);
            }
            XMVECTOR BLPlanePos = m_viewingFrustumNearPlaneWorldSpace[0];
            XMVECTOR BLToTL = XMVector3Normalize(m_viewingFrustumNearPlaneWorldSpace[1] - BLPlanePos);
            XMVECTOR BLToBR = XMVector3Normalize(m_viewingFrustumNearPlaneWorldSpace[3] - BLPlanePos);

            float nearPlaneWidth = std::abs(XMVectorGetX(XMVector3Length(m_viewingFrustumNearPlaneWorldSpace[3] - m_viewingFrustumNearPlaneWorldSpace[0])));
            float nearPlaneHeight = std::abs(XMVectorGetY(XMVector3Length(m_viewingFrustumNearPlaneWorldSpace[2] - m_viewingFrustumNearPlaneWorldSpace[3])));

            float pixelWidth = nearPlaneWidth / m_screenWidth;
            float pixelHeight = nearPlaneHeight / m_screenHeight;

            XMVECTOR rayPos = getCamPosition();
            XMVECTOR rayTo = BLPlanePos +
                BLToTL * (pixelHeight / 2.0f) + BLToTL * pixelHeight * (m_screenHeight - y) +
                BLToBR * (pixelWidth / 2.0f) + BLToBR * pixelWidth * x;
            XMVECTOR rayDir = XMVector3Normalize(rayTo - rayPos);

            return geom::Ray{ rayPos, rayDir };
        }
    } // rend
} // engn