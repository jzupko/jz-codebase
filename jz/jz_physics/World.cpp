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
#include <jz_core/CoordinateFrame3D.h>
#include <jz_core/Matrix3.h>
#include <jz_core/Quaternion.h>
#include <jz_core/Segment.h>
#include <jz_physics/World.h>
#include <jz_physics/broadphase/Sap.h>
#include <jz_physics/narrowphase/Body.h>
#include <jz_physics/narrowphase/collision/ICollisionShape.h>
#include <jz_physics/narrowphase/collision/Collide.h>
#include <jz_physics/narrowphase/collision/SphereShape.h>
#include <jz_physics/narrowphase/collision/TriangleTreeShape.h>
#include <algorithm>

#define JZ_USE_CONTINUOUS_COLLISION 0
#define JZ_ENABLE_FRICTION 1

namespace jz
{
    namespace physics
    {

        const Vector3 World3D::kDefaultGravity = Vector3(0, -9.8f, 0);
        const float World3D::kTimeStep = (float)(1.0 / 60.0);

        World3D::World3D()
            : 
#           if JZ_PROFILING
                AverageCollisionPairs(0u),
                mAverageCollisionPairs(0u),
#           endif
            mGravity(kDefaultGravity),
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
            for (Bodies::iterator I = mBodies.begin(); I != mBodies.end(); I++)
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
        
        /// <summary>
        /// Predicts the orientation part of a coordinate frame b from a coordinate frame a and an angular velocity.
        /// </summary>
        /// <param name="a">Coordinate frame a.</param>
        /// <param name="aAngular">The angular velocity.</param>
        /// <param name="aTimeStep">The time step.</param>
        /// <param name="arB">The predicted coordinate frame.</param>
        /// <remarks>
        /// From "Exponential Map": Grassia, F. 1998. "Practical Parameterization of Rotations Using the Exponential Map"
        ///     The Journal of Graphics Toosl, 3(3).
        /// </remarks>
        __inline void _IntegrateAngular(const Vector3& aAngular, float aTimeStep, CoordinateFrame3D& arOut)
        {
            static const Radian kMinimumAngle = Radian(1e-7f);
            static const Radian kMaximumAngle = Radian::kPi;
            static const float kTaylorFactor = (float)(1.0 / 48.0);

            Radian angle = Radian(aAngular.Length());
            if ((angle * aTimeStep) > kMaximumAngle) { angle = (kMaximumAngle / aTimeStep); }

            Vector3 axis = (angle < kMinimumAngle)
                ? (aAngular * ((0.5f * aTimeStep) - ((aTimeStep * aTimeStep * aTimeStep) * kTaylorFactor * (angle * angle).GetValue())))
                : (aAngular * (Sin(0.5f * angle * aTimeStep) / angle.GetValue()));

            Quaternion q = Quaternion(axis, Cos(0.5f * angle * aTimeStep));
            Quaternion q0;
            FromMatrix(arOut.Orientation, q0);
            // Quaternion concatenation is right-to-left.
            q0 = Quaternion::Normalize(q * q0);
            ToMatrix(q0, arOut.Orientation);
        }

