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

#include <jz_app_2D/Entity.h>
#include <jz_core/Event.h>
#include <jz_core/Logger.h>
#include <jz_core/Matrix4.h>
#include <jz_engine_2D/Cell.h>
#include <jz_engine_2D/DrawMan.h>
#include <jz_engine_2D/Light.h>
#include <jz_engine_2D/Sprite.h>
#include <jz_graphics/Font.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics/Material.h>
#include <jz_graphics/Texture.h>
#include <jz_pathfinding/PathGrid.h>
#include <jz_physics/narrowphase/Body.h>
#include <jz_physics/narrowphase/collision/BoxShape.h>
#include <jz_physics/World.h>
#include <jz_sound/Sound.h>
#include <jz_system/Files.h>
#include <jz_system/Input.h>
#include <jz_system/Loader.h>
#include <jz_system/ReadHelpers.h>
#include <jz_system/System.h>
#include <jz_system/Time.h>

#if JZ_PLATFORM_WINDOWS
#   include <jz_system/Win32.h>
#   ifndef NDEBUG
#       include <crtdbg.h>
#   endif
    
#define JZ_USE_PUFFY 1
#define JZ_USE_STONE 0

#   if JZ_USE_PUFFY
    static const char* kDiffuseFile = "puffy.dds";
    static const char* kNormalDepthFile = "puffy_NRM.dds";
    static const char* kEdgeFile = "puffy_EDGE.dat";
#   elif JZ_USE_STONE
    static const char* kDiffuseFile = "stones.dds";
    static const char* kNormalDepthFile = "stones_NRM.dds";
    static const char* kEdgeFile = "stones_EDGE.dat";
#   else
    static const char* kDiffuseFile = "rocks.dds";
    static const char* kNormalDepthFile = "rocks_NRM.dds";
    static const char* kEdgeFile = "rocks_EDGE.dat";
