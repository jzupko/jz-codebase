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

#include <jz_core/BoundingSphere.h>
#include <jz_core/CoordinateFrame2D.h>
#include <jz_core/CoordinateFrame3D.h>
#include <jz_core/Matrix2.h>
#include <jz_core/Matrix3.h>
#include <jz_core/Segment.h>
#include <jz_physics/World.h>
#include <jz_physics/broadphase/Sap.h>
#include <jz_physics/narrowphase/Body.h>
#include <jz_physics/narrowphase/collision/ICollisionShape.h>
#include <jz_physics/narrowphase/collision/Collide.h>
#include <jz_physics/narrowphase/collision/SphereShape.h>
#include <jz_physics/narrowphase/collision/TriangleTreeShape.h>
#include <algorithm>

#define JZ_ENABLE_FRICTION 1

namespace jz
{
    namespace physics
    {

        #pragma region World2D
        const Vector2 World2D::kDefaultGravity = Vector2(0, -9.8f);
        const float World2D::kTimeStep = (float)(1.0 / 60.0);

        World2D::World2D()
            : mGravity(kDefaultGravity),
            mTimePool(0.0f),
            mUnitMeter(1.0f)
        {
            Sap2D* p = new Sap2D();
            mpBroadphase.Reset(p);

            p->SetStartCollisionHandler(Sap2D::CollisionHandler::Bind<World2D, &World2D::_StartStopCollisionHandler>(this));
            p->SetStopCollisionHandler(Sap2D::CollisionHandler::Bind<World2D, &World2D::_StartStopCollisionHandler>(this));
            p->SetUpdateCollisionHandler(Sap2D::CollisionHandler::Bind<World2D, &World2D::_UpdateCollisionHandler>(this));
        }

        World2D::~World2D()
        {
            for (vector<Body2D*>::iterator I = mBodies.begin(); I != mBodies.end(); I++)
            {
                (*I)->mHandle = Constants<u16>::kMax;
                (*I)->mpWorld = null;
            }
        }

        Body2D* World2D::Create(ICollisionShape2D* apShape, u32 aType, u32 aCollidesWith)
        {
            Body2D* ret = new Body2D(this, apShape, aType, aCollidesWith);
            _Add(ret, aType, aCollidesWith);

            return ret;
        }

        void World2D::SetUnitMeter(float v)
        {
            v = Clamp(v, Constants<float>::kLooseTolerance, 1.0f);

            float c = (v / mUnitMeter);
            mGravity *= c;

            mUnitMeter = c;
            mpBroadphase->SetUnitMeter(v);
        }

        void World2D::Tick(float aTimeStep)
        {
            mTimePool += aTimeStep;

            while (mTimePool > kTimeStep)
            {
                mTimePool -= kTimeStep;

                const size_t kSize = mBodies.size();
                for (size_t i = 0u; i < kSize; i++)
                {
                    Body2D* p = mBodies[i];

                    if (p->GetType() == Body2D::kDynamic)
                    {
                        p->mVelocity += (mGravity * kTimeStep);
                    }

                    if (p->mVelocity.LengthSquared() > Constants<float>::kZeroTolerance ||
                        !Vector2::AboutEqual(p->mPrevTranslation, p->mTranslation))
                    {
                        p->mPrevTranslation = p->mTranslation;
                        p->mTranslation = (p->mTranslation + (p->mVelocity * kTimeStep));
                        p->Update();                    
                    }
                    else
                    {
                        p->mVelocity = Vector2::kZero;
                    }
                }

                mpBroadphase->Tick();
            }
        }

        void World2D::_StartStopCollisionHandler(void_p a, void_p b)
        {}

