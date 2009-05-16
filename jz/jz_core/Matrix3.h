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
#ifndef _JZ_MATRIX3_H_
#define _JZ_MATRIX3_H_

#include <jz_core/Math.h>
#include <jz_core/Vector3.h>
#include <type_traits>

namespace jz
{

    struct Matrix2;
    struct Matrix4;
    struct Matrix3
    {
        static const int S = 3;
        static const int N = S * S;

        union
        {
            // colmn major storage
            struct
            {
                float M11, M21, M31;
                float M12, M22, M32;
                float M13, M23, M33;
            };

            float pData[N];
        };

        Matrix3()
            : M11(0), M12(0), M13(0),
              M21(0), M22(0), M23(0),
              M31(0), M32(0), M33(0)
        {}

        explicit Matrix3(float s)
            : M11(s), M12(s), M13(s),
              M21(s), M22(s), M23(s),
              M31(s), M32(s), M33(s)
        {}

        Matrix3(float m11, float m12, float m13,
                float m21, float m22, float m23,
                float m31, float m32, float m33)
            : M11(m11), M12(m12), M13(m13),
              M21(m21), M22(m22), M23(m23),
              M31(m31), M32(m32), M33(m33)
        {}

        Matrix3(const Matrix3& m)
            : M11(m.M11), M12(m.M12), M13(m.M13),
              M21(m.M21), M22(m.M22), M23(m.M23),
              M31(m.M31), M32(m.M32), M33(m.M33)
        {}

        Matrix3& operator=(const Matrix3& m)
        {
            M11 = m.M11; M12 = m.M12; M13 = m.M13;
            M21 = m.M21; M22 = m.M22; M23 = m.M23;
            M31 = m.M31; M32 = m.M32; M33 = m.M33;

            return *this;
        }

        float operator()(int aRow, int aColumn) const
        {
            return pData[I(aRow, aColumn)];
        }

        float& operator()(int aRow, int aColumn)
        {
            return pData[I(aRow, aColumn)];
        }

        bool operator==(const Matrix3& m) const
        {
            return (M11 == m.M11 && M12 == m.M12 && M13 == m.M13) &&
                   (M21 == m.M21 && M22 == m.M22 && M23 == m.M23) &&
                   (M31 == m.M31 && M32 == m.M32 && M33 == m.M33);
        }

        bool operator!=(const Matrix3& m) const
        {
            return !(*this == m);
        }

        Matrix3 operator+(const Matrix3& m) const
        {
            return Matrix3(M11 + m.M11, M12 + m.M12, M13 + m.M13,
                           M21 + m.M21, M22 + m.M22, M23 + m.M23,
                           M31 + m.M31, M32 + m.M32, M33 + m.M33);
        }

        Matrix3& operator+=(const Matrix3& m)
        {
            M11 += m.M11; M12 += m.M12; M13 += m.M13;
            M21 += m.M21; M22 += m.M22; M23 += m.M23;
            M31 += m.M31; M32 += m.M32; M33 += m.M33;

            return *this;
        }

        Matrix3 operator/(float s) const
        {
            return Matrix3(M11 / s, M12 / s, M13 / s,
                           M21 / s, M22 / s, M23 / s,
                           M31 / s, M32 / s, M33 / s);
        }

        Matrix3& operator/=(float s)
        {
            M11 /= s; M12 /= s; M13 /= s;
            M21 /= s; M22 /= s; M23 /= s;
            M31 /= s; M32 /= s; M33 /= s;

            return *this;
        }

        Matrix3 operator*(float s) const
        {
            return Matrix3(M11 * s, M12 * s, M13 * s,
                           M21 * s, M22 * s, M23 * s,
                           M31 * s, M32 * s, M33 * s);
        }

        Matrix3& operator*=(float s)
        {
            M11 *= s; M12 *= s; M13 *= s;
            M21 *= s; M22 *= s; M23 *= s;
            M31 *= s; M32 *= s; M33 *= s;

            return *this;
        }

