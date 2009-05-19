//
// Copyright (c) 2009 Joseph A. Zupko
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 

#include <jz_engine_3D/CameraFPSNode.h>
#include <jz_engine_3D/PhysicsNode.h>
#include <jz_system/Input.h>
#include <jz_system/Time.h>
#include <jz_graphics/Graphics.h>

#include <jz_physics/narrowphase/Body.h>
#include <jz_physics/narrowphase/collision/SphereShape.h>
#include <jz_physics/World.h>

namespace jz
{
    namespace engine_3D
    {

        void CameraFPSNode::SetCollisionEnabled(bool b, float aCollisionRadius, bool abFriction)
        {
            mbWantsPhysicsBody = b;
            if (mbWantsPhysicsBody)
            {
                mCollisionRadius = aCollisionRadius;
                mbWantsFriction = abFriction;
            }

            if (b != mpPhysicsBody.IsValid())
            {
                PhysicsNodePtr pNode = Get<PhysicsNode>(GetBaseId(), "physics_node");
                if (pNode.IsValid())
                {
                    physics::World3D* pWorld = pNode->GetWorld();
                    
                    if (b)
                    {
                        mpPhysicsBody = pWorld->Create(new physics::SphereShape(mCollisionRadius), physics::Body3D::kDynamic | physics::Body3D::kNonAngular, physics::Body3D::kDynamic | physics::Body3D::kStatic);
                        mpPhysicsBody->SetAffectedByGravity(false);
                        mpPhysicsBody->SetFriction((mbWantsFriction) ? 1.0f : 0.0f);
                        mpPhysicsBody->SetMass(1.0f);
                        mpPhysicsBody->SetFrame(CoordinateFrame3D::CreateFromMatrix4(mWorld));
                        mpPhysicsBody->OnUpdate.Add<CameraFPSNode, &CameraFPSNode::__UpdateHandler>(this, mUpdateConnection);
                    }
                    else
                    {
                        mUpdateConnection.Reset();
                        mpPhysicsBody.Reset();
                    }
                }
            }
        }

        bool CameraFPSNode::MouseButtonHandler(system::Mouse::Button aButton, system::Mouse::State aState)
        {
            using namespace system;
            mMouseButtons[aButton] = aState;

            if (aButton == mMouseLookButton)
            {
                Input& input = Input::GetSingleton();

                if (aState == Mouse::kPressed)
                {
                    natural mouseX = input.GetMouseX();
                    natural mouseY = input.GetMouseY();
                    
                    natural width = graphics::Graphics::GetSingleton().GetViewportWidth();
                    natural height = graphics::Graphics::GetSingleton().GetViewportHeight();

                    if (mouseX > 0 && mouseY > 0 && mouseX < width && mouseY < height)
                    {
                        mMouseSavedX = mouseX;
                        mMouseSavedY = mouseY;
                        mMouseX = mMouseSavedX;
                        mMouseY = mMouseSavedY;
                        input.SetMouseVisible(false);
                        mbUpdateMouseLook = true;
                    }
                    else
                    {
                        mBackward.KeyState = Key::kReleased;
                        mForward.KeyState = Key::kReleased;
                        mLeft.KeyState = Key::kReleased;
                        mRight.KeyState = Key::kReleased;
                    }
                }
                else
                {
                    if (mbUpdateMouseLook)
                    {
                        input.SetMousePosition(mMouseSavedX, mMouseSavedY);
                        mbUpdateMouseLook = false;
                        input.SetMouseVisible(true);
                        mLastMouseChangeX = 0.0f;
                        mLastMouseChangeY = 0.0f;
                    }
                }

                return true;
            }

            return false;
        }

