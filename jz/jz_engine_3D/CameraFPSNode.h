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

#pragma once
#ifndef _JZ_ENGINE_3D_CAMERA_FPS_NODE_H_
#define _JZ_ENGINE_3D_CAMERA_FPS_NODE_H_

#include <jz_core/Event.h>
#include <jz_engine_3D/CameraNode.h>
#include <jz_system/InputPrereqs.h>
#include <vector>

namespace jz
{
    namespace engine_3D
    {

        static const system::Key::Code kDefaultForward = system::Key::kW;
        static const system::Key::Code kDefaultBackward = system::Key::kS;
        static const system::Key::Code kDefaultLeft = system::Key::kA;
        static const system::Key::Code kDefaultRight = system::Key::kD;
        static const system::Mouse::Button kDefaultMouseLookButton = system::Mouse::kLeft;
        static const float kDefaultMoveRate = (float)(1.0 / 1.0);
        static const float kOldLerpFactor = 0.5f;
        static const Radian kMinPitch = -Radian(Constants<float>::kPiOver2 * (float)(2.0 / 3.0));
        static const Radian kMaxPitch =  Radian(Constants<float>::kPiOver2 * (float)(2.0 / 3.0));

        class CameraFPSNode sealed : public CameraNode
        {
        public:
            CameraFPSNode()
                : CameraNode(),
                mbUpdateMouseLook(false),
                mbCustomMouseLookRate(false),
                mbUpdateCamera(true),
                mBackward(kDefaultBackward, system::Key::kReleased),
                mForward(kDefaultForward, system::Key::kReleased),
                mLeft(kDefaultLeft, system::Key::kReleased),
                mRight(kDefaultRight, system::Key::kReleased),
                mbFlipHorizontal(false),
                mbFlipVertical(false),
                mLastMouseChangeX(0.0f),
                mLastMouseChangeY(0.0f),
                mMouseLookButton(kDefaultMouseLookButton),
                mMouseLookRate(0.0f),
                mMoveRate(kDefaultMoveRate),
                mMouseX(0),
                mMouseY(0),
                mMouseSavedX(0),
                mMouseSavedY(0),
                mPitch(0.0f),
                mYaw(0.0f)
            { 
                memset(mMouseButtons, system::Mouse::kReleased, system::Mouse::kButtonCount * sizeof(system::Mouse::State));
            }

            CameraFPSNode(const string& aId)
                : CameraNode(aId),
                mbUpdateMouseLook(false),
                mbCustomMouseLookRate(false),
                mbUpdateCamera(true),
                mBackward(kDefaultBackward, system::Key::kReleased),
                mForward(kDefaultForward, system::Key::kReleased),
                mLeft(kDefaultLeft, system::Key::kReleased),
                mRight(kDefaultRight, system::Key::kReleased),
                mbFlipHorizontal(false),
                mbFlipVertical(false),
                mLastMouseChangeX(0.0f),
                mLastMouseChangeY(0.0f),
                mMouseLookButton(kDefaultMouseLookButton),
                mMouseLookRate(0.0f),
                mMoveRate(kDefaultMoveRate),
                mMouseX(0),
                mMouseY(0),
                mMouseSavedX(0),
                mMouseSavedY(0),
                mPitch(0.0f),
                mYaw(0.0f)
            { 
                memset(mMouseButtons, system::Mouse::kReleased, system::Mouse::kButtonCount * sizeof(system::Mouse::State));            
            }

            float GetMouseLookRate() const { return mMouseLookRate; }
            void SetMouseLookRate(float v)
            {
                mMouseLookRate = v;
                mbCustomMouseLookRate = true;
            }

            system::Key::Code GetBackwardKey() const { return mBackward.Key; }
            void SetBackwardKey(system::Key::Code v) { mBackward.Key = v; }

            system::Key::Code GetForwardKey() const { return mForward.Key; }
            void SetForwardKey(system::Key::Code v) { mForward.Key = v; }

