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

#include <jz_core/Segment.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics/VertexDeclaration.h>
#include <jz_gi/RadiosityMan.h>
#include <jz_system/Files.h>
#include <algorithm>

namespace jz
{
    gi::RadiosityMan* gi::RadiosityMan::mspSingleton = null;
    namespace gi
    {

        static const char* kEffectFilename = "radiosity.cfx";
        static const char* kRenderTechnique = "jz_Render";
        static const char* kVertexDeclarationFilename = "built-in_-1161387004.vdl";

        RadiosityMan::RadiosityMan()
            : mbDirty(false)
        {
            graphics::Graphics& graphics = graphics::Graphics::GetSingleton();

            mpEffect = graphics.Create<graphics::Effect>(kEffectFilename);
            JZ_E_ON_FAIL(mpEffect->IsLoadable(), "is not loadable.");

            mRenderTechnique = mpEffect->GetTechniqueByName(kRenderTechnique);
            JZ_E_ON_FAIL(mRenderTechnique.IsValid(), "is not valid.");

            mpVertexDeclaration = graphics.Create<graphics::VertexDeclaration>(kVertexDeclarationFilename);
            JZ_E_ON_FAIL(mpVertexDeclaration->IsLoadable(), "is not loadable.");
        }

        RadiosityMan::~RadiosityMan()
        {}

        void RadiosityMan::Draw() const
        {
            if (mbDirty)
            {
                const_cast<RadiosityMan*>(this)->_Update();
            }

            {
                mpVertexDeclaration->SetToDevice();
                mpEffect->SetTechnique(mRenderTechnique);
                graphics::Graphics& graphics = graphics::Graphics::GetSingleton();
                graphics::Pass pass = mpEffect->Begin();
                if (pass.IsValid())
                {
                    while (pass.IsValid())
                    {
                        if (pass.Begin())
                        {
                            graphics.DrawIndexedPrimitives(graphics::PrimitiveType::kTriangleList, mTree.GetIndices().Get(), (mTree.GetIndices().size() / 3u), mDrawBuffer.Get(), (mDrawBuffer.size() / 2u), sizeof(Vector3) * 2u);
                            pass.End();
                        }
                        
                        pass = pass.Next();
                    }
                    mpEffect->End();
                }
            }
        }

        static const size_t kIterations = 10u;
        static const size_t kMaxVertices = 1000u;
        
