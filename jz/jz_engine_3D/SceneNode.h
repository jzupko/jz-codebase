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
#ifndef _JZ_ENGINE_3D_SCENE_NODE_H_
#define _JZ_ENGINE_3D_SCENE_NODE_H_

#include <jz_core/BoundingSphere.h>
#include <jz_core/Event.h>
#include <jz_core/Matrix4.h>
#include <jz_core/Quaternion.h>
#include <jz_core/Tree.h>
#include <jz_core/Vector3.h>
#include <functional>
#include <map>
#include <string>

namespace jz
{
    namespace engine_3D
    {

        namespace SceneNodeFlags
        {
            enum Enum
            {
                kLocalDirty = (1 << 0),
                kWorldDirty = (1 << 1),
                kExcludeFromBounding = (1 << 2),
                kIgnoreParent = (1 << 3)
            };
        }

        static const char* kSceneNodeDefaultCloneIdPostfix = "_clone";

        class SceneNode : public TreeNode<SceneNode>
        {
        public:
            typedef tr1::function<void(SceneNode*)> RetrieveAction;

            SceneNode();
            SceneNode(const string& aId);
            virtual ~SceneNode();

            bool bDirty() const { return mbDirty; }
            bool bValidBounding() const { return mbValidBounding; }
            const Matrix4& GetWit() const { return mWit; }
            const BoundingSphere& GetWorldBounding() const { return mWorldBounding; }
            const string& GetId() const { return mId; }
            void SetId(const string& v);

            Quaternion GetLocalOrientation() const
            {
                Quaternion ret;
                ToQuaternion(mLocal, ret);
                return ret;
            }

            void SetLocalOrientation(const Quaternion& v)
            {
                ToTransform(v, mLocal);
                mFlags |= SceneNodeFlags::kLocalDirty;
            }

            Vector3 GetLocalTranslation() const { return mLocal.GetTranslation(); }
            void SetLocalTranslation(const Vector3& v)
            {
                mLocal.SetTranslation(v);
                mFlags |= SceneNodeFlags::kLocalDirty;
            }

            const Matrix4& GetLocalTransform() const { return mLocal; }
            void SetLocalTransform(const Matrix4& v)
            {
                mLocal = v;
                mFlags |= SceneNodeFlags::kLocalDirty;
            }

            Quaternion GetWorldOrientation() const
            {
                Quaternion ret;
                ToQuaternion(mWorld, ret);
                return ret;
            }

            void SetWorldOrientation(const Quaternion& v)
            {
                ToTransform(v, mWorld);
                mFlags |= SceneNodeFlags::kWorldDirty;
            }

            Vector3 GetWorldTranslation() const { return mWorld.GetTranslation(); }
            void SetWorldTranslation(const Vector3& v)
            {
                mWorld.SetTranslation(v);
                mFlags |= SceneNodeFlags::kWorldDirty;
            }

            const Matrix4& GetWitTransform() const { return mWit; }

            const Matrix4& GetWorldTransform() const { return mWorld; }
            void SetWorldTransform(const Matrix4& v)
            {
                mWorld = v;
                mFlags |= SceneNodeFlags::kWorldDirty;
            }

            typedef Event<void(SceneNode*)> Callback;
            Callback OnUpdateBegin;
            Callback OnUpdateEnd;

            bool Update(const Matrix4& aParentWorld = Matrix4::kIdentity, bool abParentChanged = false)
            {
                OnUpdateBegin(this);
                _PreUpdate(aParentWorld, abParentChanged);

                bool bReturn = _Update(aParentWorld, abParentChanged);
                if (bReturn) { _UpdateBounding(); }

                _PostUpdate(bReturn);
                if (bReturn) { OnUpdateEnd(this); }

                return bReturn;
            }

            template <typename T>
            AutoPtr<T> Clone(SceneNode* apParent)
            {
                return dynamic_cast<T*>(Clone(apParent, kSceneNodeDefaultCloneIdPostfix));
            }

            template <>
            AutoPtr<SceneNode> Clone<SceneNode>(SceneNode* apParent)
            {
                return Clone(apParent, kSceneNodeDefaultCloneIdPostfix);
            }
            
            AutoPtr<SceneNode> Clone(SceneNode* apParent, const string& aCloneIdPostfix);

            template <typename T>
            static AutoPtr<T> Get(const string& aId)
            {
                Container::iterator I = msNodes.find(aId);
                if (I != msNodes.end()) { return dynamic_cast<T*>(I->second); }
                else { return null; }
            }

            static void Get(const string& aId, RetrieveAction aAction);

            template <typename U>
            void Apply(tr1::function<void(U*)> aAction)
            {
                {
                    U* p = dynamic_cast<U*>(this);
                    if (p)
                    {
                        aAction(p);
                    }
                }

                for (iterator I = begin(); I != end(); I++)
                {
                    I->Apply(aAction);
                }
            }

        protected:
            virtual void _PopulateClone(SceneNode* apNode);
            virtual void _PreUpdate(const Matrix4& aParentWorld, bool abParentChanged) {}
            virtual void _PostUpdate(bool abChanged) {}
            virtual SceneNode* _SpawnClone(const string& aCloneId);

            unatural mFlags;
            Matrix4 mLocal;
            Matrix4 mWit;
            Matrix4 mWorld;
            bool mbValidBounding;
            BoundingSphere mWorldBounding;

        private:
            SceneNode(const SceneNode&);
            SceneNode& operator=(const SceneNode&);

            bool mbDirty;
            string mId;

            typedef map<string, SceneNode*> Container;
            typedef map<string, RetrieveAction> RetrieveContainer;

            static Container msNodes;
            static RetrieveContainer msToRetrieve;

            void _CloneChildren(SceneNode* aToParent, const string& aCloneIdPostfix);
            void _UpdateWit();

            bool _Update(const Matrix4& aParentWorld, bool abParentChanged);
            void _UpdateBounding();
        };

        typedef AutoPtr<SceneNode> SceneNodePtr;

    }
}

#endif
