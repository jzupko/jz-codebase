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

// Original license:
// --------------------------------------------------------------------
// lookup8.c, by Bob Jenkins, January 4 1997, Public Domain.
// hash(), hash2(), hash3, and mix() are externally useful functions.
// Routines to test the hash are included if SELF_TEST is defined.
// You can use this free for any purpose.  It has no warranty.
// --------------------------------------------------------------------

#pragma once
#ifndef _JZ_HASH_H_
#define _JZ_HASH_H_

#include <jz_core/StringUtility.h>

#include <cstddef>
#include <cstdio>
#include <cstdlib>

namespace jz
{

#   define hashsize(n) ((u64)1<<(n))
#   define hashmask(n) (hashsize(n)-1)

#   define mix64(a,b,c) \
    { \
        a -= b; a -= c; a ^= (c >> 43); \
        b -= c; b -= a; b ^= (a << 9);  \
        c -= a; c -= b; c ^= (b >> 8);  \
        a -= b; a -= c; a ^= (c >> 38); \
        b -= c; b -= a; b ^= (a << 23); \
        c -= a; c -= b; c ^= (b >> 5);  \
        a -= b; a -= c; a ^= (c >> 35); \
        b -= c; b -= a; b ^= (a << 49); \
        c -= a; c -= b; c ^= (b >> 11); \
        a -= b; a -= c; a ^= (c >> 12); \
        b -= c; b -= a; b ^= (a << 18); \
        c -= a; c -= b; c ^= (b >> 22); \
    }

    /*
    --------------------------------------------------------------------
    hash() -- hash a variable-length key into a 64-bit value
    k     : the key (the unaligned variable-length array of bytes)
    len   : the length of the key, counting by bytes
    level : can be any 8-byte value
    Returns a 64-bit value.  Every bit of the key affects every bit of
    the return value.  No funnels.  Every 1-bit and 2-bit delta achieves
    avalanche.  About 41+5len instructions.

    The best hash table sizes are powers of 2.  There is no need to do
    mod a prime (mod is sooo slow!).  If you need less than 64 bits,
    use a bitmask.  For example, if you need only 10 bits, do
    h = (h & hashmask(10));
    In which case, the hash table should have hashsize(10) elements.

    If you are hashing n strings (u8 **)k, do it like this:
    for (i=0, h=0; i<n; ++i) h = hash( k[i], len[i], h);

    By Bob Jenkins, Jan 4 1997.  bob_jenkins@burtleburtle.net.  You may
    use this code any way you wish, private, educational, or commercial,
    but I would appreciate if you give me credit.

    See http://burtleburtle.net/bob/hash/evahash.html
    Use for hash table lookup, or anything where one collision in 2^^64
    is acceptable.  Do NOT use for cryptographic purposes.
    --------------------------------------------------------------------
    */
    inline u64 hash64(u8c_p k, u32 length, u64 level)
    {
        register u64 a, b, c, len;

        /* Set up the internal state */
        len = length;
        a = b = level;                         /* the previous hash value */
        c = 0x9e3779b97f4a7c13LL; /* the golden ratio; an arbitrary value */

        /*---------------------------------------- handle most of the key */
        if (((size_t)k) & 7)
        {
            while (len >= 24)
            {
                a += (k[0]           +((u64)k[ 1]<< 8) + ((u64)k[ 2]<<16) + ((u64)k[ 3]<<24)
                  + ((u64)k[4 ]<<32) +((u64)k[ 5]<<40) + ((u64)k[ 6]<<48) + ((u64)k[ 7]<<56));
                b += (k[8]           +((u64)k[ 9]<< 8) + ((u64)k[10]<<16) + ((u64)k[11]<<24)
                  + ((u64)k[12]<<32) +((u64)k[13]<<40) + ((u64)k[14]<<48) + ((u64)k[15]<<56));
                c += (k[16]          +((u64)k[17]<< 8) + ((u64)k[18]<<16) + ((u64)k[19]<<24)
                  + ((u64)k[20]<<32) +((u64)k[21]<<40) + ((u64)k[22]<<48) + ((u64)k[23]<<56));

                mix64(a, b, c);

                k += 24; 
                len -= 24;
            }
        }
        else
        {
            while (len >= 24)    /* aligned */
            {
                a += *(u64 *)(k+0);
                b += *(u64 *)(k+8);
                c += *(u64 *)(k+16);

                mix64(a, b, c);

                k += 24;
                len -= 24;
            }
        }

        /*------------------------------------- handle the last 23 bytes */
        c += length;
        switch(len)              /* all the case statements fall through */
        {
            case 23: c += ((u64)k[22]<<56);
            case 22: c += ((u64)k[21]<<48);
            case 21: c += ((u64)k[20]<<40);
            case 20: c += ((u64)k[19]<<32);
            case 19: c += ((u64)k[18]<<24);
            case 18: c += ((u64)k[17]<<16);
            case 17: c += ((u64)k[16]<<8);
                /* the first byte of c is reserved for the length */
            case 16: b += ((u64)k[15]<<56);
            case 15: b += ((u64)k[14]<<48);
            case 14: b += ((u64)k[13]<<40);
            case 13: b += ((u64)k[12]<<32);
            case 12: b += ((u64)k[11]<<24);
            case 11: b += ((u64)k[10]<<16);
            case 10: b += ((u64)k[ 9]<<8);
            case  9: b += ((u64)k[ 8]);
            case  8: a += ((u64)k[ 7]<<56);
            case  7: a += ((u64)k[ 6]<<48);
            case  6: a += ((u64)k[ 5]<<40);
            case  5: a += ((u64)k[ 4]<<32);
            case  4: a += ((u64)k[ 3]<<24);
            case  3: a += ((u64)k[ 2]<<16);
            case  2: a += ((u64)k[ 1]<<8);
            case  1: a += ((u64)k[ 0]);
                /* case 0: nothing left to add */
        }

        mix64(a, b, c);
        
        /*-------------------------------------------- report the result */
        return c;
    }

    inline string hash64(const string& aString, u64 level)
    {
        u64 hash = hash64((u8_p)(aString.c_str()), aString.length(), level);

        return StringUtility::ToString(hash, true);
    }

}

#endif
