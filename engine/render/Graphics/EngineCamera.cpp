#include <iostream>

#include "EngineCamera.hpp"


namespace engn {
    namespace rend {
        EngineCamera::EngineCamera(float fov, int screenWidth, int screenHeight, const XMFLOAT3& position) :
            m_position{ position }
        {
            m_positionVec = XMLoadFloat3(&m_position);
            m_positionVec = XMVectorSetW(m_positionVec, 1.0f);
            setProjectionMatrix(fov, screenWidth, screenHeight);
            updateViewMatrix();
        }

        void EngineCamera::setNewScreenSize(int width, int height) {
            setProjectionMatrix(m_fov, width, height);
        }

        void EngineCamera::setProjectionMatrix(float fov, int width, int height) {
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
    } // rend
} // engn