        void World2D::_UpdateCollisionHelper(Body2D* pa, Body2D* pb, const WorldContactPoint2D& cp)
        {
            float friction = (pa->mFriction * pb->mFriction);

            if (pa->GetType() == Body2D::kDynamic && pb->GetType() == Body2D::kDynamic)
            {
                float invM = (pa->mInverseMass + pb->mInverseMass);
                if (invM > Constants<float>::kZeroTolerance)
                {
                    Vector2 d = Vector2::Dot((cp.WorldPointB - cp.WorldPointA), cp.WorldNormal) * cp.WorldNormal;
                    d /= invM;

                    pa->mTranslation += (d * pa->mInverseMass);
                    pb->mTranslation -= (d * pb->mInverseMass);
                    
                    Vector2 rv = Vector2::Dot((pb->mVelocity - pa->mVelocity), cp.WorldNormal) * cp.WorldNormal;
                    rv /= invM;

                    pa->mVelocity += (rv * pa->mInverseMass);
                    pb->mVelocity -= (rv * pb->mInverseMass);

#                   if JZ_ENABLE_FRICTION
                        Vector2 tv = Vector2::Dot((pb->mVelocity - pa->mVelocity), Vector2(cp.WorldNormal.Y, -cp.WorldNormal.X)) * Vector2(cp.WorldNormal.Y, -cp.WorldNormal.X);
                        tv /= invM;

                        pa->mVelocity += friction * (tv * pa->mInverseMass);
                        pb->mVelocity -= friction * (tv * pb->mInverseMass);
#                   endif
                }
            }
            else 
            {
                Body2D* pDynamic = (pa->GetType() == Body2D::kDynamic) ? pa : pb;
                Body2D* pStatic = (pa->GetType() == Body2D::kDynamic) ? pb : pa;
                Vector2 wn = (pa->GetType() == Body2D::kDynamic) ? cp.WorldNormal : -cp.WorldNormal;

                pDynamic->mTranslation += Vector2::Dot((cp.WorldPointB - cp.WorldPointA), wn) * wn;

                Vector2 rv = Vector2::Dot(-pDynamic->mVelocity, wn) * wn;
                pDynamic->mVelocity += rv;

#               if JZ_ENABLE_FRICTION
                    Vector2 tv = Vector2::Dot(pStatic->mVelocity - pDynamic->mVelocity, Vector2(wn.Y, -wn.X)) * Vector2(wn.Y, -wn.X);
                    pDynamic->mVelocity += (friction * tv);
#               endif
            }
        }

        void World2D::_UpdateCollisionConcaveHandler(Body2D* pa, Body2D* pb)
        {
            // Temp: no concave 2D shapes yet.   
        }

        void World2D::_UpdateCollisionHandler(void_p a, void_p b)
        {
            Body2D* pa = (Body2D*)a;
            Body2D* pb = (Body2D*)b;

            if (pa->GetType() == Body2D::kDynamic || pb->GetType() == Body2D::kDynamic)
            {
                if (pa->GetCollisionShape()->bConvex() && pb->GetCollisionShape()->bConvex())
                {
                    WorldContactPoint2D cp;
                    if (Collide::Collide(pa->GetCollisionShape(), CoordinateFrame2D(Matrix2::kIdentity, pa->mTranslation), 
                                         pb->GetCollisionShape(), CoordinateFrame2D(Matrix2::kIdentity, pb->mTranslation), cp))
                    {
                        _UpdateCollisionHelper(pa, pb, cp);
                    }
                }
                else
                {
                    _UpdateCollisionConcaveHandler(pa, pb);
                }
            }
        }

        void World2D::_Add(Body2D* apBody, u32 aType, u32 aCollidesWith)
        {
            apBody->mHandle = mpBroadphase->Add(apBody, aType, aCollidesWith, apBody->GetLocalBounding());
            mBodies.push_back(apBody);
        }

        void World2D::_Remove(Body2D* apBody)
        {
            mBodies.erase(find(mBodies.begin(), mBodies.end(), apBody));
            mpBroadphase->Remove(apBody->mHandle);
        }

        void World2D::_Update(Body2D* apBody, const BoundingRectangle& aBoundingRectangle)
        {
            mpBroadphase->Update(apBody->mHandle, aBoundingRectangle);
        }
        #pragma endregion

        #pragma region World3D
        const Vector3 World3D::kDefaultGravity = Vector3(0, -9.8f, 0);
        const float World3D::kTimeStep = (float)(1.0 / 60.0);

        World3D::World3D()
            : mGravity(kDefaultGravity),
            mTimePool(0.0f),
            mUnitMeter(1.0f)
        {
            Sap3D* p = new Sap3D();
            mpBroadphase.Reset(p);

            p->SetStartCollisionHandler(Sap3D::CollisionHandler::Bind<World3D, &World3D::_StartStopCollisionHandler>(this));
            p->SetStopCollisionHandler(Sap3D::CollisionHandler::Bind<World3D, &World3D::_StartStopCollisionHandler>(this));
            p->SetUpdateCollisionHandler(Sap3D::CollisionHandler::Bind<World3D, &World3D::_UpdateCollisionHandler>(this));
        }

