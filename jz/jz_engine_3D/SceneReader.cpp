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

#include <jz_engine_3D/AnimatedMeshNode.h>
#include <jz_engine_3D/JointNode.h>
#include <jz_engine_3D/SceneReader.h>
#include <jz_engine_3D/LightNode.h>
#include <jz_engine_3D/MeshNode.h>
#include <jz_engine_3D/PhysicsNode.h>
#include <jz_engine_3D/SceneNode.h>
#include <jz_engine_3D/StandardEffect.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics/Material.h>
#include <jz_graphics/Mesh.h>
#include <jz_graphics/VertexDeclaration.h>
#include <jz_physics/narrowphase/Body.h>
#include <jz_physics/narrowphase/collision/TriangleTreeShape.h>
#include <jz_system/Files.h>
#include <jz_system/ReadHelpers.h>

namespace jz
{
    namespace engine_3D
    {

        namespace SceneNodeType
        {
            enum Enum
            {
                kNone = 0,
                kAnimatedMesh = 1,
                kCamera = 2,
                kDirectionalLight = 3,
                kJoint = 4,
                kLight = 5,
                kMesh = 6,
                kNode = 7,
                kPhysics = 8,
                kPointLight = 9,
                kSpotLight = 10
            };
        }

        static SceneNode* ReadAnimatedMesh(system::IReadFilePtr& in)
        {
            using namespace graphics;
            using namespace system;

            Graphics& graphics = Graphics::GetSingleton();
            AnimatedMeshNode* ret = new AnimatedMeshNode();

            ret->SetEffect(graphics.Create<StandardEffect>(ReadString(in)));
            ret->SetMaterial(graphics.Create<Material>(ReadString(in)));
            ret->SetMesh(graphics.Create<Mesh>(ReadString(in)));

            ret->SetBindTransform(ReadMatrix4(in));
            ReadBuffer(in, ret->GetInvBindTransforms());

            ret->SetRootJointId(ReadString(in));

            size_t size = ReadSizeT(in);
            vector<string>& ids = ret->GetJointIds();
            
            for (size_t i = 0; i < size; i++)
            {
                ids.push_back(ReadString(in));
            }

            return ret;
        }

        static SceneNode* ReadDirectionalLight(system::IReadFilePtr& in)
        {
            LightNode* ret = new LightNode();
            ret->SetColor(system::ReadVector3(in));
            ret->SetType(LightNodeType::kDirectional);

            return ret;
        }

        static SceneNode* ReadJoint(system::IReadFilePtr& in)
        {
            string animationId = system::ReadString(in);

            JointNode* ret = new JointNode();
            system::ReadBuffer(in, ret->GetAnimation().KeyFrames);

            return ret;
        }

        static SceneNode* ReadMesh(system::IReadFilePtr& in)
        {
            using namespace graphics;
            using namespace system;

            Graphics& graphics = Graphics::GetSingleton();
            MeshNode* ret = new MeshNode();

            ret->SetEffect(graphics.Create<StandardEffect>(ReadString(in)));
            ret->SetMaterial(graphics.Create<Material>(ReadString(in)));
            ret->SetMesh(graphics.Create<Mesh>(ReadString(in)));

            return ret;
        }

        static SceneNode* ReadPhysics(system::IReadFilePtr& in)
        {
            PhysicsNode* ret = new PhysicsNode();
            ret->GetWorldTree()->mTriangleTree.Read(in);
            ret->GetWorldBody()->Update();

            return ret;
        }

        static SceneNode* ReadPointLight(system::IReadFilePtr& in)
        {
            LightNode* ret = new LightNode();
            ret->SetAttenuation(ReadVector3(in));
            ret->SetColor(ReadVector3(in));
            ret->SetType(LightNodeType::kPoint);

            return ret;
        }

        static SceneNode* ReadSpotLight(system::IReadFilePtr& in)
        {
            LightNode* ret = new LightNode();
            ret->SetFalloffAngle(Radian(ReadSingle(in)));
            ret->SetFalloffExponent(ReadSingle(in));
            ret->SetAttenuation(ReadVector3(in));
            ret->SetColor(ReadVector3(in));
            ret->SetType(LightNodeType::kSpot);

            return ret;
        }

        static void ReadSceneNode(system::IReadFilePtr& in, SceneNode* pParent);
        static SceneNode* ReadSceneNode(system::IReadFilePtr& in)
        {
            int childrenCount = ReadInt32(in);
            string id = ReadString(in);
            Matrix4 localTransform = ReadMatrix4(in);
            SceneNodeType::Enum type = (SceneNodeType::Enum)ReadInt32(in);
            
            SceneNode* pNode = null;
            switch (type)
            {
            case SceneNodeType::kAnimatedMesh: pNode = ReadAnimatedMesh(in); break;
            //case SceneNodeType::kCamera: pNode = ReadCamera(in); break;
            case SceneNodeType::kDirectionalLight: pNode = ReadDirectionalLight(in); break;
            case SceneNodeType::kJoint: pNode = ReadJoint(in); break;
            case SceneNodeType::kMesh: pNode = ReadMesh(in); break;
            case SceneNodeType::kNode: pNode = new SceneNode(); break;
            case SceneNodeType::kPhysics: pNode = ReadPhysics(in); break;
            case SceneNodeType::kPointLight: pNode = ReadPointLight(in); break;
            case SceneNodeType::kSpotLight: pNode = ReadSpotLight(in); break;
            default:
                throw exception("should not be here.");
            }

            pNode->SetId(id);
            pNode->SetLocalTransform(localTransform);

            for (int i = 0; i < childrenCount; i++)
            {
                ReadSceneNode(in, pNode);
            }

            return pNode;
        }

        static void ReadSceneNode(system::IReadFilePtr& in, SceneNode* pParent)
        {
            SceneNode* pNode = ReadSceneNode(in);
            pNode->SetParent(pParent);
        }

        AutoPtr<SceneNode> LoadScene(const string& aFilename)
        {
            system::IReadFilePtr pFile = system::Files::GetSingleton().Open(aFilename.c_str());

            SceneNode* pRet = new SceneNode();
            ReadSceneNode(pFile, pRet);

            return pRet;
        }

    }
}

