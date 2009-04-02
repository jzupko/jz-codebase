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

#include <jz_core/Ray2D.h>
#include <jz_pathfinding/AStar.h>
#include <jz_pathfinding/PathGrid.h>
#include <cstring>

namespace jz
{
    namespace pathfinding
    {

        PathGrid::PathGrid()
        {}

        PathGrid::~PathGrid()
        {}

        size_t PathGrid::Add(const BoundingRectangle& v)
        {
            size_t ret = 0u;

            if (mFreeList.size() > 0u)
            {
                ret = mFreeList.back();
                mFreeList.pop_back();
                mObjects[ret] = v;
            }
            else
            {
                ret = mObjects.size();
                mObjects.push_back(v);
            }

            return ret;
        }

        void PathGrid::Remove(size_t v)
        {
            mFreeList.push_back(v);
        }

        void PathGrid::Update(size_t aObject, const BoundingRectangle& v)
        {
            mObjects[aObject] = v;
        }

        static float Dist(u16 i, u16 j, void_p apUserData)
        {
            vector<Vector2>& points = *static_cast<vector<Vector2>*>(apUserData);

            float dis = Abs(points[i].X - points[j].X) + Abs(points[i].Y - points[j].Y);

            return (dis);
        }

        static bool _OK(const Vector2& v0, const Vector2& v1, size_t rectA, size_t rectB, const vector<BoundingRectangle>& rects)
        {
            float maxDis = Vector2::Distance(v0, v1);
            Ray2D ray(v0, Vector2::Normalize(v1 - v0));

            float dis = 0.0f;
            for (size_t i = 0u; i < rects.size(); i++)
            {
                if (i == rectA || i == rectB) { continue; }

                if (ray.Intersects(rects[i], dis))
                {
                    if (dis < maxDis)
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        bool PathGrid::FindPath(size_t aObject, const Vector2& v1, vector<Vector2>& arPath) const
        {
            float const kFactor = (0.5f);

            BoundingRectangle objectRect = mObjects[aObject];
            float radius = objectRect.DiagonalLength() * kFactor;

            vector<BoundingRectangle> rects;
            for (size_t i = 0u; i < mObjects.size(); i++)
            {
                if (i == aObject) { continue; }

                const BoundingRectangle& rect = mObjects[i];

                rects.push_back(BoundingRectangle(
                    rect.Min + (Vector2::Normalize(rect.Min - rect.Center()) * radius),
                    rect.Max + (Vector2::Normalize(rect.Max - rect.Center()) * radius)));
            }

            vector<size_t> pointsToRects;
            vector<Vector2> points;
            vector<Vector2> normals;
            for (size_t i = 0u; i < rects.size(); i++)
            {
                const BoundingRectangle& rect = rects[i];

                pointsToRects.push_back(i);
                points.push_back(Vector2(rect.Min.X, rect.Min.Y));
                normals.push_back(Vector2::Normalize(Vector2(rect.Min.X, rect.Min.Y) - rect.Center()));

                pointsToRects.push_back(i);
                points.push_back(Vector2(rect.Min.X, rect.Max.Y));
                normals.push_back(Vector2::Normalize(Vector2(rect.Min.X, rect.Max.Y) - rect.Center()));

                pointsToRects.push_back(i);
                points.push_back(Vector2(rect.Max.X, rect.Min.Y));
                normals.push_back(Vector2::Normalize(Vector2(rect.Max.X, rect.Min.Y) - rect.Center()));

                pointsToRects.push_back(i);
                points.push_back(Vector2(rect.Max.X, rect.Max.Y));
                normals.push_back(Vector2::Normalize(Vector2(rect.Max.X, rect.Max.Y) - rect.Center()));
            }

            for (int i = 0u; i < (int)points.size(); i++)
            {
                for (size_t j = 0u; j < rects.size(); j++)
                {
                    if (pointsToRects[i] == j) { continue; }

                    if (rects[j].Intersects(points[i]))
                    {
                        points[i] = points[points.size() - 1u];
                        normals[i] = normals[normals.size() - 1u];
                        pointsToRects[i] = pointsToRects[pointsToRects.size() - 1u];

                        points.pop_back();
                        normals.pop_back();
                        pointsToRects.pop_back();

                        i--;
                        break;
                    }
                }
            }

            const size_t kAdjacency = 6u;
            vector<u16> adjacency(points.size() * kAdjacency); adjacency.assign(points.size() * kAdjacency, AStar<u16, kAdjacency>::kNullNode);

            for (size_t offset = 0u; offset < kAdjacency; offset++)
            {
                for (size_t i = 0u; i < points.size(); i++)
                {
                    if (adjacency[(i * kAdjacency) + offset] != AStar<u16, kAdjacency>::kNullNode)
                    {
                        continue;
                    }

                    int best = -1;
                    float bestDis = Constants<float>::kMax;

                    for (size_t j = (i + 1u); j < points.size(); j++)
                    {
                        if (adjacency[(j * kAdjacency) + (kAdjacency - 1u)] != AStar<u16, kAdjacency>::kNullNode)
                        {
                            continue;
                        }

                        if (offset > 0u)
                        {
                            bool bSkip = false;
                            for (size_t check = 0u; check < offset; check++)
                            {
                                if (adjacency[(i * kAdjacency) + check] == j) { bSkip = true; break; }
                            }

                            if (bSkip)
                            {
                                continue;
                            }
                        }
                        
                        if (Vector2::Dot(normals[i], normals[j]) < 0.0f)
                        {
                            float dis = Vector2::DistanceSquared(points[i], points[j]);
                            if (dis < bestDis)
                            {
                                float dot = Vector2::Dot(normals[i], Vector2::Normalize(points[j] - points[i]));
                                if (dot > 0.0f)
                                {
                                    if (_OK(points[i], points[j], pointsToRects[i], pointsToRects[j], rects))
                                    {
                                        best = j;
                                        bestDis = dis;
                                    }
                                }
                            }
                        }
                    }

                    if (best >= 0)
                    {
                        JZ_ASSERT((adjacency[(i * kAdjacency) + offset] == AStar<u16, kAdjacency>::kNullNode));
                        adjacency[(i * kAdjacency) + offset] = (u16)best;
                        
                        JZ_ASSERT((adjacency[(best * kAdjacency) + (kAdjacency - 1u)] == AStar<u16, kAdjacency>::kNullNode));
                        for (size_t j = 0u; j < kAdjacency; j++)
                        {
                            if (adjacency[(best * kAdjacency) + j] == AStar<u16, kAdjacency>::kNullNode)
                            {
                                adjacency[(best * kAdjacency) + j] = (u16)i;
                                break;
                            }
                        }
                    }
                }
            }

            for (int i = 0; i < (int)points.size(); i++)
            {
                const size_t kIndex = ((size_t)i * kAdjacency);

                if (adjacency[kIndex] == AStar<u16, kAdjacency>::kNullNode)
                {
                    const size_t kFrom = ((adjacency.size() - kAdjacency));

                    points[i] = points[points.size() - 1u];
                    normals[i] = normals[normals.size() - 1u];
                    pointsToRects[i] = pointsToRects[pointsToRects.size() - 1u];
                    
                    for (size_t j = 0u; j < kAdjacency; j++)
                    {
                        adjacency[kIndex + j] = adjacency[kFrom + j];

                        if (adjacency[kIndex + j] != AStar<u16, kAdjacency>::kNullNode)
                        {
                            const size_t kFixup = (adjacency[kIndex + j] * kAdjacency);

                            for (size_t k = 0u; k < kAdjacency; k++)
                            {
                                if (adjacency[kFixup + k] == (kFrom / kAdjacency))
                                {
                                    adjacency[kFixup + k] = i;
                                    break;
                                }
                            }
                        }
                    }

                    points.pop_back();
                    normals.pop_back();
                    pointsToRects.pop_back();
                    adjacency.resize(kFrom);

                    i--;
                }
            }

            BoundingRectangle rect = mObjects[aObject];
            Vector2 v0 = rect.Center();

            u16 i0 = 0u;
            u16 i0noCheck = 0u;
            u16 i1 = 0u;

            for (size_t i = 1u; i < points.size(); i++)
            {
                if (Vector2::DistanceSquared(v0, points[i0]) > Vector2::DistanceSquared(v0, points[i]))
                {
                    i0noCheck = i;
                    if (_OK(v0, points[i], pointsToRects[i], pointsToRects[i], rects))
                    {
                        i0 = i;
                    }
                }
            }

            if (i0 == 0u && i0noCheck != 0u) { i0 = i0noCheck; }

            for (size_t i = 1u; i < points.size(); i++)
            {
                if (Vector2::DistanceSquared(v1, points[i1]) > Vector2::DistanceSquared(v1, points[i]))
                {
                    if (_OK(v1, points[i], pointsToRects[i], pointsToRects[i], rects))
                    {
                        i1 = i;
                    }
                }
            }

            arPath.clear();

            if (adjacency.size() > 0u)
            {
                vector<u16> p;
                if (AStar<u16, kAdjacency>::FindPath(i0, i1, adjacency, Dist, Dist, p, &points))
                {
                    arPath.resize(p.size());
                    const size_t kSize = arPath.size();
                    for (size_t i = 0u; i < kSize; i++)
                    {
                        arPath[i] = points[p[i]];
                    }

                    return true;
                }
            }
            else
            {
                arPath.resize(2u);
                arPath[0u] = mObjects[aObject].Center();
                arPath[1u] = v1;
                return true;
            }

            return false;
        }

    }
}