        void World3D::Tick(float aTimeStep)
        {
            mTimePool += aTimeStep;

            while (mTimePool > kTimeStep)
            {
#           if JZ_PROFILING
                mAverageCollisionPairs = 0u;
#           endif

                mTimePool -= kTimeStep;

                const size_t kSize = mBodies.size();
                for (size_t i = 0u; i < kSize; i++)
                {
                    Body3D* p = mBodies[i];

                    #pragma region Integrate
                    if (p->IsDynamic())
                    {
                        if (p->IsAffectedByGravity())
                        {
                            p->mLinearVelocity += (mGravity * kTimeStep);
                        }

                        p->mPrevFrame = p->mFrame;

                        bool bUpdate = false;
                        if (p->mLinearVelocity.LengthSquared() > (Constants<float>::kZeroTolerance * Constants<float>::kZeroTolerance))
                        {
                            p->mFrame.Translation += (p->mLinearVelocity * kTimeStep);
                            bUpdate = true;
                        }
                        else
                        {
                            p->mLinearVelocity = Vector3::kZero;
                        }

                        if (p->IsAngular() &&
                            p->mAngularVelocity.LengthSquared() > (Constants<float>::kZeroTolerance * Constants<float>::kZeroTolerance))
                        {
                            _IntegrateAngular(p->mAngularVelocity, kTimeStep, p->mFrame);
                            bUpdate = true;
                        }
                        else
                        {
                            p->mAngularVelocity = Vector3::kZero;
                        }

                        if (bUpdate)
                        {
                            _Update(p, p->GetWorldBounding());
                        }
                    }
                    #pragma endregion  
                }

                mpBroadphase->Tick();

                for (size_t i = 0u; i < kSize; i++)
                {
                    Body3D* p = mBodies[i];

                    if (p->IsDynamic() && !p->IsSleeping())
                    {
                        p->OnUpdate(p);
                    }
                }

#           if JZ_PROFILING
                AverageCollisionPairs = (mAverageCollisionPairs + AverageCollisionPairs) / 2u;
#           endif
            }
        }

        void World3D::_StartStopCollisionHandler(void_p a, void_p b)
        {
            Body3D* pa = (Body3D*)a;
            Body3D* pb = (Body3D*)b;

            pa->SetSleeping(false);
            pb->SetSleeping(false);
        }

