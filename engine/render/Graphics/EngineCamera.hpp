#pragma once

#include "input/Keyboard.hpp"

#include <DirectXMath.h>

#include "include/utility/utility.hpp"

#include "source/math/Ray.hpp"

namespace engn {
    namespace rend {
        using namespace DirectX;
        class EngineCamera {
        public:
            struct CameraSettings {
                static constexpr float CAMERA_SPEED = 0.02f;
                static constexpr float ROTATION_SPEED = 1.2f;
                inline static const std::array<int, 6> MOVE_KEYS{
                                inp::Keyboard::Keys::KEY_A,
                                inp::Keyboard::Keys::KEY_D,
                                inp::Keyboard::Keys::KEY_W,
                                inp::Keyboard::Keys::KEY_S,
                                inp::Keyboard::Keys::KEY_SPACE,
                                inp::Keyboard::Keys::KEY_CTRL,
                };
                inline static const std::array<int, 2> ROLL_KEYS{
                    inp::Keyboard::Keys::KEY_Q,
                    inp::Keyboard::Keys::KEY_E,
                };
                inline static std::unordered_map<int, XMVECTOR> MOVE_TO_ACTION{
                    {inp::Keyboard::Keys::KEY_A, {-1.0f, 0.0f, 0.0f, 0.0f}},
                    {inp::Keyboard::Keys::KEY_D, {1.0f, 0.0f, 0.0f, 0.0f}},
                    {inp::Keyboard::Keys::KEY_CTRL, {0.0f, -1.0f, 0.0f, 0.0f}},
                    {inp::Keyboard::Keys::KEY_SPACE, {0.0f, 1.0f, 0.0f, 0.0f}},
                    {inp::Keyboard::Keys::KEY_W, {0.0f, 0.0f, 1.0f, 0.0f}},
                    {inp::Keyboard::Keys::KEY_S, {0.0f, 0.0f, -1.0f, 0.0f}},
                    // rotation
                    {inp::Keyboard::Keys::KEY_Q, {0.0f, 0.0f, 1.0f, 0.0f}},
                    {inp::Keyboard::Keys::KEY_E, {0.0f, 0.0f, -1.0f, 0.0f}},
                };
            };
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

            //! 
            void getCamFarPlaneDirForFullScreenTriangle(std::vector<XMVECTOR>& outDirs);

            //! Getters
            const XMMATRIX& getViewMatrix() { return m_view; }
            const XMMATRIX& getProjMatrix() { return m_projection; }
            [[nodiscard]] const XMVECTOR& getCamRight() { return m_viewInv.r[0]; }
            [[nodiscard]] const XMVECTOR& getCamUp() { return m_viewInv.r[1]; }
            [[nodiscard]] const XMVECTOR& getCamForward() { return m_viewInv.r[2]; }
            [[nodiscard]] const XMVECTOR& getCamPosition() { return m_positionVec; }

        private:
            const XMVECTOR DEF_FORWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
            const XMVECTOR DEF_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
            const XMVECTOR DEF_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
            // Near plane of the viewing frustum(near plane is 1.0f because depth reversed)
            const XMVECTOR m_viewingFrustumNearPlane[4] =
            {
                {-1.0f, -1.0f, 1.0f, 1.0f},
                {-1.0f,  1.0f, 1.0f, 1.0f},
                { 1.0f,  1.0f, 1.0f, 1.0f},
                { 1.0f, -1.0f, 1.0f, 1.0f},
            };
            const XMVECTOR m_viewingFrustumFarPlaneX2[4] =
            {
                {-1.0f, -1.0f, 1.0f, 1.0f},
                {-1.0f,  3.0f, 1.0f, 1.0f},
                { 1.0f,  1.0f, 1.0f, 1.0f},
                { 3.0f, -1.0f, 1.0f, 1.0f},
            };
            XMVECTOR m_viewingFrustumNearPlaneWorldSpace[4];
            XMVECTOR m_viewingFrustumFarPlaneWorldSpaceX2[4];

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
            //! Rotation data
            XMVECTOR m_rotationQuat{ 0.0f, 0.0f, 0.0f, 1.0f };
        };
    } // rend
} // engn