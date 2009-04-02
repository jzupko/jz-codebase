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

#include <jz_engine_3D/Deferred.h>
#include <jz_engine_3D/LightNode.h>
#include <jz_engine_3D/RenderMan.h>
#include <jz_engine_3D/ShadowMan.h>
#include <jz_system/Files.h>
#include <jz_graphics/Effect.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics/Material.h>
#include <jz_graphics/Mesh.h>
#include <jz_graphics/Target.h>
#include <jz_graphics/VertexDeclaration.h>
#include <jz_graphics/VolatileTexture.h>

namespace jz
{
    engine_3D::Deferred* engine_3D::Deferred::mspSingleton = null;
    namespace engine_3D
    {

        static const float kDefaultGaussianKernelStdDev = 5.0f;
        static const char* kEffectFile = "..\\media\\engine_3D_deferred.cfx";

        static const float kDefaultAoScale = -0.3f;
        static const float kMinAoScale = -0.9f;
        static const float kMaxAoScale =  0.9f;

        static const float kDefaultAoRadius = 64.0f;
        static const float kMinAoRadius = 1.0f;
        static const float kMaxAoRadius = 64.0f;

        static const graphics::Target::Format kMrtFormats[Deferred::kMrtCount] = 
        {
            graphics::Target::kA16B16G16R16F,
            graphics::Target::kA16B16G16R16F,
            graphics::Target::kA16B16G16R16F
        };

        static const graphics::Target::Format kHdrFormats[Deferred::kHdrCount] = 
        {
            graphics::Target::kA16B16G16R16F,
            graphics::Target::kA16B16G16R16F
        };

        static const char* kDirectional = "Directional";
        static const char* kPoint = "Point";
        static const char* kPointAsQuad = "PointAsQuad";
        static const char* kSpot = "Spot";
        static const char* kSpotWithShadow = "SpotWithShadow";
        static const char* kSpotAsQuad = "SpotAsQuad";

        static const char* kAoPass1 = "AOPass1";
        static const char* kAoPass2 = "AOPass2";
        static const char* kAoPass3 = "AOPass3";
        static const char* kLdrPass = "LdrPass";

        Deferred::Deferred()
            : mbActive(false),
            mAoRadius(kDefaultAoRadius),
            mAoScale(kDefaultAoScale),
            mbDebugAO(false),
            mbDebugDeferredLighting(false)
        {
            SetGaussianKernelStdDev(kDefaultGaussianKernelStdDev);
        }

        Deferred::~Deferred()
        {
            if (mbActive)
            {
                _Unload();
                _Deactivate();
            }
        }

        void Deferred::SetAoRadius(float v)
        {
            mAoRadius = Clamp(v, kMinAoRadius, kMaxAoRadius);
        }

        void Deferred::SetAoScale(float v)
        {
            mAoScale = Clamp(v, kMinAoScale, kMaxAoScale);
        }

        void Deferred::_Load()
        {
            using namespace graphics;
#           define JZ_HELPER(name) mEffect->GetParameterBySemantic(#name, name)

            Graphics& gd = Graphics::GetSingleton();

            mEffect = gd.Create<Effect>(kEffectFile);

            JZ_HELPER(jz_ShadowBleedReduction);
            JZ_HELPER(jz_ShadowTexture);
            JZ_HELPER(jz_ShadowDelta);
            JZ_HELPER(jz_ShadowNearFar);
            JZ_HELPER(jz_ShadowScaleShift);
            JZ_HELPER(jz_ShadowTransform);

            JZ_HELPER(jz_Projection);
            JZ_HELPER(jz_View);
            JZ_HELPER(jz_World);

            JZ_HELPER(jz_Gamma);
            JZ_HELPER(jz_CameraFocalLength);
            JZ_HELPER(jz_ScreenDimensions);

            JZ_HELPER(jz_MrtTexture0);
            JZ_HELPER(jz_MrtTexture1);
            JZ_HELPER(jz_MrtTexture2);

            JZ_HELPER(jz_GaussianWeights);
            JZ_HELPER(jz_HdrTexture);
            JZ_HELPER(jz_RandomVectors);

            JZ_HELPER(jz_AoRadius);
            JZ_HELPER(jz_AoScale);
            JZ_HELPER(jz_bDebugAO);

            JZ_HELPER(jz_bDebugDeferred);
            JZ_HELPER(jz_LightAttenuation);
            JZ_HELPER(jz_LightColor);
            JZ_HELPER(jz_LightV);
            JZ_HELPER(jz_SpotDirection);
            JZ_HELPER(jz_SpotFalloffCosHalfAngle);
            JZ_HELPER(jz_SpotFalloffExponent);
#           undef JZ_HELPER

#           define JZ_HELPER(name) m##name = mEffect->GetTechniqueByName(k##name)
            JZ_HELPER(AoPass1);
            JZ_HELPER(AoPass2);
            JZ_HELPER(AoPass3);
            JZ_HELPER(LdrPass);

            JZ_HELPER(Directional);
            JZ_HELPER(Point);
            JZ_HELPER(PointAsQuad);
            JZ_HELPER(Spot);
            JZ_HELPER(SpotWithShadow);
            JZ_HELPER(SpotAsQuad);
#           undef JZ_HELPER

            #pragma region Random vectors
            {
                const Radian kAngleFactor = (Radian::kTwoPi / kTapsRt);
                const float kScaleFactor = (1.0f / kTapsRt);

                int index = 0;
                for (int i = 0; i < kTapsRt; i++)
                {
                    for (int j = 0; j < kTapsRt; j++)
                    {
                        float scl  = ((float)i * kScaleFactor) + (UniformRandomf() * kScaleFactor);
                        Radian ang = ((float)j * kAngleFactor) + (UniformRandomf() * kAngleFactor);

                        mRandomVectors[index++] = Vector4(Cos(ang) * scl, Sin(ang) * scl, 0.0f, 0.0f);
                    }
                }
            }
            #pragma endregion
        }