        Matrix3 operator*(const Matrix3& m) const
        {
            // column 1
            const float m11 = (M11 * m.M11) + (M12 * m.M21) + (M13 * m.M31);
            const float m21 = (M21 * m.M11) + (M22 * m.M21) + (M23 * m.M31);
            const float m31 = (M31 * m.M11) + (M32 * m.M21) + (M33 * m.M31);

            // column 2
            const float m12 = (M11 * m.M12) + (M12 * m.M22) + (M13 * m.M32);
            const float m22 = (M21 * m.M12) + (M22 * m.M22) + (M23 * m.M32);
            const float m32 = (M31 * m.M12) + (M32 * m.M22) + (M33 * m.M32);

            // column 3
            const float m13 = (M11 * m.M13) + (M12 * m.M23) + (M13 * m.M33);
            const float m23 = (M21 * m.M13) + (M22 * m.M23) + (M23 * m.M33);
            const float m33 = (M31 * m.M13) + (M32 * m.M23) + (M33 * m.M33);

            return Matrix3(m11, m12, m13,
                           m21, m22, m23,
                           m31, m32, m33);
        }

        Matrix3 operator-() const
        {
            return Matrix3(-M11, -M12, -M13,
                           -M21, -M22, -M23,
                           -M31, -M32, -M33);
        }

        Matrix3 operator-(const Matrix3& m) const
        {
            return Matrix3(M11 - m.M11, M12 - m.M12, M13 - m.M13,
                           M21 - m.M21, M22 - m.M22, M23 - m.M23,
                           M31 - m.M31, M32 - m.M32, M33 - m.M33);
        }

        Matrix3& operator-=(const Matrix3& m)
        {
            M11 -= m.M11; M12 -= m.M12; M13 -= m.M13;
            M21 -= m.M21; M22 -= m.M22; M23 -= m.M23;
            M31 -= m.M31; M32 -= m.M32; M33 -= m.M33;

            return *this;
        }  

        Vector3 GetCol(natural i) const
        {
            const Matrix3& m = *this;

            return Vector3(m(0, i), m(1, i), m(2, i));
        }

        Vector3 GetRow(natural i) const
        {
            const Matrix3& m = *this;
            
            return Vector3(m(i, 0), m(i, 1), m(i, 2));
        }

		bool IsReflection() const
		{
            bool bReturn = (Determinant() < 0.0f);

			return bReturn;
		}

        float Determinant() const
		{
            float ret = ((M11 * M22 * M33) + (M12 * M23 * M31) + (M13 * M21 * M32)) -
                        ((M31 * M22 * M13) + (M32 * M23 * M11) + (M33 * M21 * M12));			

			return ret;
		}

        float Sum() const
        {
            return (M11 + M12 + M13 +
                    M21 + M22 + M23 +
                    M31 + M32 + M33);
        }

        float Trace() const
        {
            return (M11 + M22 + M33);
        }

        Matrix2 GetOrientation() const;
        void SetOrientation(const Matrix2& m);

        Vector2 GetTranslation() const
        {
            return Vector2(M31, M32);
        }

        void SetTranslation(const Vector2& v)
        {
            M31 = v.X; M32 = v.Y;
        }

        static bool AboutEqual(const Matrix3& a, const Matrix3& b, float aTolerance = Constants<float>::kZeroTolerance)
        {
            return jz::AboutEqual(a.M11, b.M11, aTolerance) && jz::AboutEqual(a.M12, b.M12, aTolerance) && jz::AboutEqual(a.M13, b.M13, aTolerance) &&
                   jz::AboutEqual(a.M21, b.M21, aTolerance) && jz::AboutEqual(a.M22, b.M22, aTolerance) && jz::AboutEqual(a.M23, b.M23, aTolerance) &&
                   jz::AboutEqual(a.M31, b.M31, aTolerance) && jz::AboutEqual(a.M32, b.M32, aTolerance) && jz::AboutEqual(a.M33, b.M33, aTolerance);
        }

        static Matrix3 Abs(const Matrix3& a)
        {
            return Matrix3(jz::Abs(a.M11), jz::Abs(a.M12), jz::Abs(a.M13),
                           jz::Abs(a.M21), jz::Abs(a.M22), jz::Abs(a.M23),
                           jz::Abs(a.M31), jz::Abs(a.M32), jz::Abs(a.M33));
        }

        static Matrix3 Clamp(const Matrix3& a, const Matrix3& aMin, const Matrix3& aMax)
        {
            return Matrix3(jz::Clamp(a.M11, aMin.M11, aMax.M11), jz::Clamp(a.M12, aMin.M12, aMax.M12), jz::Clamp(a.M13, aMin.M13, aMax.M13),
                           jz::Clamp(a.M21, aMin.M21, aMax.M21), jz::Clamp(a.M22, aMin.M22, aMax.M22), jz::Clamp(a.M23, aMin.M23, aMax.M23),
                           jz::Clamp(a.M31, aMin.M31, aMax.M31), jz::Clamp(a.M32, aMin.M32, aMax.M32), jz::Clamp(a.M33, aMin.M33, aMax.M33));
        }

