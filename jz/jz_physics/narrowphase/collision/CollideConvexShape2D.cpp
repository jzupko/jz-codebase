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

#include <jz_core/CoordinateFrame2D.h>
#include <jz_physics/narrowphase/WorldContactPoint.h>
#include <jz_physics/narrowphase/collision/Collide.h>
#include <jz_physics/narrowphase/collision/ConvexShape.h>

namespace jz
{
    namespace physics
    {

        // Based on Box 2D - (C) 2009 Erin Catto, http://www.gphysics.com/

        // Note: I believe this is a 2D implementation of vclip.
        // Todo: it seems that the searching for best edge can be improved by hill climbing.
        // Idea:
        //    - store the indices of edges closest to -x, +x, -y, +y as 8-bit indices
        //    - add a function to ConvexPolygonShape that returns this index given a starting
        //      direction vector
        //    - this function should also output the iteration direction
        //      -1 the edge previous to the returned edge is a better edge.
        //       0 the edge returned is the best edged, neither of the two nearest edges are better
        //       1 the edge next to the returned edge is a better edge.
        namespace Collide
        {

            static int ClipSegmentToLine(
                Vector2 vIn[2], const Vector2& normal, float offset, Vector2 vOut[2])
            {
                int ret = 0;

                float d0 = Vector2::Dot(normal, vIn[0]) - offset;
                float d1 = Vector2::Dot(normal, vIn[1]) - offset;

                if (d0 <= 0.0f) { vOut[ret++] = vIn[0]; }
                if (d1 <= 0.0f) { vOut[ret++] = vIn[1]; }

                if ((d0 * d1) < 0.0f)
                {
                    float t = (d0 / (d0 - d1));
                    vOut[ret++] = (vIn[0] + (t * (vIn[1] - vIn[0])));
                }

                return ret;
            }

            // Finds the edge of b with the minimum dot product relative to edge of A.
            static void FindIncidentEdge(
                ConvexShape2D* a, const CoordinateFrame2D& acf,
                ConvexShape2D* b, const CoordinateFrame2D& bcf,
                int edge, Vector2 c[2])
            {
                const ConvexShape2D::Container& normalsA = a->GetNormals();
                const ConvexShape2D::Container& normalsB = b->GetNormals();
                const ConvexShape2D::Container& pointsB = b->GetPoints();

                const int kSizeA = (int)normalsA.size();
                const int kSizeB = (int)normalsB.size();

                Vector2 normal = Vector2::TransformDirection((acf * CoordinateFrame2D::Invert(bcf)), normalsA[edge]);

                int index = 0;
                float minDot = Constants<float>::kMax;
                for (int i = 0; i < kSizeB; i++)
                {
                    float dot = Vector2::Dot(normal, normalsB[i]);
                    if (dot < minDot)
                    {
                        minDot = dot;
                        index = i;
                    }
                }

                int i1 = index;
                int i0 = (i1 - 1 >= 0) ? (i1 - 1) : (kSizeB - 1);

                c[0] = Vector2::TransformPosition(bcf, pointsB[i0]);
                c[1] = Vector2::TransformPosition(bcf, pointsB[i1]);
            }

            static float GetDistance(
                ConvexShape2D* a, const CoordinateFrame2D& acf,
                ConvexShape2D* b, const CoordinateFrame2D& bcf,
                int edge)
            {
                const ConvexShape2D::Container& normalsA = a->GetNormals();
                const ConvexShape2D::Container& pointsA = a->GetPoints();
                const ConvexShape2D::Container& pointsB = b->GetPoints();

                CoordinateFrame2D m = (acf * CoordinateFrame2D::Invert(bcf));

                const int kSizeA = (int)pointsA.size();
                const int kSizeB = (int)pointsB.size();

                Vector2 normalWorld = Vector2::TransformDirection(acf, normalsA[edge]);
                Vector2 normal = Vector2::TransformDirection(CoordinateFrame2D::Invert(bcf), normalWorld);

                int index = 0;
                float minDot = Constants<float>::kMax;

                for (int i = 0u; i < kSizeB; i++)
                {
                    float dot = Vector2::Dot(pointsB[i], normal);
                    if (dot < minDot)
                    {
                        minDot = dot;
                        index = i;
                    }
                }

                float ret = Vector2::Dot(
                    Vector2::TransformPosition(bcf, pointsB[index]) -
                    Vector2::TransformPosition(acf, pointsA[edge]),
                    normalWorld);

                return ret;
            }
            