        void Deferred::_Unload()
        {
            mEffect.Reset();

            jz_ShadowBleedReduction.Reset();
            jz_ShadowTexture.Reset();
            jz_ShadowDelta.Reset();
            jz_ShadowNearFar.Reset();
            jz_ShadowScaleShift.Reset();
            jz_ShadowTransform.Reset();

            jz_Projection.Reset();
            jz_View.Reset();
            jz_World.Reset();

            jz_Gamma.Reset();
            jz_CameraFocalLength.Reset();
            jz_ScreenDimensions.Reset();

            jz_MrtTexture0.Reset();
            jz_MrtTexture1.Reset();
            jz_MrtTexture2.Reset();

            jz_GaussianWeights.Reset();
            jz_HdrTexture.Reset();
            jz_RandomVectors.Reset();
            jz_AoRadius.Reset();
            jz_AoScale.Reset();
            jz_bDebugAO.Reset();

            jz_bDebugDeferred.Reset();
            jz_LightAttenuation.Reset();
            jz_LightColor.Reset();
            jz_LightV.Reset();
            jz_SpotDirection.Reset();
            jz_SpotFalloffCosHalfAngle.Reset();
            jz_SpotFalloffExponent.Reset();

            mAoPass1.Reset();
            mAoPass2.Reset();
            mAoPass3.Reset();
            mLdrPass.Reset();

            mDirectional.Reset();
            mPoint.Reset();
            mPointAsQuad.Reset();
            mSpot.Reset();
            mSpotWithShadow.Reset();
            mSpotAsQuad.Reset();
        }

        void Deferred::Begin(const ColorRGBA& c)
        {
            using namespace graphics;

            mTargets[0]->SetToDevice(0u);
            mTargets[1]->SetToDevice(1u);
            mTargets[2]->SetToDevice(2u);

            Graphics::GetSingleton().Clear(Graphics::kColor | Graphics::kDepth | Graphics::kStencil, c);
        }

        void Deferred::PreTransparency()
        {
            graphics::Target::ResetTarget(2u);
            graphics::Target::ResetTarget(1u);
            graphics::Target::ResetTarget(0u);

            _PreTransparency();
        }

        void Deferred::End()
        {
            graphics::Target::ResetTarget(2u);
            graphics::Target::ResetTarget(1u);
            graphics::Target::ResetTarget(0u);

            _End();
        }

        void Deferred::SetGaussianKernelStdDev(float v)
        {
            mGaussianStdDev = v;
            float sum = 0.0f;

            mGaussianWeights[0] = Gaussian1D(0, mGaussianStdDev);
            for (int i = 1; i < kGaussianKernelRadius; i++)
            {
                mGaussianWeights[i] = Gaussian1D((float)i, mGaussianStdDev);
                sum += mGaussianWeights[i];
            }
            sum *= 2.0f;
            sum += mGaussianWeights[0];

            for (int i = 0; i < kGaussianKernelRadius; i++) { mGaussianWeights[i] /= sum; }
        }