        World3D::~World3D()
        {
            for (vector<Body3D*>::iterator I = mBodies.begin(); I != mBodies.end(); I++)
            {
                (*I)->mHandle = Constants<u16>::kMax;
                (*I)->mpWorld = null;
            }
        }

        Body3D* World3D::Create(ICollisionShape3D* apShape, u32 aType, u32 aCollidesWith)
        {
            Body3D* ret = new Body3D(this, apShape, aType, aCollidesWith);
            _Add(ret, aType, aCollidesWith);

            return ret;
        }

        void World3D::SetUnitMeter(float v)
        {
            v = Clamp(v, Constants<float>::kLooseTolerance, 1.0f);

            float c = (v / mUnitMeter);
            mGravity *= c;

            mUnitMeter = c;
            mpBroadphase->SetUnitMeter(v);
        }

        void World3D::Tick(float aTimeStep)
        {
            mTimePool += aTimeStep;

            while (mTimePool > kTimeStep)
            {
                mTimePool -= kTimeStep;

                const size_t kSize = mBodies.size();
                for (size_t i = 0u; i < kSize; i++)
                {
                    Body3D* p = mBodies[i];

                    if (p->GetType() == Body3D::kDynamic)
                    {
                        p->mVelocity += (mGravity * kTimeStep);
                    }

                    if (p->mVelocity.LengthSquared() > Constants<float>::kZeroTolerance ||
                        !Vector3::AboutEqual(p->mPrevTranslation, p->mTranslation))
                    {
                        p->mPrevTranslation = p->mTranslation;
                        p->mTranslation = (p->mTranslation + (p->mVelocity * kTimeStep));
                        p->Update();                    
                    }
                    else
                    {
                        p->mVelocity = Vector3::kZero;
                    }
                }

                mpBroadphase->Tick();
            }
        }

        void World3D::_StartStopCollisionHandler(void_p a, void_p b)
        {}

        void World3D::_UpdateCollisionHelper(Body3D* pa, Body3D* pb, const WorldContactPoint3D& cp)
        {
            float friction = (pa->mFriction * pb->mFriction);

            if (pa->GetType() == Body3D::kDynamic && pb->GetType() == Body3D::kDynamic)
            {
                float invM = (pa->mInverseMass + pb->mInverseMass);
                if (invM > Constants<float>::kZeroTolerance)
                {
                    Vector3 d = Vector3::Dot((cp.WorldPointB - cp.WorldPointA), cp.WorldNormal) * cp.WorldNormal;
                    d /= invM;

                    pa->mTranslation += (d * pa->mInverseMass);
                    pb->mTranslation -= (d * pb->mInverseMass);
                    
                    Vector3 df = (pb->mVelocity - pa->mVelocity);
                    Vector3 rv = Vector3::Dot(df, cp.WorldNormal) * cp.WorldNormal;
                    Vector3 rvm = (rv / invM);

                    pa->mVelocity += (rvm * pa->mInverseMass);
                    pb->mVelocity -= (rvm * pb->mInverseMass);

#                   if JZ_ENABLE_FRICTION
                        Vector3 tv = (df - rv);
                        Vector3 tvm = (tv / invM);

                        pa->mVelocity += friction * (tvm * pa->mInverseMass);
                        pb->mVelocity -= friction * (tvm * pb->mInverseMass);
#                   endif
                }
            }
            else 
            {
                Body3D* pDynamic = (pa->GetType() == Body3D::kDynamic) ? pa : pb;
                Body3D* pStatic = (pa->GetType() == Body3D::kDynamic) ? pb : pa;
                Vector3 wn = (pa->GetType() == Body3D::kDynamic) ? cp.WorldNormal : -cp.WorldNormal;

                pDynamic->mTranslation += Vector3::Dot((cp.WorldPointB - cp.WorldPointA), wn) * wn;

                Vector3 rv = Vector3::Dot(-pDynamic->mVelocity, wn) * wn;
                pDynamic->mVelocity += rv;

#               if JZ_ENABLE_FRICTION
                    Vector3 tv = (-pDynamic->mVelocity - rv);
                    pDynamic->mVelocity += (friction * tv);
#               endif
            }
        }

