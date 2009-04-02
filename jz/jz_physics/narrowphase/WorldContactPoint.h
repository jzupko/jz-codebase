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
#ifndef _JZ_PHYSICS_WORLD_CONTACT_POINT_H_
#define _JZ_PHYSICS_WORLD_CONTACT_POINT_H_

#include <jz_core/Vector2.h>
#include <jz_core/Vector3.h>

namespace jz
{
    namespace physics
    {

        struct WorldContactPoint2D
        {
            static const float kDistanceTolerance;
            static const float kNormalTolerance;

            Vector2 WorldNormal;
            Vector2 WorldPointA;
            Vector2 WorldPointB;

            Vector2 Center() const
            {
                return ((WorldPointA + WorldPointB) * 0.5f);
            }

            float GetSeparationSquared() const
            {
                return (Vector2::DistanceSquared(WorldPointA, WorldPointB));
            }

            static bool AboutEqual(const WorldContactPoint2D& a, const WorldContactPoint2D& b)
            {
                Vector2 ac = 0.5f * (a.WorldPointA + a.WorldPointB);
                Vector2 bc = 0.5f * (b.WorldPointA + b.WorldPointB);

                bool bReturn = (Vector2::DistanceSquared(ac, bc) < kDistanceTolerance);

                return bReturn;
            }

            static WorldContactPoint2D Flip(const WorldContactPoint2D& a)
            {
                WorldContactPoint2D ret;
                ret.WorldNormal = -a.WorldNormal;
                ret.WorldPointA =  a.WorldPointB;
                ret.WorldPointB =  a.WorldPointA;

                return ret;
            }
        };

        struct WorldContactPoint3D
        {
            static const float kDistanceTolerance;
            static const float kNormalTolerance;

            Vector3 WorldNormal;
            Vector3 WorldPointA;
            Vector3 WorldPointB;

            Vector3 Center() const
            {
                return ((WorldPointA + WorldPointB) * 0.5f);
            }

            float GetSeparationSquared() const
            {
                return (Vector3::DistanceSquared(WorldPointA, WorldPointB));
            }

            static bool AboutEqual(const WorldContactPoint3D& a, const WorldContactPoint3D& b)
            {
                Vector3 ac = 0.5f * (a.WorldPointA + a.WorldPointB);
                Vector3 bc = 0.5f * (b.WorldPointA + b.WorldPointB);

                bool bReturn = (Vector3::DistanceSquared(ac, bc) < kDistanceTolerance);

                return bReturn;
            }

            static WorldContactPoint3D Flip(const WorldContactPoint3D& a)
            {
                WorldContactPoint3D ret;
                ret.WorldNormal = -a.WorldNormal;
                ret.WorldPointA =  a.WorldPointB;
                ret.WorldPointB =  a.WorldPointA;

                return ret;
            }
        };

    }
}

#endif
