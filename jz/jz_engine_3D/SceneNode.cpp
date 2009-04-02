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
            : mId(string()),
            mbDirty(false),
            mFlags(SceneNodeFlags::kLocalDirty),
            mLocal(Matrix4::kIdentity),
            mWit(Matrix4::kIdentity),
            mWorld(Matrix4::kIdentity),
            mbValidBounding(false),
            mWorldBounding(BoundingSphere::kZero)
        {}

        SceneNode::SceneNode(const string& aId)
            : mId(aId),
            mbDirty(false),
            mFlags(SceneNodeFlags::kLocalDirty),
            mLocal(Matrix4::kIdentity),
            mWit(Matrix4::kIdentity),
            mWorld(Matrix4::kIdentity),
            mbValidBounding(false),
            mWorldBounding(BoundingSphere::kZero)
        {
            JZ_ASSERT(msNodes.find(aId) == msNodes.end());

            msNodes.insert(make_pair(aId, this));

            RetrieveContainer::iterator I = msToRetrieve.find(aId);

            if (I != msToRetrieve.end())
            {
                I->second(this);
                msToRetrieve.erase(I);
            }
        }

        SceneNode::~SceneNode()
        {
            if (!mId.empty())
            {
                msNodes.erase(mId);
            }
        }

        void SceneNode::SetId(const string& v)
        {
            JZ_ASSERT(msNodes.find(v) == msNodes.end());

            if (!mId.empty()) { msNodes.erase(mId); }
            mId = v;
            if (!mId.empty()) { msNodes.insert(make_pair(mId, this)); }
        }

        void SceneNode::Get(const string& aId, RetrieveAction aAction)
        {
            Container::iterator I = msNodes.find(aId);
            
            if (I != msNodes.end()) { aAction(I->second); }
            else
            {
                msToRetrieve.insert(make_pair(aId, aAction));
            }
        }

        AutoPtr<SceneNode> SceneNode::Clone(SceneNode* apParent, const string& aCloneIdPostfix)
        {
            SceneNode* clone = _SpawnClone(mId + aCloneIdPostfix);

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

        SceneNode* SceneNode::_SpawnClone(const string& aCloneId)
        {
            return new SceneNode(aCloneId);
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
                bBoundingUpdate = I->Update(mWorld, bChanged) || bBoundingUpdate;
            }

            mbDirty = bBoundingUpdate;

            return bBoundingUpdate;
        }

        void SceneNode::_UpdateBounding()
        {
            mbValidBounding = false;
            mWorldBounding = BoundingSphere::kZero;
            iterator I = begin();

            for (; I != end(); I++)
            {
                if (I->mbValidBounding && (I->mFlags & SceneNodeFlags::kExcludeFromBounding) == 0)
                {
                    mWorldBounding = I->mWorldBounding;
                    mbValidBounding = true;
                    I++;
                    break;
                }
            }

            for (; I != end(); I++)
            {
                if (I->mbValidBounding && (I->mFlags & SceneNodeFlags::kExcludeFromBounding) == 0)
                {
                    mWorldBounding = BoundingSphere::Merge(mWorldBounding, I->mWorldBounding);
                }
            }
        }

    }
}

