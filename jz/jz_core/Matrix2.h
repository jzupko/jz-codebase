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
#ifndef _JZ_MATRIX2_H_
#define _JZ_MATRIX2_H_

#include <jz_core/Math.h>
#include <jz_core/Vector2.h>
#include <type_traits>

namespace jz
{

    struct Matrix3;
    struct Matrix4;
    struct Matrix2
    {
        static const int S = 2;
        static const int N = S * S;

        union
        {
            // column major storage
            struct
            {
                float M11, M21;
                float M12, M22;
            };

            float pData[N];
        };

        Matrix2()
            : M11(0), M12(0),
              M21(0), M22(0)
        {}

        explicit Matrix2(float s)
            : M11(s), M12(s),
              M21(s), M22(s)
        {}

        Matrix2(float m11, float m12,
                float m21, float m22)
            : M11(m11), M12(m12),
              M21(m21), M22(m22)
        {}

        Matrix2(const Matrix2& m)
            : M11(m.M11), M12(m.M12),
              M21(m.M21), M22(m.M22)
        {}

        Matrix2& operator=(const Matrix2& m)
        {
            M11 = m.M11; M12 = m.M12;
            M21 = m.M21; M22 = m.M22;

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

        bool operator==(const Matrix2& m) const
        {
            return (M11 == m.M11 && M12 == m.M12 &&
                    M21 == m.M21 && M22 == m.M22);
        }

        bool operator!=(const Matrix2& m) const
        {
            return !(*this == m);
        }

        Matrix2 operator+(const Matrix2& m) const
        {
            return Matrix2(M11 + m.M11, M12 + m.M12,
                           M21 + m.M21, M22 + m.M22);
        }

        Matrix2& operator+=(const Matrix2& m)
        {
            M11 += m.M11; M12 += m.M12;
            M21 += m.M21; M22 += m.M22;

            return *this;
        }

        Matrix2 operator/(float s) const
        {
            return Matrix2(M11 / s, M12 / s,
                           M21 / s, M22 / s);
        }

        Matrix2& operator/=(float s)
        {
            M11 /= s; M12 /= s; 
            M21 /= s; M22 /= s;

            return *this;
        }

        Matrix2 operator*(float s) const
        {
            return Matrix2(M11 * s, M12 * s,
                           M21 * s, M22 * s);
        }

        Matrix2& operator*=(float s)
        {
            M11 *= s; M12 *= s;
            M21 *= s; M22 *= s;

            return *this;
        }

        Matrix2 operator*(const Matrix2& m) const
        {
            // column 1
            const float m11 = (M11 * m.M11) + (M12 * m.M21);
            const float m21 = (M21 * m.M11) + (M22 * m.M21);

            // column 2
            const float m12 = (M11 * m.M12) + (M12 * m.M22);
            const float m22 = (M21 * m.M12) + (M22 * m.M22);

            return Matrix2(m11, m12,
                           m21, m22);
        }

        Matrix2 operator-() const
        {
            return Matrix2(-M11, -M12,
                           -M21, -M22);
        }

        Matrix2 operator-(const Matrix2& m) const
        {
            return Matrix2(M11 - m.M11, M12 - m.M12,
                           M21 - m.M21, M22 - m.M22);
        }

        Matrix2& operator-=(const Matrix2& m)
        {
            M11 -= m.M11; M12 -= m.M12;
            M21 -= m.M21; M22 -= m.M22;

            return *this;
        }  

        Vector2 GetCol(natural i) const
        {
            const Matrix2& m = *this;

            return Vector2(m(0, i), m(1, i));
        }

        Vector2 GetRow(natural i) const
        {
            const Matrix2& m = *this;
            
            return Vector2(m(i, 0), m(i, 1));
        }

		bool IsReflection() const
		{
            bool bReturn = (Determinant() < 0.0f);

			return bReturn;
		}

        float Determinant() const
		{
            float ret = (M11 * M22) - (M12 * M21);

			return ret;
		}

        float Sum() const
        {
            return (M11 + M12 +
                    M21 + M22);
        }

        float Trace() const
        {
            return (M11 + M22);
        }

        static bool AboutEqual(const Matrix2& a, const Matrix2& b, float aTolerance = Constants<float>::kZeroTolerance)
        {
            return jz::AboutEqual(a.M11, b.M11, aTolerance) && jz::AboutEqual(a.M12, b.M12, aTolerance) &&
                   jz::AboutEqual(a.M21, b.M21, aTolerance) && jz::AboutEqual(a.M22, b.M22, aTolerance);
        }

        static Matrix2 Abs(const Matrix2& a)
        {
            return Matrix2(jz::Abs(a.M11), jz::Abs(a.M12),
                           jz::Abs(a.M21), jz::Abs(a.M22));
        }

        static Matrix2 Clamp(const Matrix2& a, const Matrix2& aMin, const Matrix2& aMax)
        {
            return Matrix2(jz::Clamp(a.M11, aMin.M11, aMax.M11), jz::Clamp(a.M12, aMin.M12, aMax.M12),
                           jz::Clamp(a.M21, aMin.M21, aMax.M21), jz::Clamp(a.M22, aMin.M22, aMax.M22));
        }


        static Matrix2 Lerp(const Matrix2& a, const Matrix2& b, float aWeightOfB)
        {   
            return Matrix2(jz::Lerp(a.M11, b.M11, aWeightOfB), jz::Lerp(a.M12, b.M12, aWeightOfB),
                           jz::Lerp(a.M21, b.M21, aWeightOfB), jz::Lerp(a.M22, b.M22, aWeightOfB));
        }

        static Matrix2 Min(const Matrix2& a, const Matrix2& b)
        {   
            return Matrix2(jz::Min(a.M11, b.M11), jz::Min(a.M12, b.M12),
                           jz::Min(a.M21, b.M21), jz::Min(a.M22, b.M22));
        }

        static Matrix2 Max(const Matrix2& a, const Matrix2& b)
        {   
            return Matrix2(jz::Max(a.M11, b.M11), jz::Max(a.M12, b.M12),
                           jz::Max(a.M21, b.M21), jz::Max(a.M22, b.M22));
        }

        static Matrix2 Transpose(const Matrix2& a)
        {
            return Matrix2(a.M11, a.M21,
                           a.M12, a.M22);
        }

        static Matrix2 Invert(const Matrix2& a)
        {
            float det = a.Determinant();

            if (!jz::AboutZero(det))
            {
                Matrix2 ret = Matrix2(
                    (det * a.M22), (-det * a.M12),
                   (-det * a.M21),  (det * a.M11));

                return ret;
            }
            else
            {
                return a;
            }
        }

        static const Matrix2 kIdentity;
        static const Matrix2 kZero;

        static Matrix2 CreateFromUpperLeft(const Matrix3& m);
        static Matrix2 CreateFromUpperLeft(const Matrix4& m);

        static Matrix2 CreateRotation(Radian a);
        static Matrix2 CreateScale(float s);
        static Matrix2 CreateScale(float sx, float sy);
        static Matrix2 CreateScale(const Vector2& s);

    private:
        // column major storage
        int I(int aRow, int aColumn) const
        {
            return (aColumn * S) + aRow;
        }
    };

    __inline Matrix2 operator*(float s, const Matrix2& m)
    {
        return (m * s);
    }
    
}

#endif