            static float GetMaxDistance(
                ConvexShape2D* a, const CoordinateFrame2D& acf,
                ConvexShape2D* b, const CoordinateFrame2D& bcf,
                int& arEdgeIndex)
            {
                const ConvexShape2D::Container& normals = a->GetNormals();
                const ConvexShape2D::Container& points = a->GetPoints();
                
                CoordinateFrame2D m = (bcf * CoordinateFrame2D::Invert(acf));

                const int kSize = (int)points.size();

                Vector2 d = Vector2::TransformPosition(m, b->GetBounding().Center()) - a->GetBounding().Center();

                // Do hill climbing here, and also allow for warm starting by storing the previous
                // starting edge index.
                float bestDot = Constants<float>::kMin;
                int prev = (int)(kSize - 1u);
                int edge = 0;
                int p = (int)(kSize - 1u);
                for (int i = 0; i < kSize; i++)
                {
                    float dot = Vector2::Dot(normals[i], d);
                    if (dot > bestDot)
                    {
                        bestDot = dot;
                        edge = i;
                        prev = p;
                    }
                    p = i;
                }

                float s = GetDistance(a, acf, b, bcf, edge);
                if (s > 0.0f) { return s; }

                float sPrev = GetDistance(a, acf, b, bcf, prev);
                if (sPrev > 0.0f) { return sPrev; }

                int next = (edge + 1 < kSize) ? (edge + 1) : 0;
                float sNext = GetDistance(a, acf, b, bcf, next);
                if (sNext > 0.0f) { return sNext; }

                int bestEdge;
                float bestS;
                int increment;

                if ((sPrev > s) && (sPrev > sNext))
                {
                    increment = -1;
                    bestEdge = prev;
                    bestS = sPrev;
                }
                else if (sNext > s)
                {
                    increment = 1;
                    bestEdge = next;
                    bestS = sNext;
                }
	            else
	            {
                    arEdgeIndex = edge;
                    return s;
                }

                while (true)
                {
                    if (increment == -1) { edge = (bestEdge - 1 >= 0) ? (bestEdge - 1) : (kSize - 1); }
                    else { edge = (bestEdge + 1 < kSize) ? (bestEdge + 1) : 0; }

                    s = GetDistance(a, acf, b, bcf, edge);
                    if (s > 0.0f) { return s; }
                    else if (s > bestS)
                    {
                        bestEdge = edge;
                        bestS = s;
                    }
                    else
                    {
                        break;
                    }
                }

                arEdgeIndex = bestEdge;
                return bestS;
            }

            bool Collide(
                ConvexShape2D* a, const CoordinateFrame2D& acf,
                ConvexShape2D* b, const CoordinateFrame2D& bcf,
                WorldContactPoint2D& cp)
            {
                int edgeA = 0;
                float sA = GetMaxDistance(a, acf, b, bcf, edgeA);
                if (sA > 0.0f) { return false; }

                int edgeB = 0;
                float sB = GetMaxDistance(b, bcf, a, acf, edgeB);
                if (sB > 0.0f) { return false; }

                ConvexShape2D* pa;
                ConvexShape2D* pb;
                CoordinateFrame2D const* pacf;
                CoordinateFrame2D const* pbcf;
                int edge;
                bool bFlip;
                
                const float kRelativeTolerance = 0.98f;
                const float kAbsoluteTolerance = 0.001f;

                if (sB > ((kRelativeTolerance * sA) + kAbsoluteTolerance))
                {
                    pa = b;
                    pb = a;
                    pacf = &bcf;
                    pbcf = &acf;
                    edge = edgeB;
                    bFlip = true;
                }
                else
                {
                    pa = a;
                    pb = b;
                    pacf = &acf;
                    pbcf = &bcf;
                    edge = edgeA;
                    bFlip = false;
                }

                Vector2 incidentEdge[2];
                FindIncidentEdge(pa, *pacf, pb, *pbcf, edge, incidentEdge);

                const ConvexShape2D::Container& pointsA = pa->GetPoints();
                const int kSizeA = (int)pointsA.size();

                Vector2 v12 = pointsA[edge];
                Vector2 v11 = (edge - 1 >= 0) ? (pointsA[edge - 1]) : (pointsA[kSizeA - 1]);

                Vector2 dv = (v12 - v11);
                Vector2 sideNormal = Vector2::Normalize(Vector2::TransformDirection(*pacf, dv));
                Vector2 frontNormal = Vector2(-sideNormal.Y, sideNormal.X);

                v11 = Vector2::TransformPosition(*pacf, v11);
                v12 = Vector2::TransformPosition(*pacf, v12);

                float frontOffset = Vector2::Dot(frontNormal, v11);
                float sideOffset1 = -Vector2::Dot(sideNormal, v11);
                float sideOffset2 = Vector2::Dot(sideNormal, v12);

                Vector2 clipPoints1[2];
                Vector2 clipPoints2[2];
                
                int np = ClipSegmentToLine(incidentEdge, -sideNormal, sideOffset1, clipPoints1);
                if (np < 2) { return false; }

	            np = ClipSegmentToLine(clipPoints1,  sideNormal, sideOffset2, clipPoints2);
                if (np < 2) { return false; }

                cp.WorldNormal = (bFlip) ? (frontNormal) : (-frontNormal);
                cp.WorldPointB = Vector2::Lerp(clipPoints2[0], clipPoints2[1], 0.5f);
                cp.WorldPointA = Vector2::Lerp(v12, v11, 0.5f);

                return true;
            }

        }

    }
}