        void Deferred::_Activate()
        {
            using namespace graphics;
            JZ_ASSERT(Graphics::GetSingletonExists());

            if (!mbActive)
            {
                Graphics& graphics = Graphics::GetSingleton();

                for (int i = 0; i < kMrtCount; i++) { mTargets[i] = graphics.Create<Target>(0u, 0u, kMrtFormats[i]); }
                for (int i = 0; i < kHdrCount; i++) { mHdrTargets[i] = graphics.Create<Target>(0u, 0u, kHdrFormats[i]); }

                graphics.OnLoad.Add<Deferred, &Deferred::_Load>(this, mLoadCallback);
                graphics.OnUnload.Add<Deferred, &Deferred::_Unload>(this, mUnloadCallback);

                mbActive = true;

                if (graphics.IsLoaded()) { _Load(); }
            }
        }

        void Deferred::_Deactivate()
        {
            if (mbActive)
            {
                graphics::Graphics& graphics = graphics::Graphics::GetSingleton();

                if (graphics.IsLoaded()) { _Unload(); }

                mbActive = false;

                mUnloadCallback.Reset();
                mLoadCallback.Reset();

                for (int i = 0; i < kHdrCount; i++) { mHdrTargets[i].Reset(); }
                for (int i = 0; i < kMrtCount; i++) { mTargets[i].Reset(); }
            }
        }

        void Deferred::_Set(graphics::Mesh* apMesh)
        {
            apMesh->GetVertexDeclaration()->SetToDevice();
            apMesh->SetIndices();
            apMesh->SetVertices();
        }

        void Deferred::_DoPasses(graphics::Mesh* apMesh)
        {
            graphics::Pass pass = mEffect->Begin();
            if (pass.IsValid())
            {
                while (pass.IsValid())
                {
                    if (pass.Begin())
                    {
                        apMesh->Draw();
                        pass.End();
                    }
                    pass = pass.Next();
                }
                mEffect->End();
            }
        }

        void Deferred::_Ao()
        {
            using namespace graphics;

            RenderMan& rm = RenderMan::GetSingleton();

            mHdrTargets[0]->SetToDevice(0u);
            mEffect->SetTechnique(mAoPass1);
            _DoPasses(rm.GetUnitQuadMesh().Get());

            mHdrTargets[1]->SetToDevice(0u);
            jz_HdrTexture.Set(mHdrTargets[0]);
            mEffect->SetTechnique(mAoPass2);
            _DoPasses(rm.GetUnitQuadMesh().Get());

            mHdrTargets[0]->SetToDevice(0u);
            jz_HdrTexture.Set(mHdrTargets[1]);
            mEffect->SetTechnique(mAoPass3);
            _DoPasses(rm.GetUnitQuadMesh().Get());

            Swap(mHdrTargets[0], mTargets[1]);
            mTargets[0]->SetToDevice(0u);
            jz_MrtTexture1.Set(mTargets[1]);
        }

