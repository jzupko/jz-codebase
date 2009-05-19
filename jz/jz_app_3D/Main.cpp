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
#define ADD_MIRROR 0
#define ADD_GUI 0
#define FPS_COUNTER 1
#define ADD_RIGID_BODIES 0

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
#include <jz_engine_3D/RigidBodyNode.h>
#include <jz_engine_3D/SceneNode.h>
#include <jz_engine_3D/SceneReader.h>
#include <jz_engine_3D/ShadowMan.h>
#include <jz_engine_3D/SimpleEffect.h>
#include <jz_engine_3D/StandardEffect.h>
#include <jz_graphics/Effect.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics/Material.h>
#include <jz_graphics/Mesh.h>
#include <jz_graphics/Texture.h>
#include <jz_gui/Font.h>
#include <jz_gui/GuiImage.h>
#include <jz_gui/GuiImageEffect.h>
#include <jz_gui/GuiMan.h>
#include <jz_physics/narrowphase/Body.h>
#include <jz_physics/narrowphase/collision/Collide.h>
#include <jz_physics/narrowphase/collision/SphereShape.h>
#include <jz_sail/ThreePointLighting.h>
#include <jz_system/Files.h>
#include <jz_system/Input.h>
#include <jz_system/Loader.h>
#include <jz_system/Profiler.h>
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
        if (p->GetBoundingSphere().Intersects(aSphere))
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
            pRootNode->Apply<jz::engine_3D::LightNode>(std::tr1::bind(GatherLights, p->GetBoundingSphere(), std::tr1::placeholders::_1, std::tr1::ref(mot)));

            ::jz::sail::ThreePointLighting tpl;
            ::jz::sail::ThreePointLighting::LightSettings settings;
            tpl.Tick(jz::Matrix4::kIdentity, p->GetBoundingSphere(), mot, settings);

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
        if (p->bCastShadow())
        {
            if (apLight->GetBoundingSphere().Intersects(p->GetBoundingBox()))
            {
                const jz::BoundingBox& aabb = p->GetBoundingBox();
                float d = jz::Vector3::Distance(apLight->GetWorldTranslation(), p->GetBoundingBox().Center());
                jz::Vector3 n = jz::Vector3::Normalize(apLight->GetWorldTranslation() - p->GetBoundingBox().Center());
                float r = aabb.EffectiveRadius(n);

                (*apMaxRange) = jz::Max(*apMaxRange, (d + r));

                if (apLight->GetWorldFrustum().Test(p->GetBoundingBox()) != ::jz::Geometric::kDisjoint)
                {
                    p->PoseForShadow(apLight);
                }
            }
        }
    }

    static void Lighter(const ::jz::Region& aViewFrustum, const ::jz::engine_3D::SceneNodePtr& apRoot, ::jz::engine_3D::LightNode* p)
    {
        if (p->GetShadowHandle() >= 0)
        {
            if (aViewFrustum.Test(p->GetBoundingSphere()) != jz::Geometric::kDisjoint)
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
        if (aReflectedFrustum.Test(p->GetBoundingBox()) != jz::Geometric::kDisjoint)
        {
            p->PoseForReflection(apNode);
        }
    }

    static void Reflector(const ::jz::Region& aViewFrustum, const ::jz::engine_3D::SceneNodePtr& apRoot, ::jz::engine_3D::ReflectivePlaneNode* p)
    {
        if (p->GetReflectionHandle() >= 0)
        {
            if (aViewFrustum.Test(p->GetBoundingSphere()) != jz::Geometric::kDisjoint)
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
        if (aFrustum.Test(p->GetBoundingBox()) != ::jz::Geometric::kDisjoint)
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
            if (ray.Intersects(p->GetBoundingBox(), distance))
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
                using namespace jz::gui;
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
                    files.AddArchive(new ZipArchive("media.dat"));

#                   if JZ_MULTITHREADED
                        Loader loader;
#                   endif

#if START_FULLSCREEN
                    Graphics graphics(true);
#else
                    Graphics graphics;
#endif
                    {
                        RenderMan man;
#if ADD_GUI
                        GuiMan gm;
#endif
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
                        PhysicsNodePtr pPhysicsNode = SceneNode::Get<PhysicsNode>("1930_room", "physics_node");
                        CameraFPSNodePtr pCamera(new CameraFPSNode(string(), "main_camera"));
                        pCamera->SetCollisionEnabled(true, 0.2f);
                        float ar = ((float)graphics.GetViewportWidth()) / ((float)graphics.GetViewportHeight());
                        pCamera->SetWorldTranslation(Vector3::kUp + Vector3::kRight);
                        pCamera->SetProjection(Matrix4::CreatePerspectiveFieldOfViewDirectX(Radian::kPiOver2, ar, 0.099f, 22.63f));
                        pCamera->SetMoveRate(2.0f);
                        pCamera->SetActive(true);
                        pCamera->SetParent(pRoot.Get());

#                       if ADD_RIGID_BODIES
                        static const float kBoxScale = 0.2f;
                        static const int kBoxCount = 1;

                        for (int i = 0; i < kBoxCount; i++)
                        {
                            RigidBodyNodePtr pRigidBody = new RigidBodyNode(string(), "box" + StringUtility::ToString(i));
                            pRigidBody->SetParent(pRoot.Get());
                            pRigidBody->SetWorldTranslation(Vector3::kUp * 2.0f + Vector3::kRight + Vector3::kForward);
                            pRigidBody->SetAsPhysicalBox(Vector3::kOne * kBoxScale, 1.0f);
                            pRigidBody->SetEffect(man.GetSimpleEffect());
                            pRigidBody->SetMesh(man.GetUnitBoxMesh());
                            pRigidBody->SetCastShadow(false);
                            pRigidBody->SetScale(Vector3::kOne * kBoxScale);
                        }
#                       endif

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
                            pMirror->SetEffect(graphics.Create<StandardEffect>("mirror.cfx"));
                            pMirror->SetMesh(RenderMan::GetSingleton().GetUnitQuadMesh());
                        }
#                       endif

                        ThreePointLighting lighting;
                        lighting.GetLightLearner().Load("woman.dat");
                        ImageIlluminationMetrics ideal;
                        ideal.Entropy = 0.09282619f;
                        ideal.MaxIntensity = 0.937254965f;
                        ideal.Roll = 0.7495458f;
                        ideal.Yaw =0.8952829f;
                        lighting.SetIdeal(ideal);

#if FPS_COUNTER
                        float timeDelta = 0.0f;
                        float fps = 0.0f;
                        unatural frameCount = 0u;
#endif

#if ADD_GUI
                        float fWidth = (float)graphics.GetViewportWidth();
                        float fHeight = (float)graphics.GetViewportHeight();

                        const wstring kTestSentence = L"Hello World!\nThis is a test!";
                        vector<GuiImagePtr> mLetters;

                        gui::FontPtr pFont(graphics.Create<gui::Font>(48, 64u, gui::Font::kAntiAliased, "Arial.ttf"));

                        GuiImageEffectPtr pGuiEffect(graphics.Create<GuiImageEffect>("GuiImage.cfx"));

                        MaterialPtr pGuiMaterial(graphics.Create<Material>("gui_test_material.mat"));
                        pGuiMaterial->AddParameter("jz_ImageTexture", pFont->GetTexture());
                        pGuiMaterial->AddParameter("jz_Filtered", false);

                        natural xOffset = 0;
                        natural yOffset = 0;
                        size_t fontHeight = pFont->GetFontHeight();
                        for (size_t i = 0u; i < kTestSentence.size(); i++)
                        {
                            if (kTestSentence[i] == '\n')
                            {
                                xOffset = 0;
                                yOffset += (natural)(fontHeight);
                            }
                            else
                            {
                                gui::Font::Entry entry;
                                if (pFont->GetEntry(kTestSentence[i], entry))
                                {
                                    GuiImagePtr pGuiImage(new GuiImage());
                                    pGuiImage->SetEffect(pGuiEffect.Get());
                                    pGuiImage->SetMaterial(pGuiMaterial.Get());
                                    pGuiImage->SetTextureScaleShift(entry.TextureScaleShift);

                                    pGuiImage->SetScale((float)(entry.Width) / fWidth, (float)(entry.Height) / fHeight);
                                    pGuiImage->SetTranslation(((float)(xOffset + entry.XDrawOffset)) / fWidth, ((float)(yOffset + entry.YDrawOffset)) / fHeight);
                                    pGuiImage->SetTransparent(true);

                                    xOffset += entry.XStepSize;
                                    mLetters.push_back(pGuiImage);
                                }
                            }
                        }
#endif

                        Profiler profiler;

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
                                profiler.Begin("Update");

                                Time::GetSingleton().Tick();
                                float t = Time::GetSingleton().GetElapsedSeconds();

#                               if JZ_MULTITHREADED                                
                                    loader.Tick();
#                               endif

#if FPS_COUNTER
                                timeDelta += t;
                                frameCount++;
                                if (timeDelta > 1.0f)
                                {
                                    fps = (frameCount / timeDelta);
                                    timeDelta = 0.0f;
                                    frameCount = 0u;
                                }
#endif

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
                                    pRoot->Apply<LightNode>(bind(GatherLights, pWoman->GetBoundingSphere(), tr1::placeholders::_1, tr1::ref(mot)));
                                    if (lighting.Tick(man.GetInverseView(),
                                        pWoman->GetBoundingSphere(),
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

                                pRoot->Update();
                                profiler.End("Update");
                                #pragma endregion

                                #pragma region Pose
                                profiler.Begin("Pose");
#if ADD_GUI
                                for (size_t i = 0u; i < mLetters.size(); i++)
                                {
                                    mLetters[i]->Pose();
                                }
#endif
                                Region frustum(-man.GetView().GetTranslation(), man.GetView() * man.GetProjection());
                                pRoot->Apply<IRenderable>(tr1::bind(Poser, frustum, tr1::placeholders::_1));
                                pRoot->Apply<LightNode>(tr1::bind(Lighter, frustum, pRoot, tr1::placeholders::_1));
                                pRoot->Apply<ReflectivePlaneNode>(tr1::bind(Reflector, frustum, pRoot, tr1::placeholders::_1));
                                profiler.End("Pose");
                                #pragma endregion

                                #pragma region Debug text
#if FPS_COUNTER
                                man.AddConsoleLine("FPS: " + StringUtility::ToString(fps));
                                man.AddConsoleLine("Avg update time: " + StringUtility::ToString(profiler.GetAverageSeconds("Update")));
                                man.AddConsoleLine("Avg pose time: " + StringUtility::ToString(profiler.GetAverageSeconds("Pose")));
                                man.AddConsoleLine("Avg draw time: " + StringUtility::ToString(profiler.GetAverageSeconds("Draw")));
#if JZ_PROFILING
                                man.AddConsoleLine("Avg distance iterations: " + StringUtility::ToString(Collide::AverageMprDistanceIterations));
                                man.AddConsoleLine("Avg collide iterations: " + StringUtility::ToString(Collide::AverageMprCollideIterations));
                                if (pPhysicsNode.IsValid())
                                {
                                    man.AddConsoleLine("Avg collision pairs: " + StringUtility::ToString(pPhysicsNode->GetWorld()->AverageCollisionPairs));
                                }
#endif
#endif
                                #pragma endregion

                                #pragma region Draw
                                profiler.Begin("Draw");
                                man.Render();
                                profiler.End("Draw");
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
