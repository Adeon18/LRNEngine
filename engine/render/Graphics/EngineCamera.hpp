#pragma once

#include <DirectXMath.h>

#include "include/utility/utility.hpp"

#include "source/math/Ray.hpp"

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
            //void addWorldRotationMat(const XMVECTOR& angles);
            //! Relative quaternions rotation
            void addRelativeRotationQuat(const XMVECTOR& angles);

            // Set the camera position
            void setPosition(const XMVECTOR& pos);

            //! Update the entire view matrix, recalc inverse
            void updateViewMatrix();
            //! Update only the position part of the view matrix, recalc inverse, CAN BE OPTIMIZED
            void updateViewMatrixPos();

            //! Called wen the window is resized
            void setNewScreenSize(int width, int height);

            //! Set the projection matrix
            void setProjectionMatrix(float fov, int width, int height);

            //! Cast ray in the mouse direction from camera position
            geom::Ray castRay(float x, float y);

            //! Getters
            const XMMATRIX& getViewMatrix() { return m_view; }
            const XMMATRIX& getProjMatrix() { return m_projection; }
            const XMMATRIX& getProjMatrixReversed() { return m_projectionReversed; }
            [[nodiscard]] const XMVECTOR& getCamRight() { return m_viewInv.r[0]; }
            [[nodiscard]] const XMVECTOR& getCamUp() { return m_viewInv.r[1]; }
            [[nodiscard]] const XMVECTOR& getCamForward() { return m_viewInv.r[2]; }
            [[nodiscard]] const XMVECTOR& getCamPosition() { return m_positionVec; }

        private:
            const XMVECTOR m_viewingFrustumNearPlane[4] =
            {
                {-1.0f, -1.0f, 1.0f, 1.0f},
                {-1.0f,  1.0f, 1.0f, 1.0f},
                { 1.0f,  1.0f, 1.0f, 1.0f},
                { 1.0f, -1.0f, 1.0f, 1.0f},
            };
            XMVECTOR m_viewingFrustumNearPlaneWorldSpace[4];

            const XMVECTOR DEF_FORWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
            const XMVECTOR DEF_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
            const XMVECTOR DEF_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

            int m_screenWidth;
            int m_screenHeight;

            //! Position Data
            XMVECTOR m_positionVec;
            XMFLOAT3 m_position;
            float m_fov;

            //! Projection data
            XMMATRIX m_view;
            XMMATRIX m_viewInv;
            XMMATRIX m_projection;
            // Reversed projection matrix to make ray dragging work
            XMMATRIX m_projectionReversed;
            //! Rotation data
            XMVECTOR m_rotationQuat{ 0.0f, 0.0f, 0.0f, 1.0f };
        };
    } // rend
} // engn