        static bool _Collide(const BoundingSphere& a, const Triangle3D& b, const Vector3& bt, WorldContactPoint3D& cp)
        {
            Vector3 p = Collide::ClosestPointOnTriangleToP(a.Center, b);
            Vector3 v = (p - a.Center);

            float lenSq = (v.LengthSquared());
            bool bReturn = (lenSq <= (a.Radius * a.Radius));

            if (bReturn)
            {
                float l = Sqrt(lenSq);
                float d = (l - a.Radius);
                Vector3 n = (-v / l);

                cp.WorldNormal = n;
                cp.WorldPointB = (p + bt);
                cp.WorldPointA = (cp.WorldPointB + (d * cp.WorldNormal));
            }

            return bReturn;
        }

        // conservative advancement
        static bool _ContinuousCollide(Body3D* pb, SphereShape* pc, const Triangle3D& b, const Vector3& bp, const Vector3& bt, WorldContactPoint3D& cp, float& t)
        {
            t = 0.0f;
            Segment referenceSegment = Segment::Create(bp, bt);
            Segment segment = Segment::Create(pb->GetPrevTranslation() - bp, pb->GetTranslation() - bt);
            float maxDistance = segment.Distance();

            float r2 = (pc->Radius * pc->Radius);
            while (t >= 0.0f && t <= 1.0f)
            {
                Vector3 center = segment.Evaluate(t);
                Vector3 p = Collide::ClosestPointOnTriangleToP(center, b);
                Vector3 v = (p - center);

                float lenSq = (v.LengthSquared());
                if (lenSq <= r2)
                {
                    float l = Sqrt(lenSq);
                    float d = (l - pc->Radius);
                    Vector3 n = (-v / l);

                    cp.WorldNormal = n;
                    cp.WorldPointB = (p + referenceSegment.Evaluate(t));
                    cp.WorldPointA = (cp.WorldPointB + (d * cp.WorldNormal));
                    return true;
                }
                else
                {
                    float distance = Max(Sqrt(lenSq) - pc->Radius, Constants<float>::kLooseTolerance);
                    t += (distance / maxDistance);
                }
            }

            return false;
        }

