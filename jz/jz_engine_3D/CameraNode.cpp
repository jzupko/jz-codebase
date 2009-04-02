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

#include <jz_engine_3D/CameraNode.h>
#include <jz_engine_3D/RenderMan.h>
#include <jz_graphics/Graphics.h>

namespace jz
{
    namespace engine_3D
    {

        void CameraNode::_PostUpdate(bool abChanged)
        {
            if (abChanged)
            {
                mView = Matrix4::Invert(mWorld);
                mbViewDirty = true;
            }

            if (mbActive && mbViewDirty)
            {
                RenderMan::GetSingleton().SetView(mView);
                mbViewDirty = false;
            }

            if (mbActive && mbProjectionDirty)
            {
                RenderMan::GetSingleton().SetProjection(mProjection);
                mbProjectionDirty = false;
            }

            SceneNode::_PostUpdate(abChanged);
        }

        void CameraNode::ResizeHandler()
        {
            float near;
            float far;
            Matrix4::ExtractNearFarDirectX(mProjection, near, far);

            natural width = graphics::Graphics::GetSingleton().GetViewportWidth();
            natural height = graphics::Graphics::GetSingleton().GetViewportHeight();

            float aspectRatio = (float)width / (float)height;
            Radian fov = Matrix4::ExtractFov(mProjection);

            SetProjection(Matrix4::CreatePerspectiveFieldOfViewDirectX(fov, aspectRatio, near, far));
        }

        void CameraNode::SetActive(bool b)
        {
            if (b != mbActive)
            {
                mbActive = b;

                if (mbActive)
                {
                    mbProjectionDirty = true;
                    mbViewDirty = true;

                    graphics::Graphics::GetSingleton().OnReset.Add<CameraNode, &CameraNode::OnResize>(this, mResizeConnection);
                }
                else
                {
                    mResizeConnection.Reset();
                }
            }
        }

    }
}
