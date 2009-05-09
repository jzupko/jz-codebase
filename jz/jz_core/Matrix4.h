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
#ifndef _JZ_MATRIX4_H_
#define _JZ_MATRIX4_H_

#include <jz_core/Prereqs.h>
#include <jz_core/Matrix3.h>
#include <jz_core/Vector3.h>
#include <jz_core/Vector4.h>
#include <type_traits>

namespace jz
{

    struct Plane;
    struct Quaternion;

    struct Matrix4
    {
        static const int S = 4;
        static const int N = S * S;

        union
        {
            // row-major storage
            struct
            {
                float M11, M12, M13, M14;
                float M21, M22, M23, M24;
                float M31, M32, M33, M34;
                float M41, M42, M43, M44;
            };

            float pData[N];
        };

        Matrix4()
            : M11(0), M12(0), M13(0), M14(0),
              M21(0), M22(0), M23(0), M24(0),
              M31(0), M32(0), M33(0), M34(0),
              M41(0), M42(0), M43(0), M44(0)
        {}

        explicit Matrix4(float s)
            : M11(s), M12(s), M13(s), M14(s),
              M21(s), M22(s), M23(s), M24(s),
              M31(s), M32(s), M33(s), M34(s),
              M41(s), M42(s), M43(s), M44(s)
        {}

        Matrix4(float m11, float m12, float m13, float m14,
                float m21, float m22, float m23, float m24,
                float m31, float m32, float m33, float m34,
                float m41, float m42, float m43, float m44)
            : M11(m11), M12(m12), M13(m13), M14(m14),
              M21(m21), M22(m22), M23(m23), M24(m24),
              M31(m31), M32(m32), M33(m33), M34(m34),
              M41(m41), M42(m42), M43(m43), M44(m44)
        {}

        Matrix4(const Matrix4& b)
            : M11(b.M11), M12(b.M12), M13(b.M13), M14(b.M14),
              M21(b.M21), M22(b.M22), M23(b.M23), M24(b.M24),
              M31(b.M31), M32(b.M32), M33(b.M33), M34(b.M34),
              M41(b.M41), M42(b.M42), M43(b.M43), M44(b.M44)
        {}

        Matrix4& operator=(const Matrix4& b)
        {
            M11 = b.M11; M12 = b.M12; M13 = b.M13; M14 = b.M14;
            M21 = b.M21; M22 = b.M22; M23 = b.M23; M24 = b.M24;
            M31 = b.M31; M32 = b.M32; M33 = b.M33; M34 = b.M34;
            M41 = b.M41; M42 = b.M42; M43 = b.M43; M44 = b.M44;

            return *this;
        }

        float operator[](size_t i) const { return pData[i]; }
        float& operator[](size_t i) { return pData[i]; }

        float operator()(int aRow, int aColumn) const
        {
            return pData[I(aRow, aColumn)];
        }

        float& operator()(int aRow, int aColumn)
        {
            return pData[I(aRow, aColumn)];
        }

        bool operator==(const Matrix4& b) const
        {
            return (M11 == b.M11 && M12 == b.M12 && M13 == b.M13 && M14 == b.M14) &&
                   (M21 == b.M21 && M22 == b.M22 && M23 == b.M23 && M24 == b.M24) &&
                   (M31 == b.M31 && M32 == b.M32 && M33 == b.M33 && M34 == b.M34) &&
                   (M41 == b.M41 && M42 == b.M42 && M43 == b.M43 && M44 == b.M44);
        }

        bool operator!=(const Matrix4& m) const
        {
            return !(*this == m);
        }

        Matrix4 operator+(const Matrix4& b) const
        {
            return Matrix4(M11 + b.M11, M12 + b.M12, M13 + b.M13, M14 + b.M14,
                           M21 + b.M21, M22 + b.M22, M23 + b.M23, M24 + b.M24,
                           M31 + b.M31, M32 + b.M32, M33 + b.M33, M34 + b.M34,
                           M41 + b.M41, M42 + b.M42, M43 + b.M43, M44 + b.M44);
        }

