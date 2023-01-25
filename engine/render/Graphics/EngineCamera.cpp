#include <iostream>

#include "EngineCamera.hpp"


namespace engn {
    namespace rend {
        EngineCamera::EngineCamera(float fov, int screenWidth, int screenHeight, const XMFLOAT3& position) :
            m_position{ position },
            m_rotation{0.0f, 0.0f, 0.0f}
        {
            m_positionVec = XMLoadFloat3(&m_position);
            m_positionVec = XMVectorSetW(m_positionVec, 1.0f);
            m_rotationVec = XMLoadFloat3(&m_rotation);
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

        void EngineCamera::setRotation(const XMVECTOR& rot) {
            XMStoreFloat3(&m_rotation, rot);
            m_rotationVec = rot;
            updateViewMatrix();
        }

        void EngineCamera::addWorldRotationMat(const XMVECTOR& angles) {
            m_rotationVec += angles;
            XMStoreFloat3(&m_rotation, m_rotationVec);
            updateViewMatrix();
        }

        void EngineCamera::addWorldRotationQuat(const XMVECTOR& angles) {
            m_rotationQuat = XMQuaternionMultiply(m_rotationQuat, XMQuaternionRotationAxis(DEF_UP_VECTOR, XMConvertToRadians(XMVectorGetY(angles))));
            m_rotationQuat = XMQuaternionMultiply(m_rotationQuat, XMQuaternionRotationAxis(DEF_RIGHT_VECTOR, XMConvertToRadians(XMVectorGetX(angles))));
            m_rotationQuat = XMQuaternionMultiply(m_rotationQuat, XMQuaternionRotationAxis(DEF_FORWARD_VECTOR, XMConvertToRadians(XMVectorGetZ(angles))));
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
            m_view = XMMatrixRotationQuaternion(m_rotationQuat);
            m_view.r[3] = m_positionVec;
            m_viewInv = XMMatrixInverse(nullptr, m_view);

            std::cout << "[" << XMVectorGetX(m_view.r[0]) << " " << XMVectorGetY(m_view.r[0]) << " " << XMVectorGetZ(m_view.r[0]) << " " << XMVectorGetW(m_view.r[0]) << "]" << std::endl;
            std::cout << "[" << XMVectorGetX(m_view.r[1]) << " " << XMVectorGetY(m_view.r[1]) << " " << XMVectorGetZ(m_view.r[1]) << " " << XMVectorGetW(m_view.r[1]) << "]" << std::endl;
            std::cout << "[" << XMVectorGetX(m_view.r[2]) << " " << XMVectorGetY(m_view.r[2]) << " " << XMVectorGetZ(m_view.r[2]) << " " << XMVectorGetW(m_view.r[2]) << "]" << std::endl;
            std::cout << "[" << XMVectorGetX(m_view.r[3]) << " " << XMVectorGetY(m_view.r[3]) << " " << XMVectorGetZ(m_view.r[3]) << " " << XMVectorGetW(m_view.r[3]) << "]" << std::endl;

        }


        void EngineCamera::updateMatrices() {

            if (m_matricesUpdated) { return; }
            m_matricesUpdated = true;

            updateViewMatrix();

        }
    } // rend
} // engn