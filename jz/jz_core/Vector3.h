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
#ifndef _JZ_VECTOR3_H_
#define _JZ_VECTOR3_H_

#include <jz_core/Angle.h>
#include <jz_core/Vector2.h>
#include <type_traits>

namespace jz
{

    struct CoordinateFrame3D;
    struct Matrix4;
    struct Matrix3;
    struct Quaternion;
    struct Vector3
    {
        union
        {
            struct
            {
                float X, Y, Z;
            };
            float pData[3];
        };

#       define JZ_NUMBER_ARRAY_NAME Vector3
#       define JZ_NUMBER_ARRAY_DIM  3
#       include <jz_core/_NumberArray.h>

        Vector3(const Vector2& v, float z)
            : X(v.X), Y(v.Y), Z(z)
        {}

        static Vector3 Cross(const Vector3& a, const Vector3& b)
        {
            Vector3 ret;
            ret.X = (a.Y * b.Z) - (a.Z * b.Y);
            ret.Y = (a.Z * b.X) - (a.X * b.Z);
            ret.Z = (a.X * b.Y) - (a.Y * b.X);

            return ret;
        }

        static Vector3 UnitCross(const Vector3& a, const Vector3& b)
        {
            Vector3 ret = Vector3::Normalize(Vector3::Cross(a, b));
            
            return ret;
        }

        static Vector3 CreateFromAxis(Axis::Id aAxis)
        {
            Vector3 ret(Vector3::kZero);
            ret[aAxis] = 1.0f;

            return ret;
        }

        static Vector3 Transform(const Quaternion& q, const Vector3& u);
        static Vector3 Transform(const Matrix3& m, const Vector3& v);
        static Vector3 TransformDirection(const CoordinateFrame3D& m, const Vector3& v);
        static Vector3 TransformPosition(const CoordinateFrame3D& m, const Vector3& v);
        static Vector3 TransformDirection(const Matrix4& m, const Vector3& v);
        static Vector3 TransformPosition(const Matrix4& m, const Vector3& v);

        static const Vector3 kBackward;
        static const Vector3 kDown;
        static const Vector3 kForward;
        static const Vector3 kLeft;
        static const Vector3 kMax;        
        static const Vector3 kMin;
        static const Vector3 kNegOne;        
        static const Vector3 kNegUnitX;
        static const Vector3 kNegUnitY;
        static const Vector3 kNegUnitZ;
        static const Vector3 kOne;
        static const Vector3 kRight;
        static const Vector3 kUnitX;
        static const Vector3 kUnitY;
        static const Vector3 kUnitZ;
        static const Vector3 kUp;
        static const Vector3 kZero;
    };

    __inline Vector3 operator*(float s, const Vector3& v)
    {
        return (v * s);
    }