        Matrix4& operator+=(const Matrix4& b)
        {
            M11 += b.M11; M12 += b.M12; M13 += b.M13; M14 += b.M14;
            M21 += b.M21; M22 += b.M22; M23 += b.M23; M24 += b.M24;
            M31 += b.M31; M32 += b.M32; M33 += b.M33; M34 += b.M34;
            M41 += b.M41; M42 += b.M42; M43 += b.M43; M44 += b.M44;

            return *this;
        }

        Matrix4 operator/(float s) const
        {
            return Matrix4(M11 / s, M12 / s, M13 / s, M14 / s,
                           M21 / s, M22 / s, M23 / s, M24 / s,
                           M31 / s, M32 / s, M33 / s, M34 / s,
                           M41 / s, M42 / s, M43 / s, M44 / s);
        }

        Matrix4& operator/=(float s)
        {
            M11 /= s; M12 /= s; M13 /= s; M14 /= s;
            M21 /= s; M22 /= s; M23 /= s; M24 /= s;
            M31 /= s; M32 /= s; M33 /= s; M34 /= s;
            M41 /= s; M42 /= s; M43 /= s; M44 /= s;

            return *this;
        }

        Matrix4 operator*(float s) const
        {
            return Matrix4(M11 * s, M12 * s, M13 * s, M14 * s,
                           M21 * s, M22 * s, M23 * s, M24 * s,
                           M31 * s, M32 * s, M33 * s, M34 * s,
                           M41 * s, M42 * s, M43 * s, M44 * s);
        }

        Matrix4& operator*=(float s)
        {
            M11 *= s; M12 *= s; M13 *= s; M14 *= s;
            M21 *= s; M22 *= s; M23 *= s; M24 *= s;
            M31 *= s; M32 *= s; M33 *= s; M34 *= s;
            M41 *= s; M42 *= s; M43 *= s; M44 *= s;

            return *this;
        }

        Matrix4 operator*(const Matrix4& b) const
        {
            // column 1
            const float m11 = (M11 * b.M11) + (M12 * b.M21) + (M13 * b.M31) + (M14 * b.M41);
            const float m21 = (M21 * b.M11) + (M22 * b.M21) + (M23 * b.M31) + (M24 * b.M41);
            const float m31 = (M31 * b.M11) + (M32 * b.M21) + (M33 * b.M31) + (M34 * b.M41);
            const float m41 = (M41 * b.M11) + (M42 * b.M21) + (M43 * b.M31) + (M44 * b.M41);

            // column 2
            const float m12 = (M11 * b.M12) + (M12 * b.M22) + (M13 * b.M32) + (M14 * b.M42);
            const float m22 = (M21 * b.M12) + (M22 * b.M22) + (M23 * b.M32) + (M24 * b.M42);
            const float m32 = (M31 * b.M12) + (M32 * b.M22) + (M33 * b.M32) + (M34 * b.M42);
            const float m42 = (M41 * b.M12) + (M42 * b.M22) + (M43 * b.M32) + (M44 * b.M42);

            // column 3
            const float m13 = (M11 * b.M13) + (M12 * b.M23) + (M13 * b.M33) + (M14 * b.M43);
            const float m23 = (M21 * b.M13) + (M22 * b.M23) + (M23 * b.M33) + (M24 * b.M43);
            const float m33 = (M31 * b.M13) + (M32 * b.M23) + (M33 * b.M33) + (M34 * b.M43);
            const float m43 = (M41 * b.M13) + (M42 * b.M23) + (M43 * b.M33) + (M44 * b.M43);

            // column 4
            const float m14 = (M11 * b.M14) + (M12 * b.M24) + (M13 * b.M34) + (M14 * b.M44);
            const float m24 = (M21 * b.M14) + (M22 * b.M24) + (M23 * b.M34) + (M24 * b.M44);
            const float m34 = (M31 * b.M14) + (M32 * b.M24) + (M33 * b.M34) + (M34 * b.M44);
            const float m44 = (M41 * b.M14) + (M42 * b.M24) + (M43 * b.M34) + (M44 * b.M44);

            return Matrix4(m11, m12, m13, m14,
                           m21, m22, m23, m24,
                           m31, m32, m33, m34,
                           m41, m42, m43, m44);
        }

