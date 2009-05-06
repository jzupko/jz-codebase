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
#ifndef _JZ_ENGINE_3D_DEFERRED_H_
#define _JZ_ENGINE_3D_DEFERRED_H_

#include <jz_core/Auto.h>
#include <jz_core/Color.h>
#include <jz_core/Event.h>
#include <jz_core/Utility.h>
#include <jz_core/Vector4.h>
#include <jz_graphics/Effect.h>
#include <vector>

namespace jz
{
    namespace graphics
    {
        class Mesh;
        class Target; typedef AutoPtr<Target> TargetPtr;
        class VolatileTexture; typedef AutoPtr<VolatileTexture> VolatileTexturePtr;
    }

    namespace engine_3D
    {

        class LightNode; typedef AutoPtr<LightNode> LightNodePtr;
        class Deferred sealed : public Singleton<Deferred>
        {
        public:
            Deferred();
            ~Deferred();

            static const int kTapsRt = 4;
            static const int kCircularTaps = (kTapsRt * kTapsRt);

            static const int kGaussianKernelRadius = 11;
            static const int kMrtCount = 3;
            static const int kHdrCount = 2;
            static const int kShadowCount = 2;

            bool bActive() const { return mbActive; }
            void SetActive(bool b)
            {
                if (b != mbActive)
                {
                    if (b) { _Activate(); }
                    else { _Deactivate(); }
                }
            }

            float GetAoRadius() const { return mAoRadius; }
            void SetAoRadius(float v);

            float GetAoScale() const { return mAoScale; }
            void SetAoScale(float v);

            bool bDebugAO() const { return mbDebugAO; }
            void SetDebugAO(bool v) { mbDebugAO = v; }

            bool bDebugDeferredLighting() const { return mbDebugDeferredLighting; }
            void SetDebugDeferredLighting(bool v) { mbDebugDeferredLighting = v; }

            void RenderLight(LightNode* apNode)
            {
                if (mbActive && apNode) { mLights.push_back(apNode); }
            }

            void Begin(const ColorRGBA& c = ColorRGBA::kBlack);
            void PreTransparency();
            void End();

            float GetBloomThreshold() const { return mBloomThreshold; }
            void SetBloomThreshold(float v) { mBloomThreshold = Max(v, 0.0f); }

            float GetMotionBlurAmount() const { return mMotionBlurAmount; }
            void SetMotionBlurAmount(float v) { mMotionBlurAmount = Max(v, 0.0f); }

            float GetShadowControlTerm() const { return mShadowControlTerm; }
            void SetShadowControlTerm(float v) { mShadowControlTerm = Max(v, 0.0f); }

            float GetGaussianKernelStdDev() const { return mGaussianStdDev; }
            void SetGaussianKernelStdDev(float v);

        private:
            void _Load();
            void _Unload();

            Event<void()>::Entry mLoadCallback;
            Event<void()>::Entry mUnloadCallback;

            graphics::Parameter<graphics::Target> jz_ShadowTexture;
            graphics::Parameter<Vector2> jz_ShadowDelta;
            graphics::Parameter<Vector2> jz_ShadowNearFar;
            graphics::Parameter<Vector4> jz_ShadowScaleShift;
            graphics::Parameter<Matrix4> jz_ShadowTransform;
            graphics::Parameter<float> jz_ShadowControlTerm;

            graphics::Parameter<Matrix4> jz_Projection;
            graphics::Parameter<Matrix4> jz_View;
            graphics::Parameter<Matrix4> jz_World;

            graphics::Parameter<Matrix4> jz_PrevProjection;
            graphics::Parameter<Matrix4> jz_PrevView;
            graphics::Parameter<float> jz_MotionBlurAmount;

            graphics::Parameter<float> jz_BloomThreshold;

            graphics::Parameter<float> jz_Gamma;
            graphics::Parameter<Vector2> jz_CameraFocalLength;
            graphics::Parameter<Vector2> jz_ScreenDimensions;

            graphics::Parameter<graphics::Target> jz_MrtTexture0;
            graphics::Parameter<graphics::Target> jz_MrtTexture1;
            graphics::Parameter<graphics::Target> jz_MrtTexture2;

            graphics::Parameter<float> jz_GaussianWeights;
            graphics::Parameter<graphics::Target> jz_HdrTexture;
            graphics::Parameter<Vector4> jz_RandomVectors;

            graphics::Parameter<float> jz_AoRadius;
            graphics::Parameter<float> jz_AoScale;
            graphics::Parameter<bool> jz_bDebugAO;

            graphics::Technique mAoPass1;
            graphics::Technique mAoPass2;
            graphics::Technique mAoPass3;
            graphics::Technique mLdrPass;
            graphics::Technique mShadowBlurPass1;
            graphics::Technique mShadowBlurPass2;
            graphics::Technique mMotionBlur;
            graphics::Technique mBloomProcess;
            graphics::Technique mBloomBlurPass1;
            graphics::Technique mBloomBlurPass2;
            graphics::EffectPtr mEffect;

            graphics::Parameter<bool> jz_bDebugDeferred;
            graphics::Parameter<Vector3> jz_LightAttenuation;
            graphics::Parameter<Vector3> jz_LightColor;
            graphics::Parameter<Vector3> jz_LightV;
            graphics::Parameter<Vector3> jz_SpotDirection;
            graphics::Parameter<float> jz_SpotFalloffCosHalfAngle;
            graphics::Parameter<float> jz_SpotFalloffExponent;

            graphics::Technique mDirectional;
            graphics::Technique mPoint;
            graphics::Technique mPointAsQuad;
            graphics::Technique mSpot;
            graphics::Technique mSpotWithShadow;
            graphics::Technique mSpotAsQuad;
            graphics::Technique mSpotWithShadowAsQuad;

            bool mbActive;
            float mShadowControlTerm;

            float mMotionBlurAmount;
            float mBloomThreshold;

            graphics::TargetPtr mTargets[kMrtCount];
            graphics::TargetPtr mHdrTargets[kHdrCount];
            graphics::TargetPtr mShadowTargets[kShadowCount];

            float mGaussianWeights[kGaussianKernelRadius];
            float mGaussianStdDev;

            Vector4 mRandomVectors[kCircularTaps];

            float mAoRadius;
            float mAoScale;
            bool mbDebugAO;

            bool mbDebugDeferredLighting;
            vector<LightNode*> mLights;

            void _Activate();
            void _Deactivate();

            void _Set(graphics::Mesh* apMesh);
            void _DoPasses(graphics::Mesh* apMesh);
            void _Ao();
            void _Bloom();
            void _MotionBlur();
            void _ShadowMapProcess();
            void _DeferredLighting();
            void _Ldr();
            void _InitPost();
            void _PreTransparency();
            void _End();
            void _PushScreenDimensions(natural aWidth, natural aHeight);

            Deferred(const Deferred&);
            Deferred& operator=(const Deferred&);
        };

    }
}

#endif
