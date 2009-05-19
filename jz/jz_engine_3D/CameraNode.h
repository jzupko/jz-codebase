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
#ifndef _JZ_ENGINE_3D_CAMERA_NODE_H_
#define _JZ_ENGINE_3D_CAMERA_NODE_H_

#include <jz_core/Event.h>
#include <jz_engine_3D/SceneNode.h>

namespace jz
{
    namespace engine_3D
    {

        class CameraNode : public SceneNode
        {
        public:
            CameraNode()
                : SceneNode(),
                mbActive(false),
                mbProjectionDirty(false),
                mbViewDirty(false),
                mProjection(Matrix4::kIdentity),
                mView(Matrix4::kIdentity)
            {
                mFlags |= SceneNodeFlags::kExcludeFromBounding;
            }

            CameraNode(const string& aBaseId, const string& aId)
                : SceneNode(aBaseId, aId),
                mbActive(false),
                mbProjectionDirty(false),
                mbViewDirty(false),
                mProjection(Matrix4::kIdentity),
                mView(Matrix4::kIdentity)
            {
                mFlags |= SceneNodeFlags::kExcludeFromBounding;
            }

            const Matrix4& GetProjection() const { return mProjection; }
            void SetProjection(const Matrix4& p)
            {
                mProjection = p;
                mbProjectionDirty = true;
            }

            const Matrix4& GetView() const { return mView; }

            bool IsActive() const { return mbActive; }
            virtual void SetActive(bool b);

        protected:
            virtual void _PostUpdate(bool abChanged) override;

            bool mbActive;
            bool mbProjectionDirty;
            bool mbViewDirty;
            Matrix4 mProjection;
            Matrix4 mView;

            virtual ~CameraNode()
            {}

            void OnResize(natural aWidth, natural aHeight)
            {
                ResizeHandler();
            }

            virtual void ResizeHandler();

        private:
            friend void jz::__IncrementRefCount<engine_3D::CameraNode>(engine_3D::CameraNode*);
            friend void jz::__DecrementRefCount<engine_3D::CameraNode>(engine_3D::CameraNode*);

            CameraNode(const CameraNode&);
            CameraNode& operator=(const CameraNode&);

            Event<void(natural, natural)>::Entry mResizeConnection;
        };

        typedef AutoPtr<CameraNode> CameraNodePtr;

    }
}

#endif