        Matrix4 operator-() const
        {
            return Matrix4(-M11, -M12, -M13, -M14,
                           -M21, -M22, -M23, -M24,
                           -M31, -M32, -M33, -M34,
                           -M41, -M42, -M43, -M44);
        }

        Matrix4 operator-(const Matrix4& b) const
        {
            return Matrix4(M11 - b.M11, M12 - b.M12, M13 - b.M13, M14 - b.M14,
                           M21 - b.M21, M22 - b.M22, M23 - b.M23, M24 - b.M24,
                           M31 - b.M31, M32 - b.M32, M33 - b.M33, M34 - b.M34,
                           M41 - b.M41, M42 - b.M42, M43 - b.M43, M44 - b.M44);
        }

        Matrix4& operator-=(const Matrix4& b)
        {
            M11 -= b.M11; M12 -= b.M12; M13 -= b.M13; M14 -= b.M14;
            M21 -= b.M21; M22 -= b.M22; M23 -= b.M23; M24 -= b.M24;
            M31 -= b.M31; M32 -= b.M32; M33 -= b.M33; M34 -= b.M34;
            M41 -= b.M41; M42 -= b.M42; M43 -= b.M43; M44 -= b.M44;

            return *this;
        }  

        Vector4 GetCol(natural i) const
        {
            const Matrix4& m = *this;

            return Vector4(m(0, i), m(1, i), m(2, i), m(3, i));
        }

        Vector4 GetRow(natural i) const
        {
            const Matrix4& m = *this;
            
            return Vector4(m(i, 0), m(i, 1), m(i, 2), m(i, 3));
        }
        
        float Sum() const
        {
            return (M11 + M12 + M13 + M14 +
                    M21 + M22 + M23 + M24 +
                    M31 + M32 + M33 + M34 +
                    M41 + M42 + M43 + M44);
        }

        void SwapRows(natural r1, natural r2)
        {
            pData[I(r1, 0)] = pData[I(r2, 0)];
            pData[I(r1, 1)] = pData[I(r2, 1)];
            pData[I(r1, 2)] = pData[I(r2, 2)];
            pData[I(r1, 3)] = pData[I(r2, 3)];                                    
        }

        Matrix3 GetOrientation() const;
        void SetOrientation(const Matrix3& m);

        Vector3 GetTranslation() const
        {
            return Vector3(M41, M42, M43);
        }

        void SetTranslation(const Vector3& v)
        {
            M41 = v.X; M42 = v.Y; M43 = v.Z;
        }

        static bool AboutEqual(const Matrix4& a, const Matrix4& b, float aTolerance = Constants<float>::kZeroTolerance)
        {
            return (jz::AboutEqual(a.M11, b.M11, aTolerance) && jz::AboutEqual(a.M12, b.M12, aTolerance) && jz::AboutEqual(a.M13, b.M13, aTolerance) && jz::AboutEqual(a.M14, b.M14, aTolerance)) &&
                   (jz::AboutEqual(a.M21, b.M21, aTolerance) && jz::AboutEqual(a.M22, b.M22, aTolerance) && jz::AboutEqual(a.M23, b.M23, aTolerance) && jz::AboutEqual(a.M24, b.M24, aTolerance)) &&
                   (jz::AboutEqual(a.M31, b.M31, aTolerance) && jz::AboutEqual(a.M32, b.M32, aTolerance) && jz::AboutEqual(a.M33, b.M33, aTolerance) && jz::AboutEqual(a.M34, b.M34, aTolerance)) &&
                   (jz::AboutEqual(a.M41, b.M41, aTolerance) && jz::AboutEqual(a.M42, b.M42, aTolerance) && jz::AboutEqual(a.M43, b.M43, aTolerance) && jz::AboutEqual(a.M44, b.M44, aTolerance));
        }

        static Matrix4 Abs(const Matrix4& a)
        {
            return Matrix4(jz::Abs(a.M11), jz::Abs(a.M12), jz::Abs(a.M13), jz::Abs(a.M14),
                           jz::Abs(a.M21), jz::Abs(a.M22), jz::Abs(a.M23), jz::Abs(a.M24),
                           jz::Abs(a.M31), jz::Abs(a.M32), jz::Abs(a.M33), jz::Abs(a.M34),
                           jz::Abs(a.M41), jz::Abs(a.M42), jz::Abs(a.M43), jz::Abs(a.M44));
        }

