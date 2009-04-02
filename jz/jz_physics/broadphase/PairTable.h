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
#ifndef _JZ_PHYSICS_PAIR_TABLE_H_
#define _JZ_PHYSICS_PAIR_TABLE_H_

#include <jz_core/Delegate.h>
#include <vector>

namespace jz
{
    namespace physics
    {

        typedef vector<bool> BitArray;

        struct Pair
        {
            Pair()
                : A(Constants<u16>::kMax), B(Constants<u16>::kMax)
            {}

            Pair(u16 a, u16 b)
            {
                A = a;
                B = b;

                Order(A, B);
            }

            bool Equals(u16 a, u16 b)
            {
                Order(a, b);

                return (A == a) && (B == b);
            }

            uint Hash() const
            {
                return Hash(A, B);
            }

            u16 A;
            u16 B;

            // From: http://www.concentric.net/~Ttwang/tech/inthash.htm
            static uint Hash(u16 a, u16 b)
            {
                Order(a, b);

                uint c = (((uint)a) | (((uint)b) << 16));

                c = ~c + (c << 15);
                c = c ^ (c >> 12);
                c = c + (c << 2);
                c = c ^ (c >> 4);
                c = c * 2057;
                c = c ^ (c >> 16);

                return c;
            }

            static void Order(u16& a, u16& b)
            {
                if (a > b) { jz::Swap(a, b); }
            }
        };

        inline bool operator<(const Pair& a, const Pair& b)
        {
            if (a.A == b.A) { return (a.B < b.B); }
            else { return (a.A < b.A); }
        }

        inline bool operator>(const Pair& a, const Pair& b)
        {
            if (a.A == b.A) { return (a.B > b.B); }
            else { return (a.A > b.A); }
        }

        inline bool operator==(const Pair& a, const Pair& b)
        {
            return (a.A == b.A) && (a.B == b.B);
        }

        inline bool operator !=(const Pair& a, const Pair& b)
        {
            return (a.A != b.A) || (a.B != b.B);
        }

        /// <summary>
        /// Called when a pair is added or removed.
        /// </summary>
        /// <param name="aPair">The added or removed Pair.</param>
        /// <remarks>
        /// Pair add/removes are batched and reported once per PairTable.Tick() call.
        /// </remarks>
        typedef Delegate<void(const Pair& aPair)> PairCallback;

        /// <summary>
        /// Handles management of pairs.
        /// </summary>
        /// <remarks>
        /// A pair in the physics system is two intersecting collision objects that require
        /// restitution.
        /// 
        /// Uses a combination hash-table with chaining and hash-table with open addressing idea.
        /// The array is effectively a double-linked list that uses array indices instead of pointers.
        /// Pairs are added contiguously into the array. When a pair is removed, the last pair in
        /// the array is placed in the hole and its "pointers" are updated to maintain contiguous
        /// ordering. Everthing is O(1) except for find (and consequently, remove) operations, which
        /// area O(n) in the worst case. This is dependent on the amount of collisions in the hash
        /// table which is usually dependent on the quality of the hashing function.
        /// 
        /// Based on ICE PairManager: http://www.codercorner.com/PairManager.rar .
        /// </remarks>
        class PairTable
        {
        public:
            static const int kMinSizePower = (sizeof(u8) * 8);
            static const int kMaxSizePower = (sizeof(u16) * 8);

            PairTable(int aSizePower = kMinSizePower)
                : mPairCount(0u)
            {
                _SetSize(jz::Clamp(aSizePower - 1, kMinSizePower - 1, kMaxSizePower - 1));
                _Grow();
            }

            void Add(u16 a, u16 b)
            {
                if (a != b)
                {
                    if (_Add(a, b)) { mAdds.push_back(Pair(a, b)); }
                }
            }

            bool Get(u16 a, u16 b, Pair& arPair)
            {
                if (a == b) { return false; }

                uint hash = Pair::Hash(a, b);
                uint tableIndex = _GetIndex(hash);

                if (mTable[tableIndex] == mNull) { return false; }
                else
                {
                    for (uint i = mTable[tableIndex]; i != mNull; i = mList[i].Next)
                    {
                        if (mPairs[i].Equals(a, b))
                        {
                            arPair = mPairs[i];
                            return true;
                        }
                    }

                    return false;
                }
            }

            void Remove(u16 a)
            {
                for (uint i = 0; i < mPairCount; i++)
                {
                    if (mPairs[i].A == a || mPairs[i].B == a)
                    {
                        Remove(mPairs[i].A, mPairs[i].B);
                        i--;
                    }
                }
            }

            void Remove(u16 a, u16 b)
            {
                if (a != b)
                {
                    if (_Remove(a, b))
                    {
                        mRemoves.push_back(Pair(a, b));
                    }
                }
            }

            void Remove(const BitArray& aPairs)
            {
                for (uint i = 0; i < mPairCount; i++)
                {
                    u16 a = mPairs[i].A;
                    u16 b = mPairs[i].B;

                    if (aPairs[a] || aPairs[b])
                    {
                        Remove(a, b);

                        // Since we know the pair is in the list, it will always be removed,
                        // reducing pair count by 1 and placing a new pair in this index.
                        i--;
                    }
                }
            }

            void Tick(PairCallback aAddedCallback, PairCallback aUpdatedCallback, PairCallback aRemovedCallback)
            {
                _TickHelper(mAdds, aAddedCallback);
                _TickHelper(mRemoves, aRemovedCallback);

                for (uint i = 0; i < mPairCount; i++)
                {
                    aUpdatedCallback(mPairs[i]);
                }
            }

        private:
            uint mMask;
            u16 mNull;
            int mSizePower;
            uint mSize;

            struct Node
            {
                u16 Next;
                u16 Prev;
            };

            vector<Pair> mAdds;
            vector<Pair> mRemoves;
            uint mPairCount;

            // mList is effectively the prev/next pointers for the linked-list of Pair elements
            // in mPairs. It is stored as a separate array to allow the mPairs array to be returned
            // as a contiguous array of pairs.
            vector<Node> mList;
            vector<Pair> mPairs;
            vector<u16> mTable;

            bool _Add(u16 a, u16 b);
            void _Grow();

            /// <summary>
            /// This ensures that the array is always contiguous.
            /// </summary>
            /// <param name="i">The index of the hole to patch.</param>
            void _PatchHole(u16 i);

            bool _Remove(u16 a, u16 b);      
            void _SetSize(int aSizePower);

            u16 _GetIndex(uint aHash)
            {
                u16 ret = (u16)(aHash & mMask);

                return ret;
            }

            void _TickHelper(vector<Pair>& aList, PairCallback aCallback)
            {
                int count = aList.size();
                for (int i = 0; i < count; i++)
                {
                    aCallback(aList[i]);
                }

                aList.clear();
            }

        };

    }
}

#endif
