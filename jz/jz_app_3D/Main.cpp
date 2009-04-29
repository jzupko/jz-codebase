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

// #define DEBUG_PHYSICS 1
// #define DEBUG_SHADOWS 1
#define DEFERRED 1

#include <jz_core/Logger.h>
#include <jz_core/Region.h>
#include <jz_engine_3D/AnimatedMeshNode.h>
#include <jz_engine_3D/CameraFPSNode.h>
#include <jz_engine_3D/Deferred.h>
#include <jz_engine_3D/LightNode.h>
#include <jz_engine_3D/IRenderable.h>
#include <jz_engine_3D/IShadowable.h>
#include <jz_engine_3D/MeshNode.h>
#include <jz_engine_3D/PhysicsNode.h>
#include <jz_engine_3D/RenderMan.h>
#include <jz_engine_3D/SceneNode.h>
#include <jz_engine_3D/SceneReader.h>
#include <jz_engine_3D/ShadowMan.h>
#include <jz_engine_3D/StandardEffect.h>
#include <jz_graphics/Effect.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics/Material.h>
#include <jz_graphics/Mesh.h>
#include <jz_graphics/Texture.h>
#include <jz_physics/narrowphase/Body.h>
#include <jz_physics/narrowphase/collision/SphereShape.h>
#include <jz_sail/ThreePointLighting.h>
#include <jz_system/Files.h>
#include <jz_system/Input.h>
#include <jz_system/Loader.h>
#include <jz_system/System.h>
#include <jz_system/Time.h>
#include <jz_core/StringUtility.h>

#if JZ_PLATFORM_WINDOWS
#   include <jz_system/Win32.h>
#   ifndef NDEBUG
#       include <crtdbg.h>
#   endif
    
    static void ActivateAnimation(::jz::engine_3D::AnimatedMeshNode* p)
    {
        p->SetNonDeferred(true);
        p->GetAnimationControl()->SetStartIndex(10);
        p->GetAnimationControl()->SetEndIndex(92);
        p->GetAnimationControl()->SetPlay(true);
    }

    static void SetThreePoint(::jz::engine_3D::ThreePoint tp, ::jz::engine_3D::AnimatedMeshNode* p)
    {
        p->GetEffect()->SetThreePoint(tp);
    }

    static void GatherLights(::jz::BoundingSphere& aSphere, ::jz::engine_3D::LightNode* p, ::std::vector<::jz::sail::ThreePointLighting::MotivatingLight>& arOut)
    {
        if (p->GetWorldBounding().Intersects(aSphere))
        {
            ::jz::sail::ThreePointLighting::MotivatingLight light;
            light.Attenuation = p->GetAttenuation();
            light.Diffuse = ::jz::ColorRGB(p->GetColor().X, p->GetColor().Y, p->GetColor().Z);
            light.Direction = p->GetWorldLightDirection();
            light.Position = p->GetWorldTranslation();
            light.Specular = light.Diffuse;
            light.SpotCosHalfAngle = p->GetFalloffCosHalfAngle();
            light.SpotExponent = p->GetFalloffExponent();
            light.Type = (::jz::sail::ThreePointLighting::MotivatingLight::LightType)p->GetType();
            arOut.push_back(light);
        }
    }

    static void ActivateShadowing(::jz::engine_3D::LightNode* p)
    {
        if (p->GetType() == ::jz::engine_3D::LightNodeType::kSpot)
        {
            p->SetCastShadow(true);
        }
#       pragma region Temporarily disable point lights
        else
        {
            //p->SetColor(jz::Vector3::kZero);
            //p->Update(jz::Matrix4::kIdentity, true);
        }
#       pragma endregion
    }

    static void Light(float* apMaxRange, ::jz::engine_3D::LightNode* apLight, ::jz::engine_3D::IShadowable* p)
    {
        if (apLight->GetWorldBounding().Intersects(p->GetAABB()))
        {
            jz::BoundingBox aabb = p->GetAABB();
            float d = jz::Vector3::Distance(apLight->GetWorldTranslation(), p->GetAABB().Center());
            jz::Vector3 n = jz::Vector3::Normalize(apLight->GetWorldTranslation() - p->GetAABB().Center());
            float r = aabb.EffectiveRadius(n);

            (*apMaxRange) = jz::Max(*apMaxRange, (d + r));

            if (apLight->GetWorldFrustum().Test(p->GetAABB()) != ::jz::Geometric::kDisjoint)
            {
                p->PoseForShadow(apLight);
            }
        }
    }

    static void Lighter(const ::jz::Region& aViewFrustum, const ::jz::engine_3D::SceneNodePtr& apRoot, ::jz::engine_3D::LightNode* p)
    {
        if (p->GetShadowHandle() >= 0)
        {
            if (aViewFrustum.Test(p->GetWorldBounding()) != jz::Geometric::kDisjoint)
            {
                float maxRange = jz::Constants<float>::kMin;

                apRoot->Apply<jz::engine_3D::IShadowable>(std::tr1::bind(Light, &maxRange, p, std::tr1::placeholders::_1));

                if (!jz::AboutEqual(maxRange, p->GetRange()))
                {
                    p->SetRange(maxRange);
                    p->Update(jz::Matrix4::kIdentity, true);
                }
            }
        }
    }

    static bool KeyHandler(jz::system::Key::Code aCode, jz::system::Key::State aState)
    {
        if (aCode == jz::system::Key::kEscape && aState == jz::system::Key::kReleased)
        {
            bool bFullscreen = (jz::graphics::Graphics::GetSingleton().GetFullscreen());
            if (bFullscreen)
            {
                jz::graphics::Graphics::GetSingleton().SetFullscreen(false);

                return true;
            }
        }

        return false;
    }

    static void Poser(const ::jz::Region& aFrustum, ::jz::engine_3D::IRenderable* p)
    {
        if (aFrustum.Test(p->GetAABB()) != ::jz::Geometric::kDisjoint)
        {
            p->PoseForRender();
        }
    }

