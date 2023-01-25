#include <iostream>

#include "EngineCamera.hpp"


namespace engn {
    namespace rend {
        EngineCamera::EngineCamera(float fov, int screenWidth, int screenHeight, const XMFLOAT3& position) :
            m_position{ position },
            m_rotation{0.0f, 0.0f, 0.0f}
        {
            m_positionVec = XMLoadFloat3(&m_position);
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
            XMStoreFloat3(&m_position, m_positionVec);
            updateViewMatrix();
        }

        void EngineCamera::addRelativeOffset(const XMVECTOR& offset) {
            m_positionVec +=
                XMVectorGetX(offset) * getCamRight() +
                XMVectorGetY(offset) * getCamUp() +
                XMVectorGetZ(offset) * getCamForward();
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

       /* void EngineCamera::addWorldRotationQuat(const XMVECTOR& angles) {
            m_rotationVec +=
                XMVectorGetX(angles) * DEF_FORWARD_VECTOR +
                XMVectorGetY(angles) * getCamUp() +
                XMVectorGetZ(angles) * getCamForward();
            XMStoreFloat3(&m_rotation, m_rotationVec);
            updateViewMatrix();
        }*/

        void EngineCamera::updateViewMatrix() {
            // Get the simple rotation matrix NOT via quaternion
            XMMATRIX rotMatrix = XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
            // Get the transformed forward vector by position and rotation
            XMVECTOR forwardVec = XMVector3Transform(DEF_FORWARD_VECTOR, rotMatrix);
            forwardVec += m_positionVec;

            m_upVec = XMVector3Transform(DEF_UP_VECTOR, rotMatrix);
            
            m_view = XMMatrixLookAtLH(m_positionVec, forwardVec, m_upVec);
            m_viewT = XMMatrixTranspose(m_view);

            std::cout << "[" << XMVectorGetX(m_viewT.r[0]) << " " << XMVectorGetY(m_viewT.r[0]) << " " << XMVectorGetZ(m_viewT.r[0]) << " " << XMVectorGetW(m_viewT.r[0]) << "]" << std::endl;
            std::cout << "[" << XMVectorGetX(m_viewT.r[1]) << " " << XMVectorGetY(m_viewT.r[1]) << " " << XMVectorGetZ(m_viewT.r[1]) << " " << XMVectorGetW(m_viewT.r[1]) << "]" << std::endl;
            std::cout << "[" << XMVectorGetX(m_viewT.r[2]) << " " << XMVectorGetY(m_viewT.r[2]) << " " << XMVectorGetZ(m_viewT.r[2]) << " " << XMVectorGetW(m_viewT.r[2]) << "]" << std::endl;
            std::cout << "[" << XMVectorGetX(m_viewT.r[3]) << " " << XMVectorGetY(m_viewT.r[3]) << " " << XMVectorGetZ(m_viewT.r[3]) << " " << XMVectorGetW(m_viewT.r[3]) << "]" << std::endl;

           /* if (m_basisUpdated) { return; }
            m_basisUpdated = true;*/

            //m_viewInv = glm::toMat4(m_rotationQuat);
        }


        void EngineCamera::updateMatrices() {

            if (m_matricesUpdated) { return; }
            m_matricesUpdated = true;

            updateViewMatrix();

            // Add the offset(posiiton) to view inverse
            //m_viewInv[3] = glm::vec4(m_position, 1.0f);
        }

        void EngineCamera::addRelativeRotation(const XMVECTOR& angles) {
            m_basisUpdated = false;
            m_matricesUpdated = false;

            // roll, pitch, yaw
            //m_rotationQuat *= glm::angleAxis(glm::radians(angles.z), glm::vec3{ getCamForward() });
            //m_rotationQuat *= glm::angleAxis(glm::radians(angles.x), glm::vec3{ getCamRight() });
            //m_rotationQuat *= glm::angleAxis(glm::radians(angles.y), glm::vec3{ getCamUp() });

           //glm::normalize(m_rotationQuat);
        }
    } // rend
} // engn