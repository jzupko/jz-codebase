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

#include <jz_engine_3D/SceneNode.h>

namespace jz
{
    namespace engine_3D
    {

        SceneNode::Container SceneNode::msNodes;
        SceneNode::RetrieveContainer SceneNode::msToRetrieve;

        SceneNode::SceneNode()
            : mBaseId(string()),
            mId(string()),
            mbDirty(false),
            mFlags(SceneNodeFlags::kLocalDirty),
            mLocal(Matrix4::kIdentity),
            mWit(Matrix4::kIdentity),
            mWorld(Matrix4::kIdentity),
            mbValidBounding(false),
            mWorldAABB(BoundingBox::kZero),
            mWorldBounding(BoundingSphere::kZero)
        {}

        SceneNode::SceneNode(const string& aBaseId, const string& aId)
            : mBaseId(aBaseId),
            mId(aId),
            mbDirty(false),
            mFlags(SceneNodeFlags::kLocalDirty),
            mLocal(Matrix4::kIdentity),
            mWit(Matrix4::kIdentity),
            mWorld(Matrix4::kIdentity),
            mbValidBounding(false),
            mWorldAABB(BoundingBox::kZero),
            mWorldBounding(BoundingSphere::kZero)
        {
            string combinedId = (aBaseId + aId);
            JZ_ASSERT(msNodes.find(combinedId) == msNodes.end());

            msNodes.insert(make_pair(combinedId, this));

            RetrieveContainer::iterator I = msToRetrieve.find(combinedId);

            if (I != msToRetrieve.end())
            {
                I->second(this);
                msToRetrieve.erase(I);
            }
        }

        SceneNode::~SceneNode()
        {
            if (!mBaseId.empty() || !mId.empty())
            {
                msNodes.erase(mBaseId + mId);
            }
        }

        void SceneNode::SetParent(weak_pointer p)
        {
            TreeNode<SceneNode>::SetParent(p);

            if (GetParent() && (GetBaseId() == string()))
            {
                SetBaseId(GetParent()->GetBaseId());
            }
        }

        void SceneNode::SetIds(const string& aBaseId, const string& aId)
        {
            JZ_ASSERT(msNodes.find(aBaseId + aId) == msNodes.end());

            if (!(mBaseId + mId).empty()) { msNodes.erase(mBaseId + mId); }
            mBaseId = aBaseId;
            mId = aId;
            if (!(mBaseId + mId).empty()) { msNodes.insert(make_pair(mBaseId + mId, this)); }
        }

        void SceneNode::SetBaseId(const string& v)
        {
            JZ_ASSERT(msNodes.find(v + mId) == msNodes.end());

            if (!(mBaseId + mId).empty()) { msNodes.erase(mBaseId + mId); }
            mBaseId = v;
            if (!(mBaseId + mId).empty()) { msNodes.insert(make_pair(mBaseId + mId, this)); }
        }

        void SceneNode::SetId(const string& v)
        {
            JZ_ASSERT(msNodes.find(mBaseId + v) == msNodes.end());

            if (!(mBaseId + mId).empty()) { msNodes.erase(mBaseId + mId); }
            mId = v;
            if (!(mBaseId + mId).empty()) { msNodes.insert(make_pair(mBaseId + mId, this)); }
        }

        void SceneNode::Get(const string& aBaseId, const string& aId, RetrieveAction aAction)
        {
            Container::iterator I = msNodes.find(aBaseId + aId);
            
            if (I != msNodes.end()) { aAction(I->second); }
            else
            {
                msToRetrieve.insert(make_pair(aBaseId + aId, aAction));
            }
        }

        AutoPtr<SceneNode> SceneNode::Clone(SceneNode* apParent, const string& aCloneIdPostfix)
        {
            SceneNode* clone = _SpawnClone(mBaseId, mId + aCloneIdPostfix);

            if (clone)
            {
                _PopulateClone(clone);

                _CloneChildren(clone, aCloneIdPostfix);
                clone->SetParent(apParent);

                return clone;
            }
            else
            {
                return null;
            }
        }

        void SceneNode::_PopulateClone(SceneNode* apNode)
        {
            apNode->mLocal = mLocal;
        }

        SceneNode* SceneNode::_SpawnClone(const string& aBaseId, const string& aCloneId)
        {
            return new SceneNode(aBaseId, aCloneId);
        }