        static Matrix3 Invert(const Matrix3& m)
        {
            float n11 = m.M11; float n12 = m.M12; float n13 = m.M13;
            float n21 = m.M21; float n22 = m.M22; float n23 = m.M23;
            float n31 = m.M31; float n32 = m.M32; float n33 = m.M33;

            // entries in new matrix p to be multiplied by (1 / |M|)
            float p11 = (n22 * n33) - (n23 * n32);
            float p21 = (n23 * n31) - (n21 * n33);
            float p31 = (n21 * n32) - (n22 * n31);

            // 1 / |M|
            float t = 1.0f / ((n11 * p11) + (n12 * p21) + (n13 * p31));

            Matrix3 ret;
            ret.M11 = p11 * t; ret.M12 = ((n13 * n32) - (n12 * n33)) * t; ret.M13 = ((n12 * n23) - (n13 * n22)) * t;
            ret.M21 = p21 * t; ret.M22 = ((n11 * n33) - (n13 * n31)) * t; ret.M23 = ((n13 * n21) - (n11 * n23)) * t;
            ret.M31 = p31 * t; ret.M32 = ((n12 * n31) - (n11 * n32)) * t; ret.M33 = ((n11 * n22) - (n12 * n21)) * t;

            return ret;
        }

        static Matrix3 Lerp(const Matrix3& a, const Matrix3& b, float aWeightOfB)
        {   
            return Matrix3(jz::Lerp(a.M11, b.M11, aWeightOfB), jz::Lerp(a.M12, b.M12, aWeightOfB), jz::Lerp(a.M13, b.M13, aWeightOfB),
                           jz::Lerp(a.M21, b.M21, aWeightOfB), jz::Lerp(a.M22, b.M22, aWeightOfB), jz::Lerp(a.M23, b.M23, aWeightOfB),
                           jz::Lerp(a.M31, b.M31, aWeightOfB), jz::Lerp(a.M32, b.M32, aWeightOfB), jz::Lerp(a.M33, b.M33, aWeightOfB));
        }

        static Matrix3 Min(const Matrix3& a, const Matrix3& b)
        {   
            return Matrix3(jz::Min(a.M11, b.M11), jz::Min(a.M12, b.M12), jz::Min(a.M13, b.M13),
                           jz::Min(a.M21, b.M21), jz::Min(a.M22, b.M22), jz::Min(a.M23, b.M23),
                           jz::Min(a.M31, b.M31), jz::Min(a.M32, b.M32), jz::Min(a.M33, b.M33));
        }

        static Matrix3 Max(const Matrix3& a, const Matrix3& b)
        {   
            return Matrix3(jz::Max(a.M11, b.M11), jz::Max(a.M12, b.M12), jz::Max(a.M13, b.M13),
                           jz::Max(a.M21, b.M21), jz::Max(a.M22, b.M22), jz::Max(a.M23, b.M23),
                           jz::Max(a.M31, b.M31), jz::Max(a.M32, b.M32), jz::Max(a.M33, b.M33));
        }

        static Matrix3 Transpose(const Matrix3& a)
        {
            return Matrix3(a.M11, a.M21, a.M31,
                           a.M12, a.M22, a.M32,
                           a.M13, a.M23, a.M33);
        }

        static void FindEigenspace(const Matrix3& M, Vector3& arValues, Matrix3& arVectors);

        static const Matrix3 kIdentity;
        static const Matrix3 kZero;

        static Matrix3 CreateFromUpperLeft(const Matrix4& m);

        static Matrix3 CreateScale(float s);
        static Matrix3 CreateScale(float sx, float sy);
        static Matrix3 CreateScale(const Vector2& s);
        static Matrix3 CreateTranslation(float tx, float ty);
        static Matrix3 CreateTranslation(const Vector2& t);

        private:
            // column major storage
            int I(int aRow, int aColumn) const
            {
                return (aColumn * S) + aRow;
            }
    };

    __inline Matrix3 operator*(float s, const Matrix3& m)
    {
        return (m * s);
    }
    
}

#endif
