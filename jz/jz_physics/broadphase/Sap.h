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
#ifndef _JZ_PHYSICS_SAP_H_
#define _JZ_PHYSICS_SAP_H_

#include <jz_core/AddressBuffer.h>
#include <jz_core/Delegate.h>
#include <jz_core/Vector2.h>
#include <jz_core/Vector3.h>
#include <jz_physics/broadphase/IBroadphase.h>
#include <jz_physics/broadphase/PairTable.h>

namespace jz
{
    namespace physics
    {

        typedef vector<bool> BitArray;

        struct EndPoint
        {
            static const uint kMinMaxFlag;
            static const int kOwnerShift;
            static const uint kOwnerMask;

            static const u16 kSentinelId;
            static const uint kSentinelMin;
            static const uint kSentinelMax;

            EndPoint()
                : mOwnerAndFlags(0u), Value(0u)
            {}

            EndPoint(bool abMax, ushort aOwnerHandle, uint aValue)
            {
                mOwnerAndFlags = 0u;
                Value = aValue;

                SetMax(abMax);
                SetOwnerId(aOwnerHandle);
            }

            EndPoint(const EndPoint& b)
                : mOwnerAndFlags(b.mOwnerAndFlags), Value(b.Value)
            {}

            EndPoint& operator=(const EndPoint& b)
            {
                mOwnerAndFlags = b.mOwnerAndFlags;
                Value = b.Value;

                return *this;
            }
             
            bool operator==(const EndPoint& b) const { return (Value == b.Value); }
            bool operator!=(const EndPoint& b) const{ return (Value != b.Value); }
            bool operator<(const EndPoint& b) const { return (Value < b.Value); }
            bool operator>(const EndPoint& b) const { return (Value > b.Value); }

            bool IsMax() const { return ((mOwnerAndFlags & kMinMaxFlag) != 0); }
            void SetMax(bool b)
            {
                if (b) { mOwnerAndFlags |= kMinMaxFlag; }
                else { mOwnerAndFlags &= ~kMinMaxFlag; }
            }

            bool IsSentinel() const { return (OwnerId() == kSentinelId); }

            u16 OwnerId() const { return (ushort)((mOwnerAndFlags & kOwnerMask) >> kOwnerShift); }
            void SetOwnerId(u16 value)
            {
                mOwnerAndFlags |= (uint)((((uint)value) << kOwnerShift) & kOwnerMask);
            }

            uint Value;

        private:
            // bit 0:     flag indicating min/max
            // bit 1:     flag indicating no sentinel/sentinel.
            // bits 2-31: index of AABB
            uint mOwnerAndFlags;
        };

        class Sap2D : public IBroadphase2D
        {
        public:
            typedef Delegate<void(void_p, void_p)> CollisionHandler;

            static const Vector2 kCollisionBoundary;
            static const BoundingRectangle kMaximumBounding;

            Sap2D(int aPairTableSizePower = PairTable::kMinSizePower);
            virtual ~Sap2D() {}

            CollisionHandler GetStartCollisionHandler() const { return mStartCollision; }
            CollisionHandler GetStopCollisionHandler() const { return mStopCollision; }
            CollisionHandler GetUpdateCollisionHandler() const { return mUpdateCollision; }

            void SetStartCollisionHandler(CollisionHandler h) { mStartCollision = h; }
            void SetStopCollisionHandler(CollisionHandler h) { mStopCollision = h; }
            void SetUpdateCollisionHandler(CollisionHandler h) { mUpdateCollision = h; }

            virtual ushort Add(void_p apCollideable, u32 aType, u32 aCollidesWith, const BoundingRectangle& aBounding) override;
            virtual float GetUnitMeter() const { return mUnitMeter; }
            virtual void Remove(ushort aHandle) override;
            virtual void SetUnitMeter(float v) override { mUnitMeter = v; }
            virtual void Tick() override;
            virtual void Update(u16 aHandle, const BoundingRectangle& aNewBounding) override;

        protected:
            float mUnitMeter;

            friend void ::jz::__IncrementRefCount<physics::Sap2D>(physics::Sap2D*);
            friend void ::jz::__DecrementRefCount<physics::Sap2D>(physics::Sap2D*);

            CollisionHandler mStartCollision;
            CollisionHandler mStopCollision;
            CollisionHandler mUpdateCollision;

            struct RectangleEntry
            {
                RectangleEntry(void_p apObject, u32 aType, u32 aCollidesWith)
                {
                    MaxX = -1;
                    MaxY = -1;

                    MinX = -1;
                    MinY = -1;

                    CollidesWith = aCollidesWith;
                    Type = aType;
                    Object = apObject;
                }

                union
                {
                    int Min[2];
                    struct
                    {
                        int MinX, MinY;
                    };
                };

                union
                {
                    int Max[2];
                    struct
                    {
                        int MaxX, MaxY;
                    };
                };