        void Deferred::_DeferredLighting()
        {
            using namespace graphics;

            RenderMan& rm = RenderMan::GetSingleton();
            ShadowMan& sm = ShadowMan::GetSingleton();

            jz_bDebugDeferred.Set(mbDebugDeferredLighting);
            size_t count = mLights.size();
            for (size_t i = 0; i < count; i++)
            {
                LightNode* light = mLights[i];

                const wchar_t* pDescription = (light->GetType() == LightNodeType::kSpot)
                    ? L"Spot light" : ((light->GetType() == LightNodeType::kPoint) ? L"Point light" : L"Directional light");

                jz_LightAttenuation.Set(light->GetAttenuation());
                jz_LightColor.Set(light->GetColor());

                Vector3 viewDirection = Vector3::TransformDirection(rm.GetView(), light->GetWorldLightDirection());
                Vector3 viewPosition = Vector3::TransformPosition(rm.GetView(), light->GetWorldTranslation());
                jz_LightV.Set((light->GetType() == LightNodeType::kDirectional) ? viewDirection : viewPosition);

                if (light->GetType() == LightNodeType::kSpot)
                {
                    jz_SpotDirection.Set(viewDirection);
                    jz_SpotFalloffCosHalfAngle.Set(light->GetFalloffCosHalfAngle());
                    jz_SpotFalloffExponent.Set(light->GetFalloffExponent());
                }

                if (light->GetType() == LightNodeType::kDirectional)
                {
                    mEffect->SetTechnique(mDirectional);
                    _Set(rm.GetUnitQuadMesh().Get());
                    _DoPasses(rm.GetUnitQuadMesh().Get());
                }
                else if (light->GetType() == LightNodeType::kPoint)
                {
                    const BoundingSphere& worldBounding = rm.GetWorldFrustumBoundingSphere();
                    if (light->GetWorldBounding().Contains(worldBounding))
                    {
                        mEffect->SetTechnique(mPointAsQuad);
                        _Set(rm.GetUnitQuadMesh().Get());
                        _DoPasses(rm.GetUnitQuadMesh().Get());
                    }
                    else
                    {
                        Matrix4 m = Matrix4::CreateScale(light->GetRange()) * light->GetWorldTransform();

                        jz_Projection.Set(rm.GetProjection());
                        jz_View.Set(rm.GetView());
                        jz_World.Set(m);

                        mEffect->SetTechnique(mPoint);
                        _Set(rm.GetUnitSphereMesh().Get());
                        _DoPasses(rm.GetUnitSphereMesh().Get());
                    }
                }
                else if (light->GetType() == LightNodeType::kSpot)
                {
                    const BoundingSphere& worldBounding = rm.GetWorldFrustumBoundingSphere();
                    if (light->GetWorldFrustum().Test(worldBounding) == Geometric::kContains)
                    {
                        mEffect->SetTechnique(mSpotAsQuad);
                        _Set(rm.GetUnitQuadMesh().Get());
                        _DoPasses(rm.GetUnitQuadMesh().Get());
                    }
                    else
                    {
                        float t = Tan(0.5f * light->GetFalloffAngle()) * light->GetRange();
                        Matrix4 m = Matrix4::CreateScale(t, t, light->GetRange()) * light->GetWorldTransform();

                        jz_Projection.Set(rm.GetProjection());
                        jz_View.Set(rm.GetView());
                        jz_World.Set(m);

                        if (light->GetShadowHandle() >= 0)
                        {
                            Vector2 nf;
                            Matrix4::ExtractNearFarDirectX(light->GetShadowProjection(), nf.X, nf.Y);

                            jz_ShadowTexture.Set(sm.GetShadowTarget());
                            jz_ShadowDelta.Set(sm.GetShadowDelta());
                            jz_ShadowNearFar.Set(nf);
                            jz_ShadowScaleShift.Set(sm.GetScaleShift(light->GetShadowHandle()));
                            jz_ShadowTransform.Set((rm.GetInverseView() * light->GetShadowTransform() * sm.GetShadowPostTransform()));
                            jz_ShadowBleedReduction.Set(sm.GetShadowBleedReduction());

                            mEffect->SetTechnique(mSpotWithShadow);
                        }
                        else
                        {
                            mEffect->SetTechnique(mSpot);
                        }

                        _Set(rm.GetUnitFrustumMesh().Get());
                        _DoPasses(rm.GetUnitFrustumMesh().Get());
                    }
                }
            }
            mLights.clear();
        }

        void Deferred::_Ldr()
        {
            using namespace graphics;

            RenderMan& rm = RenderMan::GetSingleton();

            Target::ResetTarget(0u);
            jz_MrtTexture0.Set(mTargets[0]);

            mEffect->SetTechnique(mLdrPass);
            _Set(rm.GetUnitQuadMesh().Get());
            _DoPasses(rm.GetUnitQuadMesh().Get());
        }

        void Deferred::_InitPost()
        {
            using namespace graphics;

            Graphics& gd = Graphics::GetSingleton();
            RenderMan& rm = RenderMan::GetSingleton();

            float width = (float)gd.GetViewportWidth();
            float height = (float)gd.GetViewportHeight();

            Vector2 nf;
            Matrix4::ExtractNearFarDirectX(rm.GetProjection(), nf.X, nf.Y);

            float fly = 1.0f / Tan(Matrix4::ExtractFov(rm.GetProjection()) * 0.5f);
            float flx = fly * (width / height);

            jz_Projection.Set(Matrix4::kIdentity);
            jz_View.Set(Matrix4::kIdentity);
            jz_World.Set(Matrix4::kIdentity);

            jz_Gamma.Set(rm.GetGamma());
            jz_CameraFocalLength.Set(Vector2(flx, fly));
            jz_ScreenDimensions.Set(Vector2(width, height));

            jz_RandomVectors.Set<kCircularTaps>(mRandomVectors);
            jz_GaussianWeights.Set<kGaussianKernelRadius>(mGaussianWeights);

            jz_AoRadius.Set(mAoRadius);
            jz_AoScale.Set(mAoScale);
            jz_bDebugAO.Set(mbDebugAO);

            jz_MrtTexture1.Set(mTargets[1]);
            jz_MrtTexture2.Set(mTargets[2]);

            _Set(rm.GetUnitQuadMesh().Get());
        }

        void Deferred::_PreTransparency()
        {
            _InitPost();
            _Ao();
            _DeferredLighting();
        }

        void Deferred::_End()
        {
            _InitPost();
            _Ldr();
        }

    }
}