	template <typename ITR>
	int CalculatePrincipalComponentAxes(ITR aBegin, ITR aEnd, Vector3& arR, Vector3& arS, Vector3& arT)
	{
		int count = 0;
		Vector3 mean = Vector3::kZero;

		for (ITR I = aBegin; I != aEnd; I++)
		{
			mean += (*I);
			count++;
		}

		float invCount = (count > 0) ? ((float)(1.0 / ((double)count))) : 0.0f;
		mean *= invCount;

        float m11 = 0.0f;
        float m22 = 0.0f;
        float m33 = 0.0f;
        float m12 = 0.0f;
        float m13 = 0.0f;
        float m23 = 0.0f;

		for (ITR I = aBegin; I != aEnd; I++)
		{
			Vector3 v = ((*I) - mean);
            m11 += v.X * v.X;
            m22 += v.Y * v.Y;
            m33 += v.Z * v.Z;
            m12 += v.X * v.Y;
            m13 += v.X * v.Z;
            m23 += v.Y * v.Z;
		}

        m11 *= invCount;
        m22 *= invCount;
        m33 *= invCount;
        m12 *= invCount;
        m13 *= invCount;
        m23 *= invCount;

		Matrix3 m = Matrix3(m11, m12, m13, m12, m22, m23, m13, m23, m33);
        Vector3 eigenValues;
        Matrix3 eigenVectors;

        Matrix3::FindEigenspace(m, eigenValues, eigenVectors);

        m11 = jz::Abs(eigenValues.X);
        m22 = jz::Abs(eigenValues.Y);
        m33 = jz::Abs(eigenValues.Z);

        if (jz::GreaterThan(m11, m22) && jz::GreaterThan(m11, m33))
        {
            arR = Vector3(eigenVectors.M11, eigenVectors.M21, eigenVectors.M31);

            if (jz::GreaterThan(m22, m33))
            {
                arS = Vector3(eigenVectors.M12, eigenVectors.M22, eigenVectors.M32);
                arT = Vector3(eigenVectors.M13, eigenVectors.M23, eigenVectors.M33);
            }
            else
            {
                arS = Vector3(eigenVectors.M13, eigenVectors.M23, eigenVectors.M33);
                arT = Vector3(eigenVectors.M12, eigenVectors.M22, eigenVectors.M32);
            }
        }
        else if (jz::GreaterThan(m22, m33))
        {
            arR = Vector3(eigenVectors.M12, eigenVectors.M22, eigenVectors.M32);

            if (jz::GreaterThan(m11, m33))
            {
                arS = Vector3(eigenVectors.M11, eigenVectors.M21, eigenVectors.M31);
                arT = Vector3(eigenVectors.M13, eigenVectors.M23, eigenVectors.M33);
            }
            else
            {
                arS = Vector3(eigenVectors.M13, eigenVectors.M23, eigenVectors.M33);
                arT = Vector3(eigenVectors.M11, eigenVectors.M21, eigenVectors.M31);
            }
        }
        else
        {
            arR = Vector3(eigenVectors.M13, eigenVectors.M23, eigenVectors.M33);

            if (jz::GreaterThan(m11, m22))
            {
                arS = Vector3(eigenVectors.M11, eigenVectors.M21, eigenVectors.M31);
                arT = Vector3(eigenVectors.M12, eigenVectors.M22, eigenVectors.M32);
            }
            else
            {
                arS = Vector3(eigenVectors.M12, eigenVectors.M22, eigenVectors.M32);
                arT = Vector3(eigenVectors.M11, eigenVectors.M21, eigenVectors.M31);
            }
        }

        arR = Vector3::Normalize(arR);
        arS = Vector3::Normalize(arS);
        arT = Vector3::Normalize(arT);

	    Matrix3 refl = Matrix3(arR.X, arS.X, arT.X,
						       arR.Y, arS.Y, arT.Y,
						       arR.Z, arS.Z, arT.Z);

		if (refl.IsReflection()) { (arT = -arT); }

        return count;
	}

	template <typename ITR>
	void CalculateCenter(ITR aBegin, ITR aEnd, const Vector3& r, const Vector3& s, const Vector3& t, Vector3& arCenter)
	{
		Vector3 min = Vector3::kMax;
		Vector3 max = Vector3::kMin;

		for (ITR I = aBegin; I != aEnd; I++)
		{
			Vector3 a;
			a.X = I->Dot(r);
			a.Y = I->Dot(s);
			a.Z = I->Dot(t):

			min = min.GetMin(a);
			max = max.GetMax(a);
		}

		Vector3 abc = 0.5f * (max + min);
		arCenter = (abc.X * r) + (abc.Y * s) + (abc.Z * t);
	}

	template <typename ITR>
	void CalculateCenterAndHalfExtents(ITR aBegin, ITR aEnd, const Vector3& r, const Vector3& s, const Vector3& t, Vector3& arCenter, Vector3& arHalfExtents)
	{
		Vector3 min = Vector3::kMax;
		Vector3 max = Vector3::kMin;

		for (ITR I = aBegin; I != aEnd; I++)
		{
            const Vector3& u = *I;
			Vector3 a;
            a.X = Vector3::Dot(u, r);
            a.Y = Vector3::Dot(u, s);
            a.Z = Vector3::Dot(u, t);

            min = Vector3::Min(min, a);
            max = Vector3::Max(max, a);
		}

		Vector3 abc = 0.5f * (max + min);
		arCenter = (abc.X * r) + (abc.Y * s) + (abc.Z * t);
		arHalfExtents = 0.5f * (max - min);
	}

}

#endif