        void RadiosityMan::_Update()
        {
            const MemoryBuffer<Vector3>& vertices = (mTree.GetVertices());
            const size_t vSize = vertices.size();

            const MemoryBuffer<u16>& indices = (mTree.GetIndices());
            const size_t iSize = indices.size();        

            mRadiances.resize(vSize);
            mRadiances.Initialize();

            MemoryBuffer<ColorRGB> emission(vSize);
            emission.Initialize();

            for (Container::iterator I = mInitialRadiances.begin(); I != mInitialRadiances.end(); I++)
            {
                mRadiances[I->first] = I->second;
                emission[I->first] = I->second;
            }

            #pragma region Normals
            typedef MemoryBuffer<Vector3> Normals;
            Normals normals(mRadiances.size());
            normals.Initialize();
            {
                for (size_t i = 0u; i < iSize; i += 3u)
                {
                    u16 vi0 = indices[i + 0u];
                    u16 vi1 = indices[i + 1u];
                    u16 vi2 = indices[i + 2u];

                    const Vector3& p0 = vertices[vi0];
                    const Vector3& p1 = vertices[vi1];
                    const Vector3& p2 = vertices[vi2];

                    Vector3 unormNormal = Vector3::Cross(p2 - p0, p1 - p0);
                    normals[vi0] += unormNormal;
                    normals[vi1] += unormNormal;
                    normals[vi2] += unormNormal;
                }

                for (size_t i = 0u; i < vSize; i++)
                {
                    normals[i] = Vector3::Normalize(normals[i]);
                }
            }
            #pragma endregion

            #pragma region Links
            typedef pair<u16, u16> Link;
            typedef vector<Link> Links;
            Links links;
            {
                const size_t itr = Min(vSize, kMaxVertices);
                for (size_t i = 0u; i < itr; i++)
                {
                    for (size_t j = (i + 1); j < itr; j++)
                    {
                        float di = Vector3::Dot(normals[i], vertices[j] - vertices[i]);
                        float dj = Vector3::Dot(normals[j], vertices[i] - vertices[j]);
                        if (di > Constants<float>::kLooseTolerance || dj > Constants<float>::kLooseTolerance)
                        {
                            if (_Visible(i, j))
                            {
                                if (di > 0.0f) { links.push_back(make_pair(i, j)); }
                                if (dj > 0.0f) { links.push_back(make_pair(j, i)); }
                            }
                        }
                    }
                }
            }
            sort(links.begin(), links.end());
            #pragma endregion

            #pragma region Calculate radiosity
            JZ_ASSERT(vSize <= Constants<u16>::kMax);

            for (size_t i = 0u; i < kIterations; i++)
            {
                u16 lastFirst = Constants<u16>::kMax;

                for (Links::iterator I = links.begin(); I != links.end(); I++)
                {
                    if (lastFirst != I->first && lastFirst < vSize)
                    {
                        emission[lastFirst] = ColorRGB::kBlack;
                        lastFirst = I->first;
                    }

                    if (emission[I->first].LengthSquared() > Constants<float>::kZeroTolerance)
                    {
                        const Vector3& p0 = vertices[I->second];
                        const Vector3& p1 = vertices[I->first];

                        Vector3 v = (p1 - p0);
                        float d2 = v.LengthSquared();
                        if (d2 > Constants<float>::kZeroTolerance)
                        {
                            v /= Sqrt(d2);

                            float ndotl = Max(Vector3::Dot(v, normals[I->second]) * Vector3::Dot(-v, normals[I->first]), 0.0f);
                            float att = 1.0f / (1.0f + d2);

                            ColorRGB c = (att * ndotl * emission[I->first]);
                            mRadiances[I->second] += c;
                            emission[I->second] += c;
                        }
                    }
                }

                if (lastFirst < vSize)
                {
                    emission[lastFirst] = ColorRGB::kBlack;
                }
            }
            #pragma endregion

            #pragma region Populate draw buffer
            mDrawBuffer.resize(vSize * 2u);
            for (size_t i = 0u; i < vSize; i++)
            {
                const size_t kIndex = (i * 2u);
                mDrawBuffer[kIndex + 0u] = vertices[i];
                mDrawBuffer[kIndex + 1u] = Vector3(mRadiances[i].R, mRadiances[i].G, mRadiances[i].B);
            }
            #pragma endregion

            mbDirty = false;
        }

        bool RadiosityMan::_Visible(u16 i, u16 j) const
        {
            const MemoryBuffer<Vector3>& vertices = (mTree.GetVertices());
            const size_t vSize = vertices.size();

            const MemoryBuffer<u16>& indices = (mTree.GetIndices());
            const size_t iSize = indices.size();

            const Vector3& p0 = vertices[i];
            const Vector3& p1 = vertices[j];

            Segment segment(Segment::Create(p0, p1));
            BoundingBox aabb(Vector3::Min(p0, p1), Vector3::Max(p0, p1));

            const vector<system::TriangleTree::Node>& nodes = mTree.GetNodes();
            const size_t nSize = nodes.size();

            for (size_t i = 0u; i < nSize; )
            {
                Axis::Id axis = nodes[i].GetAxis();
                float pos = nodes[i].GetSplitPos();

                if ((nodes[i].IsFront() && (pos <= aabb.Max[axis])) || (nodes[i].IsBack() && (pos >= aabb.Min[axis])))
                {
                    if (nodes[i].IsLeaf())
                    {
                        u32 tIndex = (nodes[i].TriangleIndex * 3u);
                        
                        u16 iv0 = indices[tIndex + 0u];
                        u16 iv1 = indices[tIndex + 1u];
                        u16 iv2 = indices[tIndex + 2u];

                        if (i != iv0 && i != iv1 && i != iv2 &&
                            j != iv0 && j != iv1 && j != iv2)
                        {
                            Triangle3D triangle(vertices[iv0], vertices[iv1], vertices[iv2]);

                            float t;
                            if (segment.Intersects(triangle, t))
                            {
                                return false;
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

            return true;
        }

    }
}