        // TODO:
        //   - implement arbitrary object collision with triangle trees
        //     - require closest distance between triangle and arbitrary convex object defined
        //       as support mapping
        //     - xenocollide or gjk?
        //   - implement contiuous collision detection between convex objects.
        //     - this should be a simple extension once arbitrary closest point between convex
        //       and triangle has been implemented.
        void World3D::_UpdateCollisionConcaveHandler(Body3D* apa, Body3D* apb)
        {
            Body3D* pa = null;
            Body3D* pb = null;

            SphereShape* cpa = null;
            TriangleTreeShape* cpb = null;

            if (apa->GetCollisionShape()->GetType() == ICollisionShape3D::kSphere &&
                apb->GetCollisionShape()->GetType() == ICollisionShape3D::kTriangleTree)
            {
                pa = apa;
                pb = apb;

                cpa = (SphereShape*)apa->GetCollisionShape();
                cpb = (TriangleTreeShape*)apb->GetCollisionShape();
            }
            else if (apa->GetCollisionShape()->GetType() == ICollisionShape3D::kTriangleTree &&
                apb->GetCollisionShape()->GetType() == ICollisionShape3D::kSphere)
            {
                pa = apb;
                pb = apa;

                cpa = (SphereShape*)apb->GetCollisionShape();
                cpb = (TriangleTreeShape*)apa->GetCollisionShape();
            }

            if (cpa && cpb)
            {
                // continuous collision
                if (Vector3::DistanceSquared(pa->mPrevTranslation, pa->mTranslation) >  0.25f * (cpa->Radius * cpa->Radius))
                {
                    Vector3 center = (pa->mPrevTranslation + pa->mTranslation) * 0.5f;
                    Vector3 halfExtents = (Vector3::Abs(pa->mTranslation - pa->mPrevTranslation) * 0.5f) + Vector3(cpa->Radius);

                    const vector<TriangleTree::Node>& nodes = cpb->mTriangleTree.GetNodes();
                    size_t size = nodes.size();
                    for (size_t i = 0; i < size; )
                    {
                        Axis::Id axis = nodes[i].GetAxis();
                        float pos = nodes[i].GetSplitPos();

                        if ((nodes[i].IsFront() && (pos <= (center[axis] + halfExtents[axis]))) ||
                            (nodes[i].IsBack() && (pos >= (center[axis] - halfExtents[axis]))))
                        {
                            if (nodes[i].IsLeaf())
                            {
                                Triangle3D triangle = cpb->mTriangleTree.GetTriangle(nodes[i].TriangleIndex);

                                WorldContactPoint3D cp;
                                float t;
                                if (_ContinuousCollide(pa, cpa, triangle, pb->mPrevTranslation, pb->mTranslation, cp, t))
                                {
                                    Vector3 np = (pa->mPrevTranslation + ((pa->mTranslation - pa->mPrevTranslation) * t));
                                    pa->mTranslation += Vector3::Dot((np - pa->mTranslation), cp.WorldNormal) * cp.WorldNormal;
                                    _UpdateCollisionHelper(pa, pb, cp);
                                }
                            }

                            i++;
                        }
                        else
                        {
                            if (nodes[i].IsLeaf()) { i++; }
                            else
                            {
                                JZ_ASSERT(nodes[i].Sibling > i);
                                i = nodes[i].Sibling;
                            }
                        }
                    }            
                }
                else
                {
                    BoundingSphere bs = BoundingSphere((pa->mTranslation - pb->mTranslation), cpa->Radius);
                    
                    const vector<TriangleTree::Node>& nodes = cpb->mTriangleTree.GetNodes();
                    size_t size = nodes.size();
                    for (size_t i = 0; i < size; )
                    {
                        Axis::Id axis = nodes[i].GetAxis();
                        float pos = nodes[i].GetSplitPos();

                        if ((nodes[i].IsFront() && (pos <= (bs.Center[axis] + bs.Radius))) ||
                            (nodes[i].IsBack() && (pos >= (bs.Center[axis] - bs.Radius))))
                        {
                            if (nodes[i].IsLeaf())
                            {
                                Triangle3D t = cpb->mTriangleTree.GetTriangle(nodes[i].TriangleIndex);

                                WorldContactPoint3D cp;
                                if (_Collide(bs, t, pb->mTranslation, cp))
                                {
                                    _UpdateCollisionHelper(pa, pb, cp);
                                    bs = BoundingSphere((pa->mTranslation - pb->mTranslation), cpa->Radius);
                                }
                            }

                            i++;
                        }
                        else
                        {
                            if (nodes[i].IsLeaf()) { i++; }
                            else
                            {
                                JZ_ASSERT(nodes[i].Sibling > i);
                                i = nodes[i].Sibling;
                            }
                        }
                    }
                }
            }
        }

        void World3D::_UpdateCollisionHandler(void_p a, void_p b)
        {
            Body3D* pa = (Body3D*)a;
            Body3D* pb = (Body3D*)b;

            if (pa->GetType() == Body3D::kDynamic || pb->GetType() == Body3D::kDynamic)
            {
                if (pa->GetCollisionShape()->bConvex() && pb->GetCollisionShape()->bConvex())
                {
                    WorldContactPoint3D cp;
                    if (Collide::Collide(pa->GetCollisionShape(), CoordinateFrame3D(Matrix3::kIdentity, pa->mTranslation), 
                                         pb->GetCollisionShape(), CoordinateFrame3D(Matrix3::kIdentity, pb->mTranslation), cp))
                    {
                        _UpdateCollisionHelper(pa, pb, cp);
                    }
                }
                else
                {
                    _UpdateCollisionConcaveHandler(pa, pb);
                }
            }
        }

        void World3D::_Add(Body3D* apBody, u32 aType, u32 aCollidesWith)
        {
            apBody->mHandle = mpBroadphase->Add(apBody, aType, aCollidesWith, apBody->GetLocalBounding());
            mBodies.push_back(apBody);
        }

        void World3D::_Remove(Body3D* apBody)
        {
            mBodies.erase(find(mBodies.begin(), mBodies.end(), apBody));
            mpBroadphase->Remove(apBody->mHandle);
        }

        void World3D::_Update(Body3D* apBody, const BoundingBox& aBoundingBox)
        {
            mpBroadphase->Update(apBody->mHandle, aBoundingBox);
        }
        #pragma endregion

    }
}

