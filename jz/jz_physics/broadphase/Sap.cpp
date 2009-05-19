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

#include <jz_physics/broadphase/Sap.h>
#include <algorithm>

namespace jz
{
    namespace physics
    {

        const uint EndPoint::kMinMaxFlag = (1 << 0);
        const int EndPoint::kOwnerShift = (int)kMinMaxFlag;
        const uint EndPoint::kOwnerMask = Constants<uint>::kMax & (~kMinMaxFlag);

        const u16 EndPoint::kSentinelId = Constants<u16>::kMax;
        const uint EndPoint::kSentinelMin = Constants<uint>::kMin;
        const uint EndPoint::kSentinelMax = Constants<uint>::kMax;

        __inline uint GetSortableUintFromFloat(float s)
        {
            uint i = *((uint*)&s);

            // 0x80000000 is hex for the sign bit of an IEEE floating point number.
            // if negative, reverse sequence
            if ((i & 0x80000000) != 0) { i = ~i; }
            // otherwise, flip sign.
            else { i |= 0x80000000; }

            return i;
        }

        const Vector3 Sap3D::kCollisionBoundary = Vector3(1e-2f);
        const BoundingBox Sap3D::kMaximumBounding = BoundingBox(Vector3(Constants<float>::kMin) + Vector3(1), Vector3(Constants<float>::kMax) - Vector3(1));

        Sap3D::Sap3D(int aPairTableSizePower)
            : mPairs(aPairTableSizePower),
            mPairRemoveCache(Constants<u16>::kMax),
            mUnitMeter(1.0f)
        {
            for (int i = 0; i < 3; i++)
            {
                // Order of adds is important to maintain ascending order.
                mAxes[i].push_back(EndPoint(false, EndPoint::kSentinelId, EndPoint::kSentinelMin));
                mAxes[i].push_back(EndPoint(true, EndPoint::kSentinelId, EndPoint::kSentinelMax));
            }
        }

        ushort Sap3D::Add(void_p apCollideable, u32 aType, u32 aCollidesWith, const BoundingBox& aBounding)
        {
            JZ_ASSERT(apCollideable != null);

            BoundingBox abr = aBounding;
            abr.Max += (kCollisionBoundary * mUnitMeter);
            abr.Min -= (kCollisionBoundary * mUnitMeter);

            abr = BoundingBox::Clamp(abr, kMaximumBounding);

            ushort handle = (ushort)mBoxes.Add(BoxEntry(apCollideable, aType, aCollidesWith));

            // I don't like this. Need a more graceful way of handling this.
            if (handle == EndPoint::kSentinelId) { throw exception("exceeded maximum number of physical objects."); }

            for (int i = 0; i < 3; i++)
            {
                int min = ((int)mAxes[i].size()) - 1;
                int max = ((int)mAxes[i].size());
                mAxes[i][min] = EndPoint(false, handle, EndPoint::kSentinelMax - 2);
                mAxes[i].push_back(EndPoint(true, handle, EndPoint::kSentinelMax - 1));
                mAxes[i].push_back(EndPoint(true, EndPoint::kSentinelId, EndPoint::kSentinelMax));

                mBoxes[handle].SetMin(i, min);
                mBoxes[handle].SetMax(i, max);
            }

            Update(handle, aBounding);

            return handle;
        }

        void Sap3D::Remove(ushort aHandle)
        {
            JZ_ASSERT(aHandle != EndPoint::kSentinelId);

            if (!mPairRemoveCache[aHandle])
            {
                mPairRemoveCache[aHandle] = true;
                mRemoves.push_back(aHandle);
            }
        }

        void Sap3D::Tick()
        {
            // Remove the axis entries, then the pairs (and dispatch callbacks, which need the object entries), 
            // then remove the object entires.
            _TickRemovesA();

            mPairs.Tick(
                PairCallback::Bind<Sap3D, &Sap3D::_AddHandler>(this),
                PairCallback::Bind<Sap3D, &Sap3D::_UpdateHandler>(this),
                PairCallback::Bind<Sap3D, &Sap3D::_RemoveHandler>(this));

            _TickRemovesB();
        }