#   endif

    static const char* kMaterialFile = "..\\media\\sprite_test_material.mat";

    static jz::app_2D::Entity* spEntity = jz::null;
    static const float kMoveRate = 0.5f;
    static jz::pathfinding::PathGrid sPathGrid;
    static std::vector<jz::Vector2> sPath;
    static size_t sPathIndex = 0u;

    static bool ButtonHandler(jz::system::Mouse::Button aButton, jz::system::Mouse::State aState)
    {
        if (aButton == jz::system::Mouse::kLeft)
        {
            if (aState == jz::system::Mouse::kPressed)
            {
                jz::natural x = jz::system::Input::GetSingleton().GetMouseX();
                jz::natural y = jz::system::Input::GetSingleton().GetMouseY();

                float width = (float)jz::graphics::Graphics::GetSingleton().GetViewportWidth();
                float height = (float)jz::graphics::Graphics::GetSingleton().GetViewportHeight();

                float fx =  ((float)x / width) * 2.0f - 1.0f;
                float fy =  (1.0f - ((float)y / height)) * 2.0f - 1.0f;

                if (width < height) { fy *= (height / width); }
                else if (height < width) { fx *= (width / height); }

                fx = jz::Clamp(fx, -1.0f, 1.0f);
                fy = jz::Clamp(fy, -1.0f, 1.0f);

                if (sPathGrid.FindPath(spEntity->GetPathId(), jz::Vector2(fx, fy), sPath))
                {
                    sPathIndex = 0u;
                }
            }

            return true;
        }

        return false;
    }

    static void UpdateMoveable()
    {
        if (sPath.size() > 0u)
        {
            jz::BoundingRectangle rect = spEntity->GetBody()->GetLocalBounding();
            rect.Min += spEntity->GetBody()->GetTranslation();
            rect.Max += spEntity->GetBody()->GetTranslation();

            if (sPathIndex < sPath.size())
            {
                if (jz::Vector2::Distance(rect.Center(), sPath[sPathIndex]) < 1e-2f)
                {
                    sPathIndex++;
                }
            }

            if (sPathIndex < sPath.size())
            {
                jz::Vector2 n = jz::Vector2::Normalize(sPath[sPathIndex] - rect.Center());
                n *= kMoveRate;

                spEntity->GetBody()->SetVelocity(n);
            }
            else
            {
                spEntity->GetBody()->SetVelocity(jz::Vector2::kZero);
            }
        }
        else
        {
            spEntity->GetBody()->SetVelocity(jz::Vector2::kZero);
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

            using namespace jz;
            using namespace jz::app_2D;
            using namespace jz::engine_2D;
            using namespace jz::graphics;
            using namespace jz::pathfinding;
            using namespace jz::physics;
            using namespace jz::sound;
            using namespace jz::system;
            
            System system(aWindowInstance);
            {
                string origDir = Files::GetWorkingDirectory();
                string modulePath = Files::ExtractPath(Files::GetCurrentProcessFilename());

                Scoped workingDir(
                    tr1::bind(Files::SetWorkingDirectory, modulePath),
                    tr1::bind(Files::SetWorkingDirectory, origDir));
             
                Files::GetSingleton().AddArchive(new FileArchive("..\\media\\compiled"));

                Loader loader;
                Graphics graphics;
                {
                    DrawMan dm;
                    Input::ButtonEvent::Entry button;
                    Input::GetSingleton().OnButton.Add<ButtonHandler>(button);
                    
                    World2D world;
                    //world.SetGravity(Vector2::kZero);
                    world.SetUnitMeter(0.1f);

                    Box2DShapePtr boxShape = new Box2DShape(BoundingRectangle(Vector2(-0.1f), Vector2(0.1f)));

                    CellPtr pCell = new Cell();
                    TexturePtr pDiffuse = graphics.Create<Texture>(kDiffuseFile);
                    TexturePtr pNormalDepth = graphics.Create<Texture>(kNormalDepthFile);

                    MaterialPtr pMaterial = graphics.Create<Material>(kMaterialFile);
                    pMaterial->AddParameter("jz_Diffuse", pDiffuse.Get());
                    pMaterial->AddParameter("jz_NormalDepth", pNormalDepth.Get());

                    MaterialPtr pGroundMaterial = graphics.Create<Material>(string());
                    pGroundMaterial->AddParameter("jz_Diffuse", graphics.Create<Texture>("ground.dds"));
                    pGroundMaterial->AddParameter("jz_NormalDepth", graphics.Create<Texture>("ground_NRM.dds"));

                    MemoryBuffer<Vector2> edges;
                    system::IReadFilePtr pEdgeFile = system::Files::GetSingleton().Open(kEdgeFile);
                    system::ReadBuffer(pEdgeFile, edges);
                    pEdgeFile.Reset();

                    SpritePtr pSprite = new Sprite();
                    pCell->AddChild(pSprite.Get());
                    pSprite->SetMaterial(pGroundMaterial.Get());
                    pSprite->SetScale(1.0f, 0.1f);
                    pSprite->SetXYDepthFactors(Vector2(0, 1.0f));
                    pSprite->SetZdepth(0.5f);
                    

                    vector<EntityPtr> entities;
                    for (int i = 0; i < 30; i++)
                    {
                        entities.push_back(new Entity());
                        entities.back()->SetPathGrid(&sPathGrid);
                        entities.back()->SetSprite(new Sprite());
                        pCell->AddChild(entities.back()->GetSprite());
                        entities.back()->GetSprite()->SetScale(0.1f, 0.1f);
                        entities.back()->GetSprite()->SetMaterial(pMaterial.Get());
                        entities.back()->GetSprite()->SetShadowEdges(&edges);
                        entities.back()->GetSprite()->SetZdepth(0.5f);
                        entities.back()->SetBody(world.Create(boxShape.Get(), Body2D::kDynamic, Body2D::kStatic | Body2D::kDynamic));
                        entities.back()->GetBody()->SetMass(0.25f);
                        entities.back()->GetBody()->SetTranslation(Vector2(2.0f * UniformRandomf() - 1.0f, 2.0f * UniformRandomf() - 1.0f));
                    }

                    LightPtr pLight = new Light();
                    pLight->SetType(Light::kPoint);
                    pLight->SetColor(ColorRGB(1, 0, 0));
                    pLight->SetPosition(-1.0f, -1.0f, 0.0f);
                    pLight->SetShadow(true);
                    //pCell->AddChild(pLight.Get());
                    
                    pLight = new Light();
                    pLight->SetType(Light::kPoint);
                    pLight->SetColor(ColorRGB(0, 1, 0));
                    pLight->SetPosition(-1.0f, 1.0f, 0.0f);
                    pLight->SetShadow(true);
                    //pCell->AddChild(pLight.Get());

                    pLight = new Light();
                    pLight->SetType(Light::kPoint);
                    pLight->SetColor(ColorRGB(0, 0, 1));
                    pLight->SetPosition(1.0f, 1.0f, 0.0f);
                    pLight->SetShadow(true);
                    //pCell->AddChild(pLight.Get());

                    pLight = new Light();
                    pLight->SetType(Light::kPoint);
                    pLight->SetColor(ColorRGB::kWhite);
                    pLight->SetPosition(1.0f, 0.0f, 0.0f);
                    pLight->SetShadow(true);
                    pCell->AddChild(pLight.Get());   

                    EntityPtr pEntity = new Entity();
                    pEntity->SetPathGrid(&sPathGrid);
                    pEntity->SetSprite(new Sprite());
                    pCell->AddChild(pEntity->GetSprite());
                    pEntity->GetSprite()->SetScale(0.1f, 0.1f);
                    pEntity->GetSprite()->SetShadowEdges(&edges);
                    pEntity->GetSprite()->SetMaterial(pMaterial.Get());
                    pEntity->GetSprite()->SetZdepth(0.2f);
                    float alpha = 0.0f;
                    float alphaChange = 1.0f;
                    pEntity->GetSprite()->SetAlpha(alpha);

                    Body2DPtr body = world.Create(boxShape.Get(), Body2D::kDynamic, Body2D::kStatic | Body2D::kDynamic);
                    body->SetMass(0.5f);
                    body->SetTranslation(Vector2::kDown * 0.5f);
                    
                    Box2DShapePtr groundShape = new Box2DShape(BoundingRectangle(Vector2(-1.0f, -0.1f), Vector2(1.0f, 0.1f)));
                    Body2DPtr ground = world.Create(groundShape.Get(), Body2D::kStatic, Body2D::kDynamic);
                    ground->SetTranslation(Vector2(0.0f, -1.0f));

                    pEntity->SetBody(body.Get());
                    spEntity = pEntity.Get();

                    FontPtr pFont = graphics.Create<Font>(0, 20, Font::kNone, "Arial");

                    Radian ang = Radian::kZero;

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
                            alpha += alphaChange * Time::GetSingleton().GetElapsedSeconds() * (50.0f / 255.0f);
                            if (alpha < 0.0f || alpha > 1.0f) { alphaChange = -alphaChange; }
                            pEntity->GetSprite()->SetAlpha(alpha);

                            ang += Radian(Time::GetSingleton().GetElapsedSeconds());
                            if (ang > Radian::kTwoPi) { ang = Radian::kZero; }
                            pLight->SetPosition(Cos(ang), Sin(ang));

                            loader.Tick();
                            UpdateMoveable();
                            world.Tick(Time::GetSingleton().GetElapsedSeconds());
                            for (vector<EntityPtr>::iterator I = entities.begin(); I != entities.end(); I++)
                            {
                                (*I)->Update();
                            }
                            pEntity->Update();
                            pCell->Update();
                            #pragma endregion

                            #pragma region Pose
                            pCell->Pose(null);
                            #pragma endregion

                            #pragma region Draw
                            dm.Draw();
                            #pragma endregion
                        }
                    }

                    spEntity = null;
                }
            }
        }
        catch (std::exception& e)
        {
            spEntity = jz::null;
            jz::LogMessage(e.what(), jz::Logger::kError);
        }

        return 0;
    }

   
#endif