            system::Key::Code GetLeftKey() const { return mLeft.Key; }
            void SetLeftKey(system::Key::Code v) { mLeft.Key = v; }

            system::Key::Code GetRightKey() const { return mRight.Key; }
            void SetRightKey(system::Key::Code v) { mRight.Key = v; }

            bool GetFlipHorizontal() const { return mbFlipHorizontal; }
            void SetFlipHorizontal(bool b) { mbFlipHorizontal = b; }

            bool GetFlipVertical() const { return mbFlipVertical; }
            void SetFlipVertical(bool b) { mbFlipVertical = b; }

            float GetMoveRate() const { return mMoveRate; }
            void SetMoveRate(float v) { mMoveRate = v; }

            system::Mouse::Button GetMouseLookButton() const { return mMouseLookButton; }
            void SetMouseLookButton(system::Mouse::Button b) { mMouseLookButton = b; }

            virtual void SetActive(bool b) override;

        protected:
            virtual ~CameraFPSNode()
            {}

            virtual void _PreUpdate(const Matrix4& aParentWorld, bool abParentChanged) override;
            virtual void ResizeHandler() override;

        private:
            friend void jz::__IncrementRefCount<engine_3D::CameraFPSNode>(engine_3D::CameraFPSNode*);
            friend void jz::__DecrementRefCount<engine_3D::CameraFPSNode>(engine_3D::CameraFPSNode*);

            CameraFPSNode(const CameraFPSNode&);
            CameraFPSNode& operator=(const CameraFPSNode&);

            Event<bool (system::Mouse::Button, system::Mouse::State)>::Entry mButtonConnection;
            Event<bool (system::Key::Code, system::Key::State)>::Entry mKeyConnection;
            Event<void (natural, natural)>::Entry mMoveConnection;

            bool KeyHandler(system::Key::Code aKey, system::Key::State aState)
            {
                bool bReturn = false;

                if (aKey == mBackward.Key) { mBackward.KeyState = aState; bReturn = true; }
                else if (aKey == mForward.Key) { mForward.KeyState = aState; bReturn = true; }
                else if (aKey == mLeft.Key) { mLeft.KeyState = aState; bReturn = true; }
                else if (aKey == mRight.Key) { mRight.KeyState = aState; bReturn = true; }

                return bReturn;
            }

            bool MouseButtonHandler(system::Mouse::Button aButton, system::Mouse::State aState);

            void MouseMoveHandler(natural x, natural y)
            {
                if (mbUpdateMouseLook)
                {
                    mMouseX = x;
                    mMouseY = y;
                }
            }

            bool _UpdateMove() const;

            bool _UpdateMouseLook() const
            {
                return (mbUpdateMouseLook);
            }

            void _UpdateMouseLookRate();

            struct KeyPair
            {
                KeyPair(system::Key::Code aKey, system::Key::State aState)
                    : Key(aKey), KeyState(aState)
                {}

                system::Key::Code Key;
                system::Key::State KeyState;
            };

            bool mbUpdateMouseLook;
            bool mbCustomMouseLookRate;
            bool mbUpdateCamera;
            KeyPair mBackward;
            KeyPair mForward;
            KeyPair mLeft;
            KeyPair mRight;
            bool mbFlipHorizontal;
            bool mbFlipVertical;
            float mLastMouseChangeX;
            float mLastMouseChangeY;
            system::Mouse::State mMouseButtons[system::Mouse::kButtonCount];
            system::Mouse::Button mMouseLookButton;
            float mMouseLookRate;
            float mMoveRate;
            natural mMouseX;
            natural mMouseY;
            natural mMouseSavedX;
            natural mMouseSavedY;
            Radian mPitch;
            Radian mYaw;
        };

        typedef AutoPtr<CameraFPSNode> CameraFPSNodePtr;

    }
}

#endif