#   ifdef DEBUG_PHYSICS
        static void HideMeshNodes(::jz::engine_3D::MeshNode* p)
        {
            p->SetVisible(false);
        }
#   endif


    int WINAPI WinMain(HINSTANCE aWindowInstance, HINSTANCE, LPSTR, int)
    {
#       ifndef NDEBUG
            _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#       endif
     
        try
        {
            MSG msg; 
            memset(&msg, 0, sizeof(MSG));

            {
                using namespace jz;
                using namespace jz::engine_3D;
                using namespace jz::graphics;
                using namespace jz::physics;
                using namespace jz::sail;
                using namespace jz::system;
                
                System system(aWindowInstance);
                {
                    Input& input = Input::GetSingleton();
                    Input::KeyEvent::Entry key;
                    input.OnKey.Add<KeyHandler>(key);

                    string origDir = Files::GetWorkingDirectory();
                    string modulePath = Files::ExtractPath(Files::GetCurrentProcessFilename());

                    Scoped workingDir(
                        tr1::bind(Files::SetWorkingDirectory, modulePath),
                        tr1::bind(Files::SetWorkingDirectory, origDir));

                    Files& files = Files::GetSingleton();
					files.AddArchive(new FileArchive("..\\media\\compiled"));

                    Loader loader;
                    Graphics graphics;
                    {
                        RenderMan man;

#                       ifdef DEFERRED
                            ShadowMan::GetSingleton().SetShadowBleedReduction(0.05f);
                            Deferred::GetSingleton().SetActive(true);
#                       endif

                        SceneNodePtr pWoman(LoadScene("1930\\woman.scn"));
                        pWoman->SetLocalTransform(Matrix4::CreateScale(0.01f));

                        //files.AddArchive(new ZipArchive("compiled.zip"));
                        SceneNodePtr pRoot(LoadScene("1930\\1930_room.scn"));
                        pWoman->SetParent(pRoot.Get());
                        pWoman->Apply<AnimatedMeshNode>(ActivateAnimation);

                        pRoot->Apply<LightNode>(ActivateShadowing);
                        PhysicsNodePtr pPhysicsNode = SceneNode::Get<PhysicsNode>("1930_room_physics_node");
                        CameraFPSNodePtr pCamera(new CameraFPSNode());
                        float ar = ((float)graphics.GetViewportWidth()) / ((float)graphics.GetViewportHeight());
                        pCamera->SetWorldTranslation(Vector3::kUp + Vector3::kRight);
                        pCamera->SetProjection(Matrix4::CreatePerspectiveFieldOfViewDirectX(Radian::kPiOver2, ar, 0.099f, 22.63f));
                        pCamera->SetParent(pRoot.Get());
                        pCamera->SetActive(true);
                        Body3DPtr pCameraBody;

                        if (pPhysicsNode.IsValid())
                        {
                            World3D* pWorld = pPhysicsNode->GetWorld();
                            pWorld->SetGravity(Vector3::kZero);
                            pCameraBody = pWorld->Create(new SphereShape(0.2f), Body3D::kDynamic, Body3D::kStatic);
                            pCameraBody->SetTranslation(pCamera->GetWorldTranslation());
                            pCameraBody->SetFriction(0.0f);

    #                       ifdef DEBUG_PHYSICS
                                pPhysicsNode->SetDebugPhysics(true);
                                pRoot->Apply<MeshNode>(HideMeshNodes);
                                man.SetClearColor(ColorRGBA(1, 0, 0, 1));
    #                       endif
                        }

                        ThreePointLighting lighting;
                        lighting.GetLightLearner().Load("..\\media\\woman.dat");
                        ImageIlluminationMetrics ideal;
                        ideal.Entropy = 1.0f;
                        ideal.MaxIntensity = 1.0f;
                        ideal.Roll = 0.25f;
                        ideal.Yaw = 0.5f;
                        lighting.SetIdeal(ideal);

                        MSG msg; 
                        memset(&msg, 0, sizeof(MSG));
                        while (msg.message != WM_QUIT)
                        {
                            if (PeekMessage(&msg, null, 0u, 0u, PM_REMOVE))
                            {
                                TranslateMessage(&msg);
                                DispatchMessage(&msg);
                            }
                            else
                            {
                                #pragma region Update
                                Time::GetSingleton().Tick();
                                float t = Time::GetSingleton().GetElapsedSeconds();
                                loader.Tick();
                                if (t > Constants<float>::kZeroTolerance)
                                {
                                    if (pCameraBody.IsValid())
                                    {
                                        Vector3 prev = (pCamera->GetWorldTranslation());
                                        pCamera->Update();
                                        Vector3 lv = (pCamera->GetWorldTranslation() - prev) / t;
                                        if (lv.LengthSquared() > Constants<float>::kZeroTolerance)
                                        {
                                            pCameraBody->SetVelocity(lv);
                                        }
                                        else
                                        {
                                            pCameraBody->SetVelocity(Vector3::kZero);
                                        }
                                    }
                                    pRoot->Update();
                                    if (pCameraBody.IsValid())
                                    {
                                        pCamera->SetWorldTranslation(pCameraBody->GetTranslation());
                                        pCamera->Update();
                                    }
                                }

                                {
                                    ThreePointLighting::LightSettings settings;
                                    vector<ThreePointLighting::MotivatingLight> mot;
                                    pRoot->Apply<LightNode>(bind(GatherLights, pWoman->GetWorldBounding(), tr1::placeholders::_1, tr1::ref(mot)));
                                    lighting.Tick(Time::GetSingleton().GetElapsedSeconds(),
                                        man.GetInverseView(),
                                        pWoman->GetWorldBounding(),
                                        mot,
                                        settings);

                                    ThreePoint tp;
                                    tp.BackDiffuse = settings.BackDiffuse;
                                    tp.BackPosition = settings.BackTransform.GetTranslation();
                                    tp.BackSpecular = settings.BackSpecular;
                                    tp.FillDirection = Vector3::TransformDirection(settings.FillTransform, Vector3::kForward);
                                    tp.KeyDiffuse = settings.KeyDiffuse;
                                    tp.KeyPosition = settings.KeyTransform.GetTranslation();
                                    tp.KeySpecular = settings.KeySpecular;
                                    tp.KeyToFillRatio = !AboutZero(settings.KeyDiffuse.R) ? (settings.FillDiffuse.R / settings.KeyDiffuse.R) : 0.0f;
                                    pWoman->Apply<AnimatedMeshNode>(tr1::bind(SetThreePoint, tp, tr1::placeholders::_1));
                                }
                                #pragma endregion

                                #pragma region Pose
                                Region frustum(-man.GetView().GetTranslation(), man.GetView() * man.GetProjection());
                                pRoot->Apply<IRenderable>(tr1::bind(Poser, frustum, tr1::placeholders::_1));
                                pRoot->Apply<LightNode>(tr1::bind(Lighter, frustum, pRoot, tr1::placeholders::_1));
                                #pragma endregion

                                #pragma region Draw
                                man.Render();
                                #pragma endregion
                            }
                        }
                    }
                }
            }
        }
        catch (std::exception& e)
        {
            jz::LogMessage(e.what(), jz::Logger::kError);
        }

        return 0;
    }

   
#endif