        void Sap3D::Update(u16 aHandle, const BoundingBox& aNewBounding)
        {
            JZ_ASSERT(aHandle != EndPoint::kSentinelId);

            BoundingBox bb = aNewBounding;
            bb.Max += (kCollisionBoundary * mUnitMeter);
            bb.Min -= (kCollisionBoundary * mUnitMeter);

            bb = BoundingBox::Clamp(bb, kMaximumBounding);

            // If in the pair remove cache, this entry is going to be removed anyway so don't update.
            if (!mPairRemoveCache[aHandle])
            {
                _UpdateHelper(aHandle, Axis::kX, mBoxes[aHandle].MinX, GetSortableUintFromFloat(bb.Min.X));
                _UpdateHelper(aHandle, Axis::kX, mBoxes[aHandle].MaxX, GetSortableUintFromFloat(bb.Max.X));
                _UpdateHelper(aHandle, Axis::kY, mBoxes[aHandle].MinY, GetSortableUintFromFloat(bb.Min.Y));
                _UpdateHelper(aHandle, Axis::kY, mBoxes[aHandle].MaxY, GetSortableUintFromFloat(bb.Max.Y));
                _UpdateHelper(aHandle, Axis::kZ, mBoxes[aHandle].MinZ, GetSortableUintFromFloat(bb.Min.Z));
                _UpdateHelper(aHandle, Axis::kZ, mBoxes[aHandle].MaxZ, GetSortableUintFromFloat(bb.Max.Z));
            }
        }

        void Sap3D::_MoveMaxLeft(int axis0, EndPoint& n, int startIndex)
        {
            int axis1 = (1 << axis0) & 3;
            int axis2 = (1 << axis1) & 3;

            u16 nhandle = n.OwnerId();
            vector<EndPoint>& data = mAxes[axis0];

            int j = startIndex - 1;
            for (; n.Value < data[j].Value; j--)
            {
                ushort jhandle = data[j].OwnerId();
                data[j + 1] = data[j];

                if (!data[j + 1].IsMax())
                {
                    mBoxes[jhandle].AdjustMin(axis0, 1);

                    // Note: Collideable() test is not used for removes as it allows the
                    //  type or collideable masks to be changed for an object while it is
                    //  colliding.
                    if (TwoSidedIntersect(mBoxes[nhandle], mBoxes[jhandle], axis1, axis2))
                    {
                        mPairs.Remove(nhandle, jhandle);
                    }
                }
                else
                {
                    mBoxes[jhandle].AdjustMax(axis0, 1);
                }
            }

            data[j + 1] = n;
            mBoxes[nhandle].SetMax(axis0, j + 1);
        }

        void Sap3D::_MoveMaxRight(int axis0, EndPoint& n, int startIndex)
        {
            int axis1 = (1 << axis0) & 3;
            int axis2 = (1 << axis1) & 3;

            ushort nhandle = n.OwnerId();
            vector<EndPoint>& data = mAxes[axis0];

            int j = startIndex + 1;
            for (; n.Value > data[j].Value; j++)
            {
                ushort jhandle = data[j].OwnerId();
                data[j - 1] = data[j];

                if (!data[j - 1].IsMax())
                {
                    mBoxes[jhandle].AdjustMin(axis0, -1);

                    if (Collideable(mBoxes[nhandle], mBoxes[jhandle])
                        // Note that the order of parameters to OneSidedIntersect matters.
                        // In this case, we are checking the minimum of the box we
                        // are updating against the maximum of the current box.
                        && OneSidedIntersect(mBoxes[nhandle], mBoxes[jhandle], axis0)
                        && TwoSidedIntersect(mBoxes[nhandle], mBoxes[jhandle], axis1, axis2))
                    {
                        mPairs.Add(nhandle, jhandle);
                    }
                }
                else
                {
                    mBoxes[jhandle].AdjustMax(axis0, -1);
                }
            }

            data[j - 1] = n;
            mBoxes[nhandle].SetMax(axis0, j - 1);
        }

        void Sap3D::_MoveMinLeft(int axis0, EndPoint& n, int startIndex)
        {
            int axis1 = (1 << axis0) & 3;
            int axis2 = (1 << axis1) & 3;

            ushort nhandle = n.OwnerId();
            vector<EndPoint>& data = mAxes[axis0];

            int j = startIndex - 1;
            for (; n.Value < data[j].Value; j--)
            {
                ushort jhandle = data[j].OwnerId();
                data[j + 1] = data[j];

                if (data[j + 1].IsMax())
                {
                    mBoxes[jhandle].AdjustMax(axis0, 1);

                    if (Collideable(mBoxes[nhandle], mBoxes[jhandle])
                        // Note that the order of parameters to OneSidedIntersect matters.
                        // In this case, we are checking the maximum of the box we
                        // are updating against the minimum of the current box.
                        && OneSidedIntersect(mBoxes[jhandle], mBoxes[nhandle], axis0)
                        && TwoSidedIntersect(mBoxes[nhandle], mBoxes[jhandle], axis1, axis2))
                    {
                        mPairs.Add(nhandle, jhandle);
                    }
                }
                else
                {
                    mBoxes[jhandle].AdjustMin(axis0, 1);
                }
            }

            data[j + 1] = n;
            mBoxes[nhandle].SetMin(axis0, j + 1);
        }

