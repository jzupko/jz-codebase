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

#include <jz_core/BoundingBox.h>
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
            JZ_ALIGNED_NEW

            bool IsIgnoringParent() const { return ((mFlags & SceneNodeFlags::kIgnoreParent) != 0); }
            bool IsLocalDirty() const { return ((mFlags & SceneNodeFlags::kLocalDirty) != 0); }
            bool IsWorldDirty() const { return ((mFlags & SceneNodeFlags::kWorldDirty) != 0); }

            typedef tr1::function<void(SceneNode*)> RetrieveAction;

            virtual void SetParent(weak_pointer p) override;

            SceneNode();
            SceneNode(const string& aBaseId, const string& aId);
            virtual ~SceneNode();

            bool bDirty() const { return mbDirty; }
            bool bValidBounding() const { return mbValidBounding; }
            const Matrix4& GetWit() const { return mWit; }

            virtual const BoundingBox& GetBoundingBox() const { return mWorldAABB; }
            virtual const BoundingSphere& GetBoundingSphere() const { return mWorldBounding; }

            virtual void SetIds(const string& aBaseId, const string& aId);
            const string& GetBaseId() const { return mBaseId; }
            virtual void SetBaseId(const string& v);
            const string& GetId() const { return mId; }
            virtual void SetId(const string& v);

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

            void Update(const Matrix4& aParentWorld = Matrix4::kIdentity, bool abParentChanged = false)
            {
                _DoPreUpdateA(aParentWorld, abParentChanged);
                _DoPreUpdateB(aParentWorld, abParentChanged);
                _DoUpdate(aParentWorld, abParentChanged);
                _DoPostUpdate();
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
            static AutoPtr<T> Get(const string& aBaseId, const string& aId)
            {
                Container::iterator I = msNodes.find(aBaseId + aId);
                if (I != msNodes.end()) { return dynamic_cast<T*>(I->second); }
                else { return null; }
            }

            static void Get(const string& aBaseId, const string& aId, RetrieveAction aAction);

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
            Matrix4 _GetUpdatedWorldTransform(const Matrix4& aParentWorld, bool abParentChanged) const;

            virtual void _PopulateClone(SceneNode* apNode);
            virtual void _PreUpdateA(const Matrix4& aParentWorld, bool abParentChanged) {}
            virtual void _PreUpdateB(const Matrix4& aParentWorld, bool abParentChanged) {}
            virtual void _PostUpdate(bool abChanged) {}
            virtual SceneNode* _SpawnClone(const string& aBaseId, const string& aCloneId);

            unatural mFlags;
            Matrix4 mLocal;
            Matrix4 mWit;
            Matrix4 mWorld;
            bool mbValidBounding;
            BoundingBox mWorldAABB;
            BoundingSphere mWorldBounding;

        private:
            SceneNode(const SceneNode&);
            SceneNode& operator=(const SceneNode&);

            bool mbDirty;
            string mBaseId;
            string mId;

            typedef map<string, SceneNode*> Container;
            typedef map<string, RetrieveAction> RetrieveContainer;

            static Container msNodes;
            static RetrieveContainer msToRetrieve;

            void _CloneChildren(SceneNode* aToParent, const string& aCloneIdPostfix);
            void _UpdateWit();

            bool _Update(const Matrix4& aParentWorld, bool abParentChanged);
            void _UpdateBounding();

            void _DoPreUpdateA(const Matrix4& aParentWorld, bool abParentChanged)
            {
                OnUpdateBegin(this);
                _PreUpdateA(aParentWorld, abParentChanged);

                for (iterator I = begin(); I != end(); I++)
                {
                    I->_DoPreUpdateA(mWorld, abParentChanged);
                }
            }

            void _DoPreUpdateB(const Matrix4& aParentWorld, bool abParentChanged)
            {
                _PreUpdateB(aParentWorld, abParentChanged);

                for (iterator I = begin(); I != end(); I++)
                {
                    I->_DoPreUpdateB(mWorld, abParentChanged);
                }
            }

            bool _DoUpdate(const Matrix4& aParentWorld, bool abParentChanged)
            {
                bool bReturn = _Update(aParentWorld, abParentChanged);

                return bReturn;
            }

            void _DoPostUpdate()
            {
                _PostUpdate(mbDirty);

                for (iterator I = begin(); I != end(); I++)
                {
                    I->_DoPostUpdate();
                }

                if (mbDirty)
                {
                    _UpdateBounding();
                    OnUpdateEnd(this);
                }
            }
        };

        typedef AutoPtr<SceneNode> SceneNodePtr;

    }
}

#endif