        void SceneNode::_CloneChildren(SceneNode* aToParent, const string& aCloneIdPostfix)
        {
            for (iterator I = begin(); I != end(); I++)
            {
                I->Clone(aToParent, aCloneIdPostfix);
            }
        }

        void SceneNode::_UpdateWit()
        {
            mWit = Matrix4::CreateNormalTransform(mWorld);
        }

        Matrix4 SceneNode::_GetUpdatedWorldTransform(const Matrix4& aParentWorld, bool abParentChanged) const
        {
            if ((mFlags & SceneNodeFlags::kIgnoreParent) == 0)
            {
                if ((mFlags & SceneNodeFlags::kWorldDirty) != 0)
                {
                    return (mWorld);
                }
                else if (abParentChanged || (mFlags & SceneNodeFlags::kLocalDirty) != 0)
                {
                    return (mLocal * aParentWorld);
                }
            }
            else
            {
                if ((mFlags & SceneNodeFlags::kWorldDirty) != 0)
                {
                    return (mWorld);
                }
                else if ((mFlags & SceneNodeFlags::kLocalDirty) != 0)
                {
                    return (mLocal);
                }
            }

            return (mWorld);
        }

        bool SceneNode::_Update(const Matrix4& aParentWorld, bool abParentChanged)
        {
            bool bChanged = false;

            if ((mFlags & SceneNodeFlags::kIgnoreParent) == 0)
            {
                if ((mFlags & SceneNodeFlags::kWorldDirty) != 0)
                {
                    _UpdateWit();
                    Matrix4 invParent = Matrix4::Invert(aParentWorld);
                    mLocal = (mWorld * invParent);

                    mFlags &= ~SceneNodeFlags::kWorldDirty;
                    mFlags &= ~SceneNodeFlags::kLocalDirty;
                    bChanged = true;
                }
                else if (abParentChanged || (mFlags & SceneNodeFlags::kLocalDirty) != 0)
                {
                    mWorld = (mLocal * aParentWorld);
                    _UpdateWit();

                    mFlags &= ~SceneNodeFlags::kLocalDirty;
                    bChanged = true;
                }
            }
            else
            {
                if ((mFlags & SceneNodeFlags::kWorldDirty) != 0)
                {
                    _UpdateWit();
                    mLocal = mWorld;

                    mFlags &= ~SceneNodeFlags::kWorldDirty;
                    mFlags &= ~SceneNodeFlags::kLocalDirty;
                    bChanged = true;
                }
                else if ((mFlags & SceneNodeFlags::kLocalDirty) != 0)
                {
                    mWorld = mLocal;
                    _UpdateWit();

                    mFlags &= ~SceneNodeFlags::kLocalDirty;
                    bChanged = true;
                }
            }

            bool bBoundingUpdate = bChanged;
            for (iterator I = begin(); I != end(); I++)
            {
                bBoundingUpdate = I->_DoUpdate(mWorld, bChanged) || bBoundingUpdate;
            }

            mbDirty = bBoundingUpdate;

            if (mbDirty)
            {
                mWorldAABB = BoundingBox::kZero;
                mWorldBounding = BoundingSphere::kZero;
                mbValidBounding = false;
            }

            return mbDirty;
        }

        void SceneNode::_UpdateBounding()
        {
            bool bValid = false;
            BoundingBox box;
            BoundingSphere sphere;
            iterator I = begin();

            for (; I != end(); I++)
            {
                if (I->mbValidBounding && (I->mFlags & SceneNodeFlags::kExcludeFromBounding) == 0)
                {
                    bValid = true;
                    box = I->mWorldAABB;
                    sphere = I->mWorldBounding;
                    
                    I++;
                    break;
                }
            }

            for (; I != end(); I++)
            {
                if (I->mbValidBounding && (I->mFlags & SceneNodeFlags::kExcludeFromBounding) == 0)
                {
                    box = BoundingBox::Merge(box, I->mWorldAABB);
                    sphere = BoundingSphere::Merge(sphere, I->mWorldBounding);
                }
            }

            if (bValid)
            {
                if (mbValidBounding)
                {
                    mWorldAABB = BoundingBox::Merge(box, mWorldAABB);
                    mWorldBounding = BoundingSphere::Merge(sphere, mWorldBounding);
                }
                else
                {
                    mWorldAABB = box;
                    mWorldBounding = sphere;
                    mbValidBounding = true;
                }
            }
        }

    }
}