        void World3D::_UpdateCollisionHandler(void_p a, void_p b)
        {
            Body3D* pa = (Body3D*)a;
            Body3D* pb = (Body3D*)b;

            if ((pa->IsDynamic() || pb->IsDynamic()))
            {
                if (pa->GetCollisionShape()->bConvex() && pb->GetCollisionShape()->bConvex())
                {
                    WorldContactPoint3D cp;
#                   if JZ_PROFILING
                       mAverageCollisionPairs++;
#                   endif

#                   if JZ_USE_CONTINUOUS_COLLISION
                       float t = 1.0f;
                       if (Collide::ContinuousCollide(
                           pa->GetCollisionShape(), pa->mPrevFrame, pa->mFrame,
                           pb->GetCollisionShape(), pb->mPrevFrame, pb->mFrame, cp, t))
                       {
                           pa->mFrame = CoordinateFrame3D::Lerp(pa->mPrevFrame, pa->mFrame, t);
                           pb->mFrame = CoordinateFrame3D::Lerp(pb->mPrevFrame, pb->mFrame, t);
#                   else
                        if (Collide::Collide(pa->GetCollisionShape(), pa->mFrame, 
                                             pb->GetCollisionShape(), pb->mFrame, cp))
                        {
#                   endif

                        _ResolveConvexCollision(pa, pb, cp);
                    }
                }
                else
                {
                    _ResolveConcaveCollision(pa, pb);
                }
            }
        }

        __inline void NormalizeIt(Vector3& v)
        {
            float max = v.Max();
            if (max > Constants<float>::kZeroTolerance)
            {
                v /= max;
            }
        }

        void World3D::_ResolveConvexCollision(Body3D* pa, Body3D* pb, const WorldContactPoint3D& cp)
        {
            float invM = (pa->mInverseMass + pb->mInverseMass);
            if (invM > Constants<float>::kZeroTolerance)
            {
                Vector3 d = Vector3::Dot((cp.WorldPointB - cp.WorldPointA), cp.WorldNormal) * cp.WorldNormal;
                d /= invM;

                pa->mFrame.Translation += (d * pa->mInverseMass);
                pb->mFrame.Translation -= (d * pb->mInverseMass);
                
                Vector3 df = (pb->mLinearVelocity - pa->mLinearVelocity);
                Vector3 rv = Vector3::Dot(df, cp.WorldNormal) * cp.WorldNormal;
                Vector3 rvm = (rv / invM);

                pa->mLinearVelocity += (rvm * pa->mInverseMass);
                pb->mLinearVelocity -= (rvm * pb->mInverseMass);

#               if JZ_ENABLE_FRICTION
                {
                    float friction = (pa->mFriction * pb->mFriction);

                    Vector3 tv = (df - rv);
                    Vector3 tvm = (tv / invM);

                    pa->mLinearVelocity += (friction * (tvm * pa->mInverseMass));
                    pb->mLinearVelocity -= (friction * (tvm * pb->mInverseMass));
                }
#               endif
            }
        }

        void World3D::_ResolveConcaveCollision(Body3D* apa, Body3D* apb)
        {
            Body3D* pa = null;
            Body3D* pb = null;

            ICollisionShape3D* cpa = null;
            TriangleTreeShape* cpb = null;

            if (apa->GetCollisionShape()->bConvex() &&
                apb->GetCollisionShape()->GetType() == ICollisionShape3D::kTriangleTree)
            {
                pa = apa;
                pb = apb;

                cpa = apa->GetCollisionShape();
                cpb = (TriangleTreeShape*)apb->GetCollisionShape();
            }
            else if (apa->GetCollisionShape()->GetType() == ICollisionShape3D::kTriangleTree &&
                     apb->GetCollisionShape()->bConvex())
            {
                pa = apb;
                pb = apa;

                cpa = apb->GetCollisionShape();
                cpb = (TriangleTreeShape*)apa->GetCollisionShape();
            }

            if (cpa && cpb)
            {
                const vector<system::TriangleTree::Node>& nodes = cpb->mTriangleTree.GetNodes();
                const size_t size = nodes.size();

                CoordinateFrame3D aInBcf = pa->mFrame * CoordinateFrame3D::Invert(pb->mFrame);
                BoundingBox aabb = pa->GetWorldBounding(aInBcf);

                for (size_t i = 0; i < size; )
                {
                    Axis::Id axis = nodes[i].GetAxis();
                    float pos = nodes[i].GetSplitPos();

                    if ((nodes[i].IsFront() && (pos <= aabb.Max[axis])) ||
                        (nodes[i].IsBack() && (pos >= aabb.Min[axis])))
                    {
                        if (nodes[i].IsLeaf())
                        {
                            Triangle3D triangle = cpb->mTriangleTree.GetTriangle(nodes[i].TriangleIndex);

                            if (aabb.Intersects(triangle.GetAABB()))
                            {
                                WorldContactPoint3D cp;
#                               if JZ_PROFILING
                                    mAverageCollisionPairs++;
#                               endif

#                           if JZ_USE_CONTINUOUS_COLLISION
                               float t = 1.0f;
                               if (Collide::ContinuousCollide(
                                   cpa, pa->mPrevFrame, pa->mFrame,
                                   &triangle, pb->mPrevFrame, pb->mFrame, cp, t))
                               {
                                   pa->mFrame = CoordinateFrame3D::Lerp(pa->mPrevFrame, pa->mFrame, t);
                                   pb->mFrame = CoordinateFrame3D::Lerp(pb->mPrevFrame, pb->mFrame, t);
#                           else
                                if (Collide::Collide(cpa, pa->mFrame, 
                                                     &triangle, pb->mFrame, cp))
                                {
#                           endif
                                    _ResolveConvexCollision(pa, pb, cp);

                                    aInBcf = pa->mFrame * CoordinateFrame3D::Invert(pb->mFrame);
                                    aabb = pa->GetWorldBounding(aInBcf);
                                }
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

        void World3D::_Add(Body3D* apBody, u32 aType, u32 aCollidesWith)
        {
            apBody->mHandle = mpBroadphase->Add(apBody, aType, aCollidesWith, apBody->GetWorldBounding());
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

    }
}