                u32 CollidesWith;
                u32 Type;
                void_p Object;

                uint MaxValue(int aAxisIndex, const vector<EndPoint>& a)
                {
                    return a[Max[aAxisIndex]].Value;
                }

                uint GetMinValue(int aAxisIndex, const vector<EndPoint>& a)
                {
                    return a[Min[aAxisIndex]].Value;
                }

                void AdjustMax(int aAxisIndex, int aValue)
                {
                    Max[aAxisIndex] += aValue;
                }

                void AdjustMin(int aAxisIndex, int aValue)
                {
                    Min[aAxisIndex] += aValue;
                }

                void SetMax(int aAxisIndex, int aValue)
                {
                    Max[aAxisIndex] = aValue;
                }

                void SetMin(int aAxisIndex, int aValue)
                {
                    Min[aAxisIndex] = aValue;
                }
            };

            void _AddHandler(const Pair& aPair)
            {
                if (mStartCollision)
                {
                    void_p a = mRectangles[aPair.A].Object;
                    void_p b = mRectangles[aPair.B].Object;

                    mStartCollision(a, b);
                }
            }
            
            void _RemoveHandler(const Pair& aPair)
            {
                if (mStopCollision)
                {
                    void_p a = mRectangles[aPair.A].Object;
                    void_p b = mRectangles[aPair.B].Object;

                    mStopCollision(a, b);
                }
            }

            void _UpdateHandler(const Pair& aPair)
            {
                if (mUpdateCollision)
                {
                    void_p a = mRectangles[aPair.A].Object;
                    void_p b = mRectangles[aPair.B].Object;

                    mUpdateCollision(a, b);
                }
            }

            static bool Collideable(const RectangleEntry& entry, const RectangleEntry& jEntry)
            {
                bool b = (entry.CollidesWith & jEntry.Type) != 0 &&
                         (entry.Type & jEntry.CollidesWith) != 0;

                return b;
            }

            static bool OneSidedIntersect(const RectangleEntry& aMin, const RectangleEntry& bMax, int axis)
            {
                bool bReturn = (bMax.Max[axis] >= aMin.Min[axis]);

                return bReturn;
            }

            static bool TwoSidedIntersect(const RectangleEntry& a, const RectangleEntry& b, int axis)
            {
                bool bReturn = (b.Max[axis] >= a.Min[axis] ||
                                b.Min[axis] <= a.Max[axis]);

                return bReturn;
            }

            AddressBuffer<RectangleEntry> mRectangles;
            vector<EndPoint> mAxes[2];
            vector<u16> mRemoves;

            PairTable mPairs;
            BitArray mPairRemoveCache;

            void _MoveMaxLeft(int aAxisIndex, EndPoint& n, int startIndex);
            void _MoveMaxRight(int aAxisIndex, EndPoint& n, int startIndex);
            void _MoveMinLeft(int aAxisIndex, EndPoint& n, int startIndex);
            void _MoveMinRight(int aAxisIndex, EndPoint& n, int startIndex);
            void _RemoveHelper(vector<int>& aToRemoves, int aAxisIndex);

            void _TickRemovesA()
            {
                if (mRemoves.size() > 0)
                {
                    mPairs.Remove(mPairRemoveCache);
                }
            }

            vector<int> mRemovesX;
            vector<int> mRemovesY;

            void _TickRemovesB();
            void _UpdateHelper(u16 aHandle, int aAxisIndex, int aOldIndex, uint aNewValue);

        private:
            Sap2D(const Sap2D&);
            Sap2D& operator=(const Sap2D&);
        };


        class Sap3D : public IBroadphase3D
        {
        public:
            typedef Delegate<void(void_p, void_p)> CollisionHandler;

            static const Vector3 kCollisionBoundary;
            static const BoundingBox kMaximumBounding;

            Sap3D(int aPairTableSizePower = PairTable::kMinSizePower);
            virtual ~Sap3D() {}

            CollisionHandler GetStartCollisionHandler() const { return mStartCollision; }
            CollisionHandler GetStopCollisionHandler() const { return mStopCollision; }
            CollisionHandler GetUpdateCollisionHandler() const { return mUpdateCollision; }

            void SetStartCollisionHandler(CollisionHandler h) { mStartCollision = h; }
            void SetStopCollisionHandler(CollisionHandler h) { mStopCollision = h; }
            void SetUpdateCollisionHandler(CollisionHandler h) { mUpdateCollision = h; }

            virtual ushort Add(void_p apCollideable, u32 aType, u32 aCollidesWith, const BoundingBox& aBounding) override;
            virtual float GetUnitMeter() const { return mUnitMeter; }
            virtual void Remove(ushort aHandle) override;
            virtual void SetUnitMeter(float v) override { mUnitMeter = v; }
            virtual void Tick() override;
            virtual void Update(u16 aHandle, const BoundingBox& aNewBounding) override;