        static Matrix4 Clamp(const Matrix4& a, const Matrix4& aMin, const Matrix4& aMax)
        {
            return Matrix4(jz::Clamp(a.M11, aMin.M11, aMax.M11), jz::Clamp(a.M12, aMin.M12, aMax.M12), jz::Clamp(a.M13, aMin.M13, aMax.M13), jz::Clamp(a.M14, aMin.M14, aMax.M14),
                           jz::Clamp(a.M21, aMin.M21, aMax.M21), jz::Clamp(a.M22, aMin.M22, aMax.M22), jz::Clamp(a.M23, aMin.M23, aMax.M23), jz::Clamp(a.M24, aMin.M24, aMax.M24),
                           jz::Clamp(a.M31, aMin.M31, aMax.M31), jz::Clamp(a.M32, aMin.M32, aMax.M32), jz::Clamp(a.M33, aMin.M33, aMax.M33), jz::Clamp(a.M34, aMin.M34, aMax.M34),
                           jz::Clamp(a.M41, aMin.M41, aMax.M41), jz::Clamp(a.M42, aMin.M42, aMax.M42), jz::Clamp(a.M43, aMin.M43, aMax.M43), jz::Clamp(a.M44, aMin.M44, aMax.M44));
        }

        static bool IsOrthogonal(const Matrix4& m)
        {
            Matrix3 transpose = Matrix3::CreateFromUpperLeft(m);
            Matrix3 inverse = Matrix3::Invert(transpose);
            transpose = Matrix3::Transpose(transpose);

            bool bReturn = Matrix3::AboutEqual(transpose, inverse, Constants<float>::kLooseTolerance);

            return bReturn;
        }

        static Matrix4 Invert(const Matrix4& m);

        static Matrix4 Lerp(const Matrix4& a, const Matrix4& b, float aWeightOfB)
        {   
            return Matrix4(jz::Lerp(a.M11, b.M11, aWeightOfB), jz::Lerp(a.M12, b.M12, aWeightOfB), jz::Lerp(a.M13, b.M13, aWeightOfB), jz::Lerp(a.M14, b.M14, aWeightOfB),
                           jz::Lerp(a.M21, b.M21, aWeightOfB), jz::Lerp(a.M22, b.M22, aWeightOfB), jz::Lerp(a.M23, b.M23, aWeightOfB), jz::Lerp(a.M24, b.M24, aWeightOfB),
                           jz::Lerp(a.M31, b.M31, aWeightOfB), jz::Lerp(a.M32, b.M32, aWeightOfB), jz::Lerp(a.M33, b.M33, aWeightOfB), jz::Lerp(a.M34, b.M34, aWeightOfB),
                           jz::Lerp(a.M41, b.M41, aWeightOfB), jz::Lerp(a.M42, b.M42, aWeightOfB), jz::Lerp(a.M43, b.M43, aWeightOfB), jz::Lerp(a.M44, b.M44, aWeightOfB));
        }

        static Matrix4 Min(const Matrix4& a, const Matrix4& b)
        {   
            return Matrix4(jz::Min(a.M11, b.M11), jz::Min(a.M12, b.M12), jz::Min(a.M13, b.M13), jz::Min(a.M14, b.M14),
                           jz::Min(a.M21, b.M21), jz::Min(a.M22, b.M22), jz::Min(a.M23, b.M23), jz::Min(a.M24, b.M24),
                           jz::Min(a.M31, b.M31), jz::Min(a.M32, b.M32), jz::Min(a.M33, b.M33), jz::Min(a.M34, b.M34),
                           jz::Min(a.M41, b.M41), jz::Min(a.M42, b.M42), jz::Min(a.M43, b.M43), jz::Min(a.M44, b.M44));
        }

