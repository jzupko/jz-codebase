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
#ifndef _JZ_QUATERNION_H_
#define _JZ_QUATERNION_H_

#include <jz_core/Angle.h>
#include <jz_core/Vector3.h>
#include <type_traits>

namespace jz
{
    struct Quaternion;

    Quaternion operator*(float s, const Quaternion& q);

    struct Quaternion
    {
        static const int N = 4;

        union
        {
            struct
            {
                float X, Y, Z, W;
            };
            float pData[N];
        };

        Quaternion()
            : X(0), Y(0), Z(0), W(1)
        {}

        Quaternion(float x, float y, float z, float w)
            : X(x), Y(y), Z(z), W(w)
        {}

        Quaternion(const Quaternion& q)
            : X(q.X), Y(q.Y), Z(q.Z), W(q.W)
        {}

        Quaternion(const Vector3& aAxis, Radian aAngle)
        {
            SetFromAxisAngle(aAxis, aAngle);
        }

        Quaternion(Radian aRoll, Radian aPitch, Radian aYaw)
        {
            SetFromEulerAngles(aRoll, aPitch, aYaw);
        }
        
        Quaternion& operator=(const Quaternion& q)
        {
            X = q.X;
            Y = q.Y;
            Z = q.Z;
            W = q.W;

            return *this;
        }

        Quaternion operator+(const Quaternion& q) const
        {
            return Quaternion(X + q.X, Y + q.Y, Z + q.Z, W + q.W);
        }

        Quaternion& operator+=(const Quaternion& q)
        {
            X += q.X;
            Y += q.Y;
            Z += q.Z;
            W += q.W;

            return *this;
        }

        Quaternion operator*(float s) const
        {
            return Quaternion(X * s, Y * s, Z * s, W * s);
        }

        Quaternion& operator*=(float s)
        {
            X *= s;
            Y *= s;
            Z *= s;
            W *= s;

            return *this;
        }

        Quaternion operator/(float s) const
        {
            return Quaternion(X / s, Y / s, Z / s, W / s);
        }

        Quaternion& operator/=(float s)
        {
            X /= s;
            Y /= s;
            Z /= s;
            W /= s;

            return *this;
        }

        Quaternion operator*(const Quaternion& q) const
        {
            return Quaternion((W * q.X) + (X * q.W) + (Y * q.Z) - (Z * q.Y),
                              (W * q.Y) + (Y * q.W) + (Z * q.X) - (X * q.Z),
                              (W * q.Z) + (Z * q.W) + (X * q.Y) - (Y * q.X),
                              (W * q.W) - (X * q.X) - (Y * q.Y) - (Z * q.Z));
        }

        Quaternion operator-() const
        {
            return Quaternion(-X, -Y, -Z, -W);
        }

        Quaternion operator-(const Quaternion& q) const
        {
            return Quaternion(X - q.X, Y - q.Y, Z - q.Z, W - q.W);
        }

        Quaternion& operator-=(const Quaternion& q)
        {
            X -= q.X;
            Y -= q.Y;
            Z -= q.Z;
            W -= q.W;

            return *this;
        }

        float Length() const
        {
            return Sqrt(LengthSquared());
        }

        float LengthSquared() const
        {
            return Dot(*this, *this);
        }

        void SetFromAxisAngle(const Vector3& aAxis, const Radian& aAngle)
        {
            Radian halfAngle = Radian(0.5f * aAngle.GetValue());
            float   sin      = Sin(halfAngle);

            X = sin * aAxis.X;
            Y = sin * aAxis.Y;
            Z = sin * aAxis.Z;
            W = Cos(halfAngle);
        }

        void SetFromEulerAngles(Radian aRoll, Radian aPitch, Radian aYaw)
        {
            *this = Quaternion(Vector3::kForward, aRoll) * Quaternion(Vector3::kRight, aPitch) * Quaternion(Vector3::kUp, aYaw);
        }

        void ToAxisAngle(Vector3& arAxis, Radian& arAngle) const
        {
            float lenSq = LengthSquared();

            if (GreaterThan(lenSq, 0.0f))
            {
                arAngle = 2.0f * ACos(W);
                arAxis = Vector3::Normalize(Vector3(X, Y, Z));
            }
            else
            {
                arAngle = Radian::kZero;
                arAxis  = Vector3::kForward;
            }
        }

        static bool AboutEqual(const Quaternion& q0, const Quaternion& q1, float aTolerance = Constants<float>::kZeroTolerance)
        {
            return (jz::AboutEqual(q0.X, q1.X, aTolerance) && 
                    jz::AboutEqual(q0.Y, q1.Y, aTolerance) && 
                    jz::AboutEqual(q0.Z, q1.Z, aTolerance) &&
                    jz::AboutEqual(q0.W, q1.W, aTolerance));
        }

        static float Dot(const Quaternion& q0, const Quaternion& q1)
        {
            return (q0.X * q1.X) + (q0.Y * q1.Y) + (q0.Z * q1.Z) + (q0.W * q1.W);
        }

        static Quaternion Invert(const Quaternion& q0)
        {
            Quaternion ret;
            float lenSq = q0.LengthSquared();

            if (GreaterThan(lenSq, 0.0f))
            {
                float invLenSq = (1.0f / lenSq);

                ret.X = -q0.X * invLenSq;
                ret.Y = -q0.Y * invLenSq;
                ret.Z = -q0.Z * invLenSq;
                ret.W =  q0.W * invLenSq;
            }
            else { ret = kInvalid; }

            return ret;
        }

        static Quaternion Nlerp(const Quaternion& q0, const Quaternion& q1, float aWeightOfB)
        {
            Quaternion ret(q0);
            
            if (jz::LessThan(Dot(q0, q1), 0.0f))
            {
                ret += (aWeightOfB * (-q1 - ret));
            }
            else
            {
                ret += (aWeightOfB * (q1 - ret));
            }

            return Normalize(ret);
        }

        static Quaternion Normalize(const Quaternion& q0)
        {
            Quaternion ret(q0);
            float len = q0.Length();

            if (len > jz::Constants<float>::kZeroTolerance) { ret /= len; }

            return ret;
        }

        static Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float aWeightOfB)
        {
            float dot = Dot(q0, q1);

            if (jz::AboutEqual(dot,  1.0f, Constants<float>::kSlerpThreshold) ||
                jz::AboutEqual(dot, -1.0f, Constants<float>::kSlerpThreshold))
            {
                return Nlerp(q0, q1, aWeightOfB);
            }
            else
            {
                dot = jz::Clamp(dot, -1.0f, 1.0f);

                Radian theta0 = ACos(dot);
                Radian theta  = theta0 * aWeightOfB;
                
                Quaternion ret = Normalize(q1 - (q0 * dot));
                ret = (q0 * Cos(theta)) + (ret * Sin(theta));

                return ret;
            }
        }

        static const Quaternion kIdentity;
        static const Quaternion kInvalid;

        static Quaternion CreateFromAxisAngle(const Vector3& aAxis, Radian aAngle);
    };

    inline Quaternion operator*(float s, const Quaternion& q)
    {
        return (q * s);
    }

}

#endif