        protected:
            float mUnitMeter;

            friend void ::jz::__IncrementRefCount<physics::Sap3D>(physics::Sap3D*);
            friend void ::jz::__DecrementRefCount<physics::Sap3D>(physics::Sap3D*);

            CollisionHandler mStartCollision;
            CollisionHandler mStopCollision;
            CollisionHandler mUpdateCollision;

            struct BoxEntry
            {
                BoxEntry(void_p apObject, u32 aType, u32 aCollidesWith)
                {
                    MaxX = -1;
                    MaxY = -1;
                    MaxZ = -1;

                    MinX = -1;
                    MinY = -1;
                    MinZ = -1;

                    CollidesWith = aCollidesWith;
                    Type = aType;
                    Object = apObject;
                }

                union
                {
                    int Min[3];
                    struct
                    {
                        int MinX, MinY, MinZ;
                    };
                };

                union
                {
                    int Max[3];
                    struct
                    {
                        int MaxX, MaxY, MaxZ;
                    };
                };

                u32 CollidesWith;
                u32 Type;
                void_p Object;

                uint MaxValue(int aAxisIndex, const vector<EndPoint>& a)
                {
                    return a[Max[aAxisIndex]].Value;
                }

                uint GetMinValue(int aAxisIndex, const vector<EndPoint>& a)
                {
                    return a[Min[aAxisIndex]].Value;
                }

                void AdjustMax(int aAxisIndex, int aValue)
                {
                    Max[aAxisIndex] += aValue;
                }

                void AdjustMin(int aAxisIndex, int aValue)
                {
                    Min[aAxisIndex] += aValue;
                }

                void SetMax(int aAxisIndex, int aValue)
                {
                    Max[aAxisIndex] = aValue;
                }

                void SetMin(int aAxisIndex, int aValue)
                {
                    Min[aAxisIndex] = aValue;
                }
            };

            void _AddHandler(const Pair& aPair)
            {
                if (mStartCollision)
                {
                    void_p a = mBoxes[aPair.A].Object;
                    void_p b = mBoxes[aPair.B].Object;

                    mStartCollision(a, b);
                }
            }
            
            void _RemoveHandler(const Pair& aPair)
            {
                if (mStopCollision)
                {
                    void_p a = mBoxes[aPair.A].Object;
                    void_p b = mBoxes[aPair.B].Object;

                    mStopCollision(a, b);
                }
            }

            void _UpdateHandler(const Pair& aPair)
            {
                if (mUpdateCollision)
                {
                    void_p a = mBoxes[aPair.A].Object;
                    void_p b = mBoxes[aPair.B].Object;

                    mUpdateCollision(a, b);
                }
            }

            static bool Collideable(const BoxEntry& entry, const BoxEntry& jEntry)
            {
                bool b = (entry.CollidesWith & jEntry.Type) != 0 &&
                         (entry.Type & jEntry.CollidesWith) != 0;

                return b;
            }

            static bool OneSidedIntersect(const BoxEntry& aMin, const BoxEntry& bMax, int axis)
            {
                bool bReturn = (bMax.Max[axis] >= aMin.Min[axis]);

                return bReturn;
            }

            static bool TwoSidedIntersect(const BoxEntry& a, const BoxEntry& b, int axis0, int axis1)
            {
                bool bReturn = !((b.Max[axis0] < a.Min[axis0]) ||
                                 (a.Max[axis0] < b.Min[axis0]) ||
                                 (b.Max[axis1] < a.Min[axis1]) ||
                                 (a.Max[axis1] < b.Min[axis1]));

                return bReturn;
            }

            AddressBuffer<BoxEntry> mBoxes;
            vector<EndPoint> mAxes[3];
            vector<u16> mRemoves;

            PairTable mPairs;
            BitArray mPairRemoveCache;

            void _MoveMaxLeft(int aAxisIndex, EndPoint& n, int startIndex);
            void _MoveMaxRight(int aAxisIndex, EndPoint& n, int startIndex);
            void _MoveMinLeft(int aAxisIndex, EndPoint& n, int startIndex);
            void _MoveMinRight(int aAxisIndex, EndPoint& n, int startIndex);
            void _RemoveHelper(vector<int>& aToRemoves, int aAxisIndex);

            void _TickRemovesA()
            {
                if (mRemoves.size() > 0)
                {
                    mPairs.Remove(mPairRemoveCache);
                }
            }

            vector<int> mRemovesX;
            vector<int> mRemovesY;
            vector<int> mRemovesZ;

            void _TickRemovesB();
            void _UpdateHelper(u16 aHandle, int aAxisIndex, int aOldIndex, uint aNewValue);

        private:
            Sap3D(const Sap3D&);
            Sap3D& operator=(const Sap3D&);
        };

    }
}

#endif