        static Matrix4 Max(const Matrix4& a, const Matrix4& b)
        {   
            return Matrix4(jz::Max(a.M11, b.M11), jz::Max(a.M12, b.M12), jz::Max(a.M13, b.M13), jz::Max(a.M14, b.M14),
                           jz::Max(a.M21, b.M21), jz::Max(a.M22, b.M22), jz::Max(a.M23, b.M23), jz::Max(a.M24, b.M24),
                           jz::Max(a.M31, b.M31), jz::Max(a.M32, b.M32), jz::Max(a.M33, b.M33), jz::Max(a.M34, b.M34),
                           jz::Max(a.M41, b.M41), jz::Max(a.M42, b.M42), jz::Max(a.M43, b.M43), jz::Max(a.M44, b.M44));
        }
        
        static Matrix4 Transpose(const Matrix4& m)
        {
            return Matrix4(m.M11, m.M21, m.M31, m.M41,
                           m.M12, m.M22, m.M32, m.M42,
                           m.M13, m.M23, m.M33, m.M43,
                           m.M14, m.M24, m.M34, m.M44);
        }

        static const Matrix4 kIdentity;
        static const Matrix4 kZero;

        static Matrix4 CreateNormalTransform(const Matrix4& m);
        static Matrix4 CreateFromRotation(const Matrix3& m);
        static Matrix4 CreatePerspectiveOffCenterDirectX(float aLeft, float aRight, float aBottom, float aTop, float aNear, float aFar);
        static Matrix4 CreatePerspectiveFieldOfViewDirectX(Radian aFov, float aAspectRatio, float aNear, float aFar);
        static Matrix4 CreatePerspectiveOffCenterOpenGL(float aLeft, float aRight, float aBottom, float aTop, float aNear, float aFar);
        static Matrix4 CreatePerspectiveFieldOfViewOpenGL(Radian aFov, float aAspectRatio, float aNear, float aFar);
        static Matrix4 CreateReflection(const Plane& p);
        static Matrix4 CreateRotationX(Radian a);
        static Matrix4 CreateRotationY(Radian a);
        static Matrix4 CreateRotationZ(Radian a);
        static Matrix4 CreateScale(float s);
        static Matrix4 CreateScale(float sx, float sy, float sz);
        static Matrix4 CreateScale(const Vector3& s);
        static Matrix4 CreateTranslation(const Vector3& t);

        static void UpdateAspectRatioDirectX(float aNewAspectRatio, Matrix4& m);
        static void UpdateAspectRatioOpenGL(float aNewAspectRatio, Matrix4& m);

        static void ExtractNearFarDirectX(const Matrix4& aProjectionTransform, float& arNear, float& arFar);
        static void ExtractNearFarOpenGL(const Matrix4& aProjectionTransform, float& arNear, float& arFar);
        static Radian ExtractFov(const Matrix4& aProjectionTransform);
        static float ExtractAspectRatio(const Matrix4& aProjectionTransform);

        private:
            // row-major storage
            int I(int aRow, int aColumn) const
            {
                return (aRow * S) + aColumn;
            }
    };

    inline Matrix4 operator*(float s, const Matrix4& m)
    {
        return Matrix4(m.M11 * s, m.M12 * s, m.M13 * s, m.M14 * s,
                       m.M21 * s, m.M22 * s, m.M23 * s, m.M24 * s,
                       m.M31 * s, m.M32 * s, m.M33 * s, m.M34 * s,
                       m.M41 * s, m.M42 * s, m.M43 * s, m.M44 * s);
    }

    void ToQuaternion(const Matrix4& b, Quaternion& q);
    void ToTransform(const Quaternion& q, Matrix4& b);
    
    inline void ToVector3(const Matrix4& b, Vector3& v)
    {
        v.X = b.M41;
        v.Y = b.M42;
        v.Z = b.M43;
    }

    inline void ToTransform(const Vector3& v, Matrix4& b)
    {
        b.M41 = v.X;
        b.M42 = v.Y;
        b.M43 = v.Z;
    }
    
	/// <summary>
	/// Creates a matrix that will transform a point into the coordinate frame formed by the three axes and the center.
	/// </summary>
	static void CalculateMatrix(const Vector3& aCenter, const Vector3& r, const Vector3& s, const Vector3& t, Matrix4& arOut)
	{
		arOut = Matrix4(
            r.X, r.Y, r.Z, 0,
            s.X, s.Y, s.Z, 0,
            t.X, t.Y, t.Z, 0,
            aCenter.X, aCenter.Y, aCenter.Z, 1);
	}

}

#endif
