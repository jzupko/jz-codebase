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
// #define DEBUG_DEFERRED 1

#define DISABLE_AUTO_CHARACTER_LIGHTING 0
#define ALLOW_DEFERRED_LIGHTING_TO_AFFECT_AUTO_LIT_CHARACTER 0
#define START_FULLSCREEN 0
#define ADD_CAMERA_LIGHT 0
#define DISABLE_ENVIRONMENT_LIGHTS 0
#define ADD_MIRROR 1

// Note: deferred must currently always be enabled. Forward rendering has not been completely implemented.
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
#include <jz_engine_3D/PickMan.h>
#include <jz_engine_3D/ReflectivePlaneNode.h>
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
#       if !DISABLE_AUTO_CHARACTER_LIGHTING
#           if !ALLOW_DEFERRED_LIGHTING_TO_AFFECT_AUTO_LIT_CHARACTER
                p->SetNonDeferred(true);
#           endif
#       endif

        p->GetAnimationControl()->SetStartIndex(10);
        p->GetAnimationControl()->SetEndIndex(92);
        p->GetAnimationControl()->SetPlay(true);
    }

    // This function sets the animated character's threepoint lighting settings.
    // Note: this only works because the animated character is unique in the scene - if there 
    //       were multiple characters, they would share the same effect which means the three point 
    //       settings would be shared between them. The proper way to do this would be to insert
    //       a function in the node's render tree using SetPostEffectFunc
    static void SetThreePoint(::jz::engine_3D::ThreePoint tp, ::jz::engine_3D::AnimatedMeshNode* p)
    {
#       if !DISABLE_AUTO_CHARACTER_LIGHTING
        p->SetThreePoint(tp);
#       endif
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

    // Compensates for lack of forward pipeline by setting transparent object's threepoint lighting rig.
    static void SetThreePointTransparents(::jz::engine_3D::SceneNode* pRootNode, ::jz::engine_3D::MeshNode* p)
    {
        if (p->GetEffect()->IsTransparent() && !p->GetEffect()->IsAlpha1Bit())
        {
            // Eww.
            std::vector<::jz::sail::ThreePointLighting::MotivatingLight> mot;
            pRootNode->Apply<jz::engine_3D::LightNode>(std::tr1::bind(GatherLights, p->GetWorldBounding(), std::tr1::placeholders::_1, std::tr1::ref(mot)));

            ::jz::sail::ThreePointLighting tpl;
            ::jz::sail::ThreePointLighting::LightSettings settings;
            tpl.Tick(jz::Matrix4::kIdentity, p->GetWorldBounding(), mot, settings);

            jz::engine_3D::ThreePoint tp;
            tp.BackDiffuse = settings.BackDiffuse;
            tp.BackPosition = settings.BackTransform.GetTranslation();
            tp.BackSpecular = settings.BackSpecular;
            tp.FillDirection = jz::Vector3::TransformDirection(settings.FillTransform, jz::Vector3::kForward);
            tp.KeyDiffuse = settings.KeyDiffuse;
            tp.KeyPosition = settings.KeyTransform.GetTranslation();
            tp.KeySpecular = settings.KeySpecular;
            tp.KeyToFillRatio = !jz::AboutZero(settings.KeyDiffuse.R) ? (settings.FillDiffuse.R / settings.KeyDiffuse.R) : 0.0f;
            p->SetThreePoint(tp);
        }
    }

    static void ActivateShadowing(::jz::engine_3D::LightNode* p)
    {
#       if DISABLE_ENVIRONMENT_LIGHTS
            p->SetColor(::jz::Vector3::kZero);
#       else
            if (p->GetType() == ::jz::engine_3D::LightNodeType::kSpot)
            {
                p->SetCastShadow(true);
            }
#       endif
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
                }
            }
        }
    }

    static void Reflect(const ::jz::Region& aReflectedFrustum, ::jz::engine_3D::ReflectivePlaneNode* apNode, ::jz::engine_3D::IReflectable* p)
    {
        if (aReflectedFrustum.Test(p->GetAABB()) != jz::Geometric::kDisjoint)
        {
            p->PoseForReflection(apNode);
        }
    }

    static void Reflector(const ::jz::Region& aViewFrustum, const ::jz::engine_3D::SceneNodePtr& apRoot, ::jz::engine_3D::ReflectivePlaneNode* p)
    {
        if (p->GetReflectionHandle() >= 0)
        {
            if (aViewFrustum.Test(p->GetWorldBounding()) != jz::Geometric::kDisjoint)
            {
                const ::jz::Plane& plane = p->GetReflectionPlane();
                size_t size = aViewFrustum.Planes.size();
                ::jz::Matrix4 m = ::jz::Matrix4::CreateReflection(plane);

                ::jz::Region reflectedFrustum(::jz::Vector3::TransformPosition(m, aViewFrustum.Center), aViewFrustum.Planes.size());

                for (size_t i = 0u; i < size; i++)
                {
                    reflectedFrustum.Planes[i] = ::jz::Plane::Transform(m, aViewFrustum.Planes[i]);
                }

                apRoot->Apply<::jz::engine_3D::IReflectable>(std::tr1::bind(Reflect, reflectedFrustum, p, std::tr1::placeholders::_1));
            }
        }
    }

    // Called "control term" to allow for testing different shadow techniques.
    // Currently, a scaling factor for an exp() depth biasing scheme.
    // This is currently a global term. Eventually, it should be attached to individal LightNodes.
    static const float kShadowControlStep = 0.01f;
    static const float kGaussianStdDevStep = 0.1f;
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
        else if (aCode == jz::system::Key::kUp && aState == jz::system::Key::kPressed)
        {
            jz::engine_3D::Deferred::GetSingleton().SetShadowControlTerm(
                jz::engine_3D::Deferred::GetSingleton().GetShadowControlTerm() + kShadowControlStep);
        }
        else if (aCode == jz::system::Key::kDown && aState == jz::system::Key::kPressed)
        {
            jz::engine_3D::Deferred::GetSingleton().SetShadowControlTerm(
                jz::engine_3D::Deferred::GetSingleton().GetShadowControlTerm() - kShadowControlStep);
        }
        else if (aCode == jz::system::Key::kRight && aState == jz::system::Key::kPressed)
        {
            jz::engine_3D::Deferred::GetSingleton().SetGaussianKernelStdDev(
                jz::engine_3D::Deferred::GetSingleton().GetGaussianKernelStdDev() + kGaussianStdDevStep);
        }
        else if (aCode == jz::system::Key::kLeft && aState == jz::system::Key::kPressed)
        {
            jz::engine_3D::Deferred::GetSingleton().SetGaussianKernelStdDev(
                jz::engine_3D::Deferred::GetSingleton().GetGaussianKernelStdDev() - kGaussianStdDevStep);
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

    namespace jz
    {
        static engine_3D::SceneNode* gspRoot = null;
        static Vector3 gsLocalPickPoint = Vector3::kZero;
        static float gsPickedDepth = 0.0f;
        static engine_3D::MeshNode* gspPickedNode = null;

        static void PickHelper(engine_3D::MeshNode* p, const Ray3D& ray)
        {
            float distance;
            if (ray.Intersects(p->GetAABB(), distance))
            {
                p->Pick(ray);
            }
        }

        static void PickMoveHandler(natural x, natural y)
        {
            if (gspPickedNode)
            {
                Vector3 vp = Vector3::TransformPosition(gspPickedNode->GetWorldTransform(), gsLocalPickPoint);
                Vector3 vpp = engine_3D::PickMan::GetSingleton().Project(vp);
                
                vpp.X = (float)x;
                vpp.Y = (float)y;

                Vector3 up = engine_3D::PickMan::GetSingleton().UnProject(vpp);

                gspPickedNode->SetWorldTranslation(
                    gspPickedNode->GetWorldTranslation() + (up - vp));
            }
        }

        static bool PickHandler(system::Mouse::Button aButton, system::Mouse::State aState)
        {
            using namespace engine_3D;
            using namespace system;
            
            if (aButton == Mouse::kRight)
            {
                if (aState == Mouse::kPressed && !gspPickedNode)
                {
                    natural x = Input::GetSingleton().GetMouseX();
                    natural y = Input::GetSingleton().GetMouseY();

                    Ray3D ray = PickMan::GetSingleton().GetPickingRay(x, y);

                    gspRoot->Apply<MeshNode>(tr1::bind(PickHelper, tr1::placeholders::_1, ray));

                    #pragma region Render and handle pick
                    {
                        IPickable* p;
                        float depth;
                        RectangleU rect;
                        rect.Top = y;
                        rect.Bottom = (y + 1);
                        rect.Left = x;
                        rect.Right = (x + 1);

                        if (PickMan::GetSingleton().Pick(rect, p, depth))
                        {
                            gspPickedNode = dynamic_cast<engine_3D::MeshNode*>(p);

                            if (gspPickedNode)
                            {
                                gsPickedDepth = depth;

                                Vector3 v = Vector3((float)x, (float)y, depth);
                                Vector3 wv = PickMan::GetSingleton().UnProject(v);
                                gsLocalPickPoint = Vector3::TransformPosition(Matrix4::Invert(gspPickedNode->GetWorldTransform()), wv);
                            }
                        }
                    }
                    #pragma endregion
                }
                else
                {
                    gsLocalPickPoint = Vector3::kZero;
                    gspPickedNode = null;
                    gsPickedDepth = 0.0f;
                }

                return true;
            }

            return false;
        }
    }

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
#if START_FULLSCREEN
                    Graphics graphics(true);
#else
                    Graphics graphics;
#endif
                    {
                        RenderMan man;
                        PickMan pm;

                        Input::ButtonEvent::Entry pickConnection;
                        Input::MouseMoveEvent::Entry pickMoveConnection;

                        input.OnButton.Add<PickHandler>(pickConnection);
                        input.OnMouseMoveAbsolute.Add<PickMoveHandler>(pickMoveConnection);

#                       ifdef DEFERRED
                            Deferred::GetSingleton().SetActive(true);
                            Deferred::GetSingleton().SetBloomThreshold(0.6f);
                            Deferred::GetSingleton().SetMotionBlurAmount(0.34f);

#                           ifdef DEBUG_DEFERRED
                                Deferred::GetSingleton().SetDebugDeferredLighting(true);
#                           endif
#                       endif

                        const float kWomanMovement = 0.5f;
                        const float kWomanMax = 4.0f;
                        const float kWomanMin = -6.0f;
                        const float kWomanScale = 0.016f;
                        float womanPosition = 0.0f;
                        float womanDirection = -1.0f;
                        SceneNodePtr pWoman(LoadScene("1930\\woman.scn"));
                        SceneNodePtr pArrowKey(LoadScene("1930\\arrow.scn"));
                        SceneNodePtr pArrowFill(pArrowKey->Clone<SceneNode>(null));

                        //files.AddArchive(new ZipArchive("compiled.zip"));
                        SceneNodePtr pRoot(LoadScene("1930\\1930_room.scn"));
                        jz::gspRoot = pRoot.Get();
                        pWoman->SetParent(pRoot.Get());
                        pWoman->Apply<AnimatedMeshNode>(ActivateAnimation);

                        pArrowKey->SetParent(pRoot.Get());
                        pArrowFill->SetParent(pRoot.Get());

                        {
                            pRoot->Update();
                            pRoot->Apply<MeshNode>(tr1::bind(SetThreePointTransparents, pRoot.Get(), tr1::placeholders::_1));
                        }

                        pRoot->Apply<LightNode>(ActivateShadowing);
                        PhysicsNodePtr pPhysicsNode = SceneNode::Get<PhysicsNode>("1930_room_physics_node");
                        CameraFPSNodePtr pCamera(new CameraFPSNode());

#                       if ADD_CAMERA_LIGHT
                        {
                            LightNodePtr pCameraLight(new LightNode());
                            pCameraLight->SetParent(pCamera.Get());
                            pCameraLight->SetType(LightNodeType::kSpot);
                            pCameraLight->SetAttenuation(Vector3(0, 0, 1));
                            pCameraLight->SetCastShadow(true);
                            pCameraLight->SetColor(Vector3::kOne * 2.5f);
                            pCameraLight->SetFalloffAngle(Degree(90.0));
                            pCameraLight->SetFalloffExponent(10.0f);
                            pCameraLight->SetLocalTranslation(Vector3::kBackward * 0.25f + Vector3::kRight * 0.25f);
                        }   
#                       endif

#                       if ADD_MIRROR
                        {
                            ReflectivePlaneNodePtr pMirror(new ReflectivePlaneNode());
                            pMirror->SetReflectionPlane(Plane(Vector3::kBackward, 0.0f));
                            pMirror->SetParent(pRoot.Get());
                            pMirror->SetEffect(graphics.Create<StandardEffect>("..\\media\\mirror.cfx"));
                            pMirror->SetMesh(RenderMan::GetSingleton().GetUnitQuadMesh());
                        }
#                       endif

                        float ar = ((float)graphics.GetViewportWidth()) / ((float)graphics.GetViewportHeight());
                        pCamera->SetWorldTranslation(Vector3::kUp + Vector3::kRight);
                        pCamera->SetProjection(Matrix4::CreatePerspectiveFieldOfViewDirectX(Radian::kPiOver2, ar, 0.099f, 22.63f));
                        pCamera->SetParent(pRoot.Get());
                        pCamera->SetMoveRate(2.0f);
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
                        ideal.Entropy = 0.09282619f;
                        ideal.MaxIntensity = 0.937254965f;
                        ideal.Roll = 0.7495458f;
                        ideal.Yaw =0.8952829f;
                        lighting.SetIdeal(ideal);

                        // Temp: FPS
                        float timeDelta = 0.0f;
                        float fps = 0.0f;
                        unatural frameCount = 0u;
                        // End Temp:

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

                                // FPS
                                timeDelta += t;
                                frameCount++;
                                if (timeDelta > 1.0f)
                                {
                                    fps = (frameCount / timeDelta);
                                    timeDelta = 0.0f;
                                    frameCount = 0u;
                                }
                                // End

                                womanPosition += (kWomanMovement * womanDirection * t);
                                if (womanPosition < kWomanMin) { womanPosition = kWomanMin; womanDirection = -womanDirection; }
                                if (womanPosition > kWomanMax) { womanPosition = kWomanMax; womanDirection = -womanDirection; }

                                pWoman->SetLocalTransform(
                                    Matrix4::CreateScale(kWomanScale) * 
                                    Matrix4::CreateRotationY((womanDirection > 0.0f) ? Radian::kZero : Radian::kPi) * 
                                    Matrix4::CreateTranslation(Vector3(1, 0, womanPosition)));

                                {
                                    ThreePointLighting::LightSettings settings;
                                    vector<ThreePointLighting::MotivatingLight> mot;
                                    pRoot->Apply<LightNode>(bind(GatherLights, pWoman->GetWorldBounding(), tr1::placeholders::_1, tr1::ref(mot)));
                                    if (lighting.Tick(man.GetInverseView(),
                                        pWoman->GetWorldBounding(),
                                        mot,
                                        settings))
                                    {
                                        pArrowKey->SetWorldTransform(Matrix4::CreateScale(0.3f) * settings.KeyTransform);
                                        pArrowFill->SetWorldTransform(Matrix4::CreateScale(0.3f) * settings.FillTransform);

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
                                }

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
                                    
                                    if (!Vector3::AboutEqual(pCamera->GetWorldTranslation(), pCameraBody->GetTranslation(), Constants<float>::kLooseTolerance))
                                    {
                                        // Ewwwwww.
                                        pCamera->SetWorldTranslation(pCameraBody->GetTranslation());
                                        float moveRate = pCamera->GetMoveRate();
                                        pCamera->SetMoveRate(0.0f);
                                        pCamera->Update();
                                        pCamera->SetMoveRate(moveRate);
                                    }
                                }
                                #pragma endregion

                                #pragma region Pose
                                Region frustum(-man.GetView().GetTranslation(), man.GetView() * man.GetProjection());
                                pRoot->Apply<IRenderable>(tr1::bind(Poser, frustum, tr1::placeholders::_1));
                                pRoot->Apply<LightNode>(tr1::bind(Lighter, frustum, pRoot, tr1::placeholders::_1));
                                pRoot->Apply<ReflectivePlaneNode>(tr1::bind(Reflector, frustum, pRoot, tr1::placeholders::_1));
                                #pragma endregion

                                #pragma region Debug text
                                // man.AddConsoleLine("FPS: " + StringUtility::ToString(fps));
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

            MessageBox(jz::null,
                e.what(),
                "Exception",
                MB_OK | MB_ICONERROR | MB_TASKMODAL);
        }

        return 0;
    }

   
#endif
