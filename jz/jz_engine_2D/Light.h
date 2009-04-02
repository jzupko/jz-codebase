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
#ifndef _JZ_ENGINE_2D_LIGHT_H_
#define _JZ_ENGINE_2D_LIGHT_H_

#include <jz_core/Auto.h>
#include <jz_core/Color.h>
#include <jz_core/Matrix3.h>
#include <jz_core/Vector4.h>
#include <jz_engine_2D/IDrawable.h>
#include <jz_engine_2D/ISerializeable.h>
#include <vector>

namespace jz
{
    namespace graphics
    {
        class Material; typedef AutoPtr<Material> MaterialPtr;
    }

    namespace system
    {
        class IWriteFile; typedef AutoPtr<IWriteFile> IWriteFilePtr;
        class IReadFile; typedef AutoPtr<IReadFile> IReadFilePtr;
    }

    namespace engine_2D
    {

        class Light : public IDrawable, public ISerializeable
        {
        public:
            Light();
            virtual ~Light();

            enum Type
            {
                kDirectional = 0,
                kPoint = 1
            };

            enum
            {
                kNone = 0,
                kShadow = (1 << 0),
                kSerializeable = (1 << 1)
            };

            virtual BoundingRectangle GetBounding() const override { return mBounding; }
            virtual float GetZdepth() const override { return (mZ); }

            virtual void Pose(IDrawable* apDrawable) override;

            virtual bool bSerializeable() const override { return ((mFlags & kSerializeable) != 0); }
            virtual void Serialize(system::IWriteFilePtr& p) override;
            virtual void Deserialize(system::IReadFilePtr& p) override;

            const ColorRGB& GetColor() const { return mColor; }
            void SetColor(const ColorRGB& c);

            const Vector3& GetDirection() const { return mDirection; }
            void SetDirection(const Vector3& v) { mDirection = v; }

            Vector3 GetPosition() const
            {
                Vector3 ret = Vector3(mBounding.Center(), mZ);

                return ret;
            }

            void SetPosition(float x, float y) { SetPosition(Vector3(x, y, mZ)); }
            void SetPosition(float x, float y, float z) { SetPosition(Vector3(x, y, z)); }
            void SetPosition(const Vector3& v) { _Update(v); }

            float GetRadius() const { return mRadius; }

            const vector<Vector3>& GetShadowVertices() const { return mShadowVertices; }

            Type GetType() const { return mType; }
            void SetType(Type v) { mType = v; }

            bool bShadow() const { return ((mFlags & kShadow) != 0); }
            void SetShadow(bool b)
            {
                if (b) { mFlags |= kShadow; }
                else { mFlags &= ~kShadow; }

                if (!b) { mShadowVertices.clear(); }
            }

            void SetSerializeable(bool b)
            {
                if (b) { mFlags |= kSerializeable; }
                else { mFlags &= ~kSerializeable; }
            }

        protected:
            friend void ::jz::__IncrementRefCount<Light>(Light*);
            friend void ::jz::__DecrementRefCount<Light>(Light*);

            void _Update(const Vector3& v);

            BoundingRectangle mBounding;
            ColorRGB mColor;
            Vector3 mDirection;
            u32 mFlags;
            float mRadius;
            float mZ;
            Type mType;

            vector<Vector3> mShadowVertices;

        private:
            Light(const Light&);
            Light& operator=(const Light&);

        };

        typedef AutoPtr<Light> LightPtr;

    }
}

#endif
