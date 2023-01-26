#pragma once

#include <DirectXMath.h>

#include "include/utility/utility.hpp"

namespace engn {
    namespace rend {
        using namespace DirectX;
        class EngineCamera {

        public:
            EngineCamera(float fov, int screenWidth, int screenHeight, const XMFLOAT3& position);

            //! Add ofset to camera without taking rotations into account
            void addWorldOffset(const XMVECTOR& offset);
            //! Add offset but with rotations in mind
            void addRelativeOffset(const XMVECTOR& offset);
            //! Add basic quaternion rotation
            void addWorldRotationQuat(const XMVECTOR& angles);
            void addWorldRotationMat(const XMVECTOR& angles);
            //! Relative quaternions rotation, don't quite understand why would I need this
            void addRelativeRotationQuat(const XMVECTOR& angles);

            void setPosition(const XMVECTOR& pos);
            void setRotation(const XMVECTOR& rot);

            //! Update all the iternal matrices and near plane data - includes updating basis, should be called after movement
            void updateMatrices();
            //! Turn the rotation quaternion to view matrix
            void updateViewMatrix();

            //! Called wen the window is resized
            void setNewScreenSize(int width, int height);

            //! Set the projection matrix
            void setProjectionMatrix(float fov, int width, int height);

            //! Getters
            const XMMATRIX& getViewMatrix() { return m_viewInv; }
            const XMMATRIX& getProjMatrix() { return m_projection; }
            [[nodiscard]] const XMVECTOR& getCamRight() { return m_view.r[0]; }
            [[nodiscard]] const XMVECTOR& getCamUp() { return m_view.r[1]; }
            [[nodiscard]] const XMVECTOR& getCamForward() { return m_view.r[2]; }
            [[nodiscard]] const XMVECTOR& getCamPosition() { return m_positionVec; }

        private:
            const XMVECTOR DEF_FORWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
            const XMVECTOR DEF_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
            const XMVECTOR DEF_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

            //! Position Data
            XMVECTOR m_positionVec;
            XMFLOAT3 m_position;
            float m_fov;

            //! Projection data
            XMMATRIX m_view;
            XMMATRIX m_viewInv;
            //XMMATRIX m_viewT;
            XMMATRIX m_projection;

            //! Rotation data
            XMVECTOR m_rotationVec;
            XMFLOAT3 m_rotation;

            XMVECTOR m_rotationQuat{ 0.0f, 0.0f, 0.0f, 1.0f };

            XMVECTOR m_leftVec;
            XMVECTOR m_upVec;
            XMVECTOR m_rightVec;
            XMVECTOR m_forwardVec;
            XMVECTOR m_backwardVec;

            bool m_basisUpdated = false;
            bool m_matricesUpdated = false;
        };
    } // rend
} // engn