        void CameraFPSNode::_PreUpdateA(const Matrix4& aParentWorld, bool abParentChanged)
        {
            using namespace system;
            CameraNode::_PreUpdateA(aParentWorld, abParentChanged);

            if (mpPhysicsBody.IsValid() && (mFlags & SceneNodeFlags::kWorldDirty) != 0)
            {
                mpPhysicsBody->SetFrame(CoordinateFrame3D::CreateFromMatrix4(mWorld));
            }

            Vector3 vChange = Vector3::kZero;

            if (_UpdateMove())
            {
                if (mBackward.KeyState == Key::kPressed) { vChange += Vector3::kBackward; mbUpdateCamera = true; }
                if (mForward.KeyState == Key::kPressed) { vChange += Vector3::kForward; mbUpdateCamera = true; }
                if (mLeft.KeyState == Key::kPressed) { vChange += Vector3::kLeft; mbUpdateCamera = true; }
                if (mRight.KeyState == Key::kPressed) { vChange += Vector3::kRight; mbUpdateCamera = true; }

                vChange = Vector3::Normalize(vChange);
            }

            if (_UpdateMouseLook())
            {
                natural mouseChangeX = (mMouseX - mMouseSavedX);
                natural mouseChangeY = (mMouseY - mMouseSavedY);

                if ((mouseChangeX != 0) || (mouseChangeY != 0))
                {
                    float changeX = (mMouseLookRate * mouseChangeX);
                    float changeY = (mMouseLookRate * mouseChangeY);

                    float lerpChangeX = Lerp(changeX, mLastMouseChangeX, kOldLerpFactor);
                    float lerpChangeY = Lerp(changeY, mLastMouseChangeY, kOldLerpFactor);

                    mYaw = (mbFlipHorizontal) ? (mYaw + Radian(lerpChangeX)) : (mYaw - Radian(lerpChangeX));
                    mPitch = (mbFlipVertical) ? (mPitch + Radian(lerpChangeY)) : (mPitch - Radian(lerpChangeY));

                    while (mYaw < -Radian::kTwoPi) mYaw += Radian::kTwoPi;
                    while (mYaw > Radian::kTwoPi) mYaw -= Radian::kTwoPi;
                    while (mPitch < -Radian::kTwoPi) mPitch += Radian::kTwoPi;
                    while (mPitch > Radian::kTwoPi) mPitch -= Radian::kTwoPi;

                    mPitch = Clamp(mPitch, kMinPitch, kMaxPitch);

                    mLastMouseChangeX = lerpChangeX;
                    mLastMouseChangeY = lerpChangeY;

                    Input::GetSingleton().SetMousePosition(mMouseSavedX, mMouseSavedY);
                    mbUpdateCamera = true;
                }
            }

            if (mbUpdateCamera)
            {
                #pragma region Orientation
                Quaternion q = Quaternion::CreateFromAxisAngle(Vector3::kUp, mYaw) * Quaternion::CreateFromAxisAngle(Vector3::kRight, mPitch);
                ToTransform(q, mWorld);
                #pragma endregion

                if (mpPhysicsBody.IsValid())
                {
                    Vector3 v = (Vector3::TransformDirection(mWorld, vChange) * mMoveRate);
                    mpPhysicsBody->SetLinearVelocity(v);
                    mpPhysicsBody->SetOrientation(Matrix3::CreateFromUpperLeft(mWorld));
                }
                else
                {
                    #pragma region Position
                    float et = (Time::GetSingleton().GetElapsedSeconds());
                    float delta = (et * mMoveRate);

                    Vector3 newPosition = mWorld.GetTranslation() + (Vector3::TransformDirection(mWorld, vChange) * delta);
                    ToTransform(newPosition, mWorld);
                    #pragma endregion
                }

                mbUpdateCamera = false;
                mFlags |= SceneNodeFlags::kWorldDirty;
            }
            else
            {
                if (mpPhysicsBody.IsValid())
                {
                    mpPhysicsBody->SetLinearVelocity(Vector3::kZero);
                }
            }
        }

        void CameraFPSNode::__UpdateHandler(physics::Body3D* apBody)
        {
            ToTransform(apBody->GetFrame(), mWorld);
            mFlags |= SceneNodeFlags::kWorldDirty;
        }

        void CameraFPSNode::ResizeHandler()
        {
            _UpdateMouseLookRate();

            CameraNode::ResizeHandler();
        }

        void CameraFPSNode::SetActive(bool b)
        {
            using namespace system;
            if (b != mbActive)
            {
                CameraNode::SetActive(b);

                Input& input = Input::GetSingleton();

                if (mbActive)
                {
                    input.OnKey.Add<CameraFPSNode, &CameraFPSNode::KeyHandler>(this, mKeyConnection);
                    input.OnMouseMoveAbsolute.Add<CameraFPSNode, &CameraFPSNode::MouseMoveHandler>(this, mMoveConnection);
                    input.OnButton.Add<CameraFPSNode, &CameraFPSNode::MouseButtonHandler>(this, mButtonConnection);

                    _UpdateMouseLookRate();
                    input.SetMouseVisible(true);
                }
                else
                {
                    mButtonConnection.Reset();
                    mMoveConnection.Reset();
                    mKeyConnection.Reset();
                    
                    mBackward.KeyState = Key::kReleased;
                    mForward.KeyState = Key::kReleased;
                    mLeft.KeyState = Key::kReleased;
                    mRight.KeyState = Key::kReleased;

                    mLastMouseChangeX = 0.0f;
                    mLastMouseChangeY = 0.0f;

                    memset(mMouseButtons, 0, Mouse::kButtonCount * sizeof(Mouse::State));
                }
            }
        }

        bool CameraFPSNode::_UpdateMove() const
        {
            return (graphics::Graphics::GetSingleton().IsActive());
        }

        void CameraFPSNode::_UpdateMouseLookRate()
        {
            if (!mbCustomMouseLookRate)
            {
                natural width = graphics::Graphics::GetSingleton().GetViewportWidth();
                natural height = graphics::Graphics::GetSingleton().GetViewportHeight();

                float diagonal = Sqrt((float)((height * height) + (width * width)));

                if (!AboutZero(diagonal, Constants<float>::kLooseTolerance))
                {
                    mMouseLookRate = (float)(2.0f / diagonal);
                }
            }
        }

    }
}
