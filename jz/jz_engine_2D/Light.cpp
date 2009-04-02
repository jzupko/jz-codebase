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

#include <jz_core/BoundingCircle.h>
#include <jz_engine_2D/Cell.h>
#include <jz_engine_2D/Light.h>
#include <jz_engine_2D/Sprite.h>
#include <jz_system/Files.h>
#include <jz_system/ReadHelpers.h>
#include <jz_system/Time.h>
#include <jz_system/WriteHelpers.h>

namespace jz
{
    namespace engine_2D
    {

        static const float kMinLuminance = (float)(1.0 / 255.0);
        static float CalculateLightRange(const ColorRGB& v)
        {
            float val = v.Max();
            float ret = Sqrt(val / kMinLuminance);

            return ret;
        }

        Light::Light()
            : IDrawable(IDrawable::kLight),
              mBounding(BoundingRectangle::kZero),
              mColor(ColorRGB::kBlack),
              mDirection(Vector3::kNegUnitZ),
              mFlags(kNone),
              mRadius(0.0f),
              mType(kPoint),
              mZ(0.0f)
        {}

        Light::~Light()
        {}

        void Light::SetColor(const ColorRGB& v)
        {
            mColor = v;
            mRadius = CalculateLightRange(mColor);

            _Update(Vector3(mBounding.Center(), mZ));
        }

        static const float kOffset = (float)(1.0 / 255.0);
        static const size_t kEntriesPerPrimitive = 6u;

        static void _ProcessPoint(const Vector3& u, const Vector3 v, Light* apLight, vector<Vector3>& arBuffer)
        {
            if (apLight->GetType() == Light::kDirectional)
            {
            }
            else
            {
                float radius = (apLight->GetRadius());
                Vector3 pos = (apLight->GetPosition());

                if (pos.Z < u.Z && pos.Z < v.Z)
                {
                    Vector3 diffU = (u - pos);
                    Vector3 diffV = (v - pos);
                    float lenU = diffU.Length();
                    float lenV = diffV.Length();

                    if (lenU > Constants<float>::kZeroTolerance && lenU < radius && 
                        lenV > Constants<float>::kZeroTolerance && lenV < radius)
                    {
                        Vector3 u1 = (pos + (diffU * (radius / lenU)));
                        Vector3 v1 = (pos + (diffV * (radius / lenV)));

                        arBuffer.push_back(u); arBuffer.push_back(Vector3::kZero);
                        arBuffer.push_back(u1); arBuffer.push_back(Vector3::kZero);
                        arBuffer.push_back(v1); arBuffer.push_back(Vector3::kZero);
                        arBuffer.push_back(u); arBuffer.push_back(Vector3::kZero);
                        arBuffer.push_back(v1); arBuffer.push_back(Vector3::kZero);
                        arBuffer.push_back(v); arBuffer.push_back(Vector3::kZero);
                    }
                }
            }
        }

        static void _Process(IDrawable* apDrawable, Light* apLight)
        {
            Cell* pCell = dynamic_cast<Cell*>(apDrawable);
            if (pCell)
            {
                BoundingCircle circle(Vector2(apLight->GetPosition().X, apLight->GetPosition().Y), apLight->GetRadius());
                for (vector<IDrawablePtr>::iterator I = pCell->begin(); I != pCell->end(); I++)
                {
                    if ((*I)->GetDrawableType() == IDrawable::kSprite)
                    {
                        Sprite* p = static_cast<Sprite*>(I->Get());
                        BoundingRectangle rect = p->GetBounding();
                        if (rect.Intersects(circle))
                        {
                            p->Pose(apLight);
                        }
                    }
                }
            }
        }

        static void _Process(IDrawable* apDrawable, Light* apLight, vector<Vector3>& arBuffer)
        {
            Cell* pCell = dynamic_cast<Cell*>(apDrawable);
            if (pCell)
            {
                BoundingCircle circle(Vector2(apLight->GetPosition().X, apLight->GetPosition().Y), apLight->GetRadius());

                arBuffer.clear();
                for (vector<IDrawablePtr>::iterator I = pCell->begin(); I != pCell->end(); I++)
                {
                    if ((*I)->GetZdepth() < apLight->GetZdepth()) { continue; }

                    if ((*I)->GetDrawableType() == IDrawable::kSprite)
                    {
                        Sprite* p = static_cast<Sprite*>(I->Get());

                        BoundingRectangle rect = p->GetBounding();
                        if (rect.Intersects(circle))
                        {
                            p->Pose(apLight);
                            if (!(p->GetAlpha() < 1.0f))
                            {
                                MemoryBuffer<Vector2>* pEdges = p->GetShadowEdges();
                                if (pEdges)
                                {
                                    const Vector2& scale = (p->GetScale());
                                    const Vector2& trans = (p->GetTranslation());
                                    const Vector2& xydf = (p->GetXYDepthFactors());

                                    float z = (p->GetZdepth() + kOffset);
                                    const size_t kSize = pEdges->size();
                                    for (size_t i = 0u; i < (kSize - 1u); i += 2u)
                                    {
                                        Vector2 a0 = ((*pEdges)[i+0] * scale) + trans;
                                        Vector2 a1 = ((*pEdges)[i+1] * scale) + trans;

                                        Vector3 p0 = Vector3(a0, z + Vector2::Dot(xydf, a0));
                                        Vector3 p1 = Vector3(a1, z + Vector2::Dot(xydf, a1));

                                        _ProcessPoint(p0, p1, apLight, arBuffer);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        void Light::Pose(IDrawable* apDrawable)
        {
            if (mRadius > Constants<float>::kZeroTolerance)
            {
                if (bShadow())
                {
                    _Process(apDrawable, this, mShadowVertices);
                }
                else
                {
                    _Process(apDrawable, this);
                }
            }
        }

        void Light::Serialize(system::IWriteFilePtr& p)
        {
            JZ_ASSERT(false);
        }

        void Light::Deserialize(system::IReadFilePtr& p)
        {
            JZ_ASSERT(false);
        }

        void Light::_Update(const Vector3& v)
        {
            mZ = v.Z;

            if (mType == kDirectional) { mBounding = BoundingRectangle::kNegOneToOne; }
            else
            {
                if (mZ > 1.0f) { mBounding = BoundingRectangle::kZero; }
                else
                {
                    Vector2 center = Vector2(v.X, v.Y);
                    float radius = mRadius;

                    if (mZ < 0.0f)
                    {
                        radius = Sqrt((mRadius * mRadius) - (mZ * mZ));
                    }

                    mBounding = BoundingRectangle(
                        center - Vector2(radius),
                        center + Vector2(radius));
                }
            }
        }

    }
}
