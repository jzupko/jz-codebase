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

#include <jz_physics/broadphase/PairTable.h>

namespace jz
{
    namespace physics
    {

        bool PairTable::_Add(u16 a, u16 b)
        {
            uint hash = Pair::Hash(a, b);
            u16 tableIndex = _GetIndex(hash);
            u16 pairIndex = mTable[tableIndex];

            while (pairIndex != mNull)
            {
                if (mPairs[pairIndex].Equals(a, b)) { return false; }

                pairIndex = mList[pairIndex].Next;
            }

            pairIndex = (u16)mPairCount;

            if (mSize <= mPairCount)
            {
                if (mSizePower == kMaxSizePower) { return false; }
                else
                {
    #if !NDEBUG
                    uint oldPairCount = mPairCount;
    #endif
                    _Grow();
    #if !NDEBUG
                    JZ_ASSERT(oldPairCount == mPairCount);
    #endif
                }
            }

            tableIndex = _GetIndex(hash);
            u16 nextPairIndex = mTable[tableIndex];
            JZ_ASSERT(nextPairIndex == mNull || nextPairIndex < mPairCount);
            JZ_ASSERT(pairIndex != nextPairIndex);

            // Insert the new pair at the head of the list.
            mList[pairIndex].Next = nextPairIndex;
            if (nextPairIndex != mNull) { mList[nextPairIndex].Prev = pairIndex; }
            mList[pairIndex].Prev = mNull;

            // Update the head pointer in the table to the new pair.
            mTable[tableIndex] = pairIndex;

            // Add the new pair into the pair array.
            mPairs[pairIndex] = Pair(a, b);
            mPairCount++;

            return true;
        }

        void PairTable::_Grow()
        {
            #pragma region Pre-resize
            uint oldPairCount = mPairCount;
            vector<Pair> oldPairs(mPairs);
            #pragma endregion

            #pragma region Resize
            _SetSize(mSizePower + 1);
            mPairCount = 0u;
            mList.resize(mSize);
            mPairs.clear(); mPairs.resize(mSize);
            mTable.resize(mSize);
            #pragma endregion

            #pragma region Clear
            for (uint i = 0; i < mSize; i++) { mList[i].Next = mNull; mList[i].Prev = mNull; }
            for (uint i = 0; i < mSize; i++) { mTable[i] = mNull; }
            #pragma endregion

            #pragma region Reinsert old pairs.
            // This is necessary as growing the table changes the mask and changes the index
            // that a hash value maps to.
            for (uint i = 0; i < oldPairCount; i++)
            {
                _Add(oldPairs[i].A, oldPairs[i].B);
                JZ_ASSERT(oldPairs[i] == mPairs[i]);
            }
            #pragma endregion
        }

        /// <summary>
        /// This ensures that the array is always contiguous.
        /// </summary>
        /// <param name="i">The index of the hole to patch.</param>
        void PairTable::_PatchHole(u16 i)
        {
            JZ_ASSERT(mPairCount > 0u);
            uint lastIndex = (mPairCount - 1u);
            JZ_ASSERT(lastIndex >= i);

            if (lastIndex > i)
            {
                // Move the last node into the i position.
                mPairs[i] = mPairs[lastIndex];
                mList[i] = mList[lastIndex];

                // Update the next and prev to the new position.
                if (mList[i].Next != mNull) { mList[mList[i].Next].Prev = i; }
                if (mList[i].Prev != mNull) { mList[mList[i].Prev].Next = i; }
                // if the index we used to patch is the head of its table entry, update the table entry.
                else
                {
                    uint hash = Pair::Hash(mPairs[i].A, mPairs[i].B);
                    u16 tableIndex = _GetIndex(hash);

                    mTable[tableIndex] = i;
                }
            }

            mPairCount--;
        }

        bool PairTable::_Remove(u16 a, u16 b)
        {
            uint hash = Pair::Hash(a, b);
            u16 tableIndex = _GetIndex(hash);
            u16 i = mTable[tableIndex];

            #pragma region If pair is the head of the table list.
            if (i != mNull)
            {
                if (mPairs[i].Equals(a, b))
                {
                    mTable[tableIndex] = mList[i].Next;
                    if (mList[i].Next != mNull) { mList[mList[i].Next].Prev = mNull; }

                    _PatchHole(i);

                    return true;
                }
            }
            else
            {
                return false;
            }
            #pragma endregion

            #pragma region Else
            for (i = mList[i].Next; i != mNull; i = mList[i].Next)
            {
                if (mPairs[i].Equals(a, b))
                {
                    if (mList[i].Next != mNull) { mList[mList[i].Next].Prev = mList[i].Prev; }
                    if (mList[i].Prev != mNull) { mList[mList[i].Prev].Next = mList[i].Next; }

                    _PatchHole(i);

                    return true;
                }
            }

            return false;
            #pragma endregion
        }
      
        void PairTable::_SetSize(int aSizePower)
        {
            mSizePower = aSizePower;
            // -1u to have a value for mNull - could cause overflow otherwise if the table reaches
            // the maximum size of a u16.
            mSize = (uint)(1 << mSizePower) - 1u;
            mMask = mSize - 1u;
            mNull = (u16)mSize;
        }

    }
}