        void Sap3D::_MoveMinRight(int axis0, EndPoint& n, int startIndex)
        {
            int axis1 = (1 << axis0) & 3;
            int axis2 = (1 << axis1) & 3;

            ushort nhandle = n.OwnerId();
            vector<EndPoint>& data = mAxes[axis0];

            int j = startIndex + 1;
            for (; n.Value > data[j].Value; j++)
            {
                ushort jhandle = data[j].OwnerId();
                data[j - 1] = data[j];

                if (data[j - 1].IsMax())
                {
                    mBoxes[jhandle].AdjustMax(axis0, -1);

                    // Note: Collideable() test is not used for removes as it allows the
                    //  type or collideable masks to be changed for an object while it is
                    //  colliding.
                    if (TwoSidedIntersect(mBoxes[nhandle], mBoxes[jhandle], axis1, axis2))
                    {
                        mPairs.Remove(nhandle, jhandle);
                    }
                }
                else
                {
                    mBoxes[jhandle].AdjustMin(axis0, -1);
                }
            }

            data[j - 1] = n;
            mBoxes[nhandle].SetMin(axis0, j - 1);
        }

        void Sap3D::_RemoveHelper(vector<int>& aToRemoves, int axis)
        {
            int count = aToRemoves.size();
            if (count > 0)
            {
                std::sort(aToRemoves.begin(), aToRemoves.end());
                vector<EndPoint>& a = mAxes[axis];

                for (int i = 0; i < count - 1; i++)
                {
                    int offset = (i + 1);
                    int start = aToRemoves[i];
                    int end = aToRemoves[i + 1] - offset;

                    // 0 and -1 to avoid touching sentinels.
                    JZ_ASSERT(start > 0 && start < (int)a.size() - 1);
                    JZ_ASSERT(end > 0 && end < (int)a.size() - 1);

                    for (int j = start; j <= end; j++)
                    {
                        a[j] = a[j + offset];
                        if (a[j].IsMax()) { mBoxes[a[j].OwnerId()].AdjustMax(axis, -offset); }
                        else { mBoxes[a[j].OwnerId()].AdjustMin(axis, -offset); }
                    }
                }

                int newAxisCount = (a.size() - count);

                for (int i = (aToRemoves[count - 1] - count + 1); i < newAxisCount - 1; i++)
                {
                    a[i] = a[i + count];
                    if (a[i].IsMax()) { mBoxes[a[i].OwnerId()].AdjustMax(axis, -count); }
                    else { mBoxes[a[i].OwnerId()].AdjustMin(axis, -count); }
                }

                // move sentinel.
                a[newAxisCount - 1] = a[a.size() - 1];

                a.resize(newAxisCount);
                aToRemoves.clear();
            }
        }
        
        void Sap3D::_TickRemovesB()
        {
            int count = (int)mRemoves.size();
            if (count > 0)
            {
                for (int i = 0; i < count; i++)
                {
                    ushort handle = mRemoves[i];
                    mPairRemoveCache[handle] = false;

                    mRemovesX.push_back(mBoxes[handle].MinX);
                    mRemovesX.push_back(mBoxes[handle].MaxX);
                    mRemovesY.push_back(mBoxes[handle].MinY);
                    mRemovesY.push_back(mBoxes[handle].MaxY);
                    mRemovesZ.push_back(mBoxes[handle].MinZ);
                    mRemovesZ.push_back(mBoxes[handle].MaxZ);
                }

                _RemoveHelper(mRemovesX, 0);
                _RemoveHelper(mRemovesY, 1);
                _RemoveHelper(mRemovesZ, 2);

                for (int i = 0; i < count; i++)
                {
                    ushort handle = mRemoves[i];
                    mBoxes[handle].Object = null;
                    mBoxes.remove(handle);
                }
                mRemoves.clear();
            }
        }

        void Sap3D::_UpdateHelper(u16 aHandle, int aAxisIndex, int aOldIndex, uint aNewValue)
        {
            int axisCount = mAxes[aAxisIndex].size();
            vector<EndPoint>& data = mAxes[aAxisIndex];

            uint oldValue = data[aOldIndex].Value;

            // Move to the right.
            if (aNewValue > oldValue)
            {
                bool bMax = data[aOldIndex].IsMax();
                EndPoint n = EndPoint(bMax, aHandle, aNewValue);

                if (bMax) { _MoveMaxRight(aAxisIndex, n, aOldIndex); }
                else { _MoveMinRight(aAxisIndex, n, aOldIndex); }
            }
            // Move to the left.
            else if (aNewValue < oldValue)
            {
                bool bMax = data[aOldIndex].IsMax();
                EndPoint n = EndPoint(bMax, aHandle, aNewValue);

                if (bMax) { _MoveMaxLeft(aAxisIndex, n, aOldIndex); }
                else { _MoveMinLeft(aAxisIndex, n, aOldIndex); }
            }
            // else, do nothing - value hasn't changed
        }

    }
}
