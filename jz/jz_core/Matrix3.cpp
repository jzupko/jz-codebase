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

#include <jz_core/Math.h>
#include <jz_core/Matrix2.h>
#include <jz_core/Matrix3.h>
#include <jz_core/Matrix4.h>
#include <jz_core/Quaternion.h>
#include <jz_core/Vector3.h>

namespace jz
{

    const Matrix3 Matrix3::kIdentity = Matrix3(1, 0, 0,
                                               0, 1, 0,
                                               0, 0, 1);
    const Matrix3 Matrix3::kZero = Matrix3(0, 0, 0,
                                           0, 0, 0,
                                           0, 0, 0);

    Matrix2 Matrix3::GetOrientation() const
    {
        return Matrix2(M11, M12,
                       M21, M22);
    }

    void Matrix3::SetOrientation(const Matrix2& m)
    {
        M11 = m.M11; M12 = m.M12;
        M21 = m.M21; M22 = m.M22;
    }

    static const int gskEigenspaceFindIterations = 32;

    static void EigenspaceHelper(Matrix3& R, float& m1, float& m2, float& m3, float& m4, float& m5, int i1, int i2)
    {
        if (!AboutZero(m2))
        {
            const float u    = (0.5f * (m4 - m1)) / m2;
            const float u2   = u * u;
            const float u2p1 = u2 + float(1);
            const float t    = !AboutEqual(u2p1, u2) ? (LessThan(u, 0.0f) ? -1.0f : 1.0f) * (Sqrt(u2p1) - Abs(u)) : 0.5f / u;
            const float c    = 1.0f / Sqrt((t * t) + 1.0f);
            const float s    = c * t;

            m1              -= t * m2;
            m4              += t * m2;
            m2               = 0;

            const float t1   = (c * m3) - (s * m5);
            m5               = (s * m3) + (c * m5);
            m3               = t1;

            for (int i = 0; i < 3; i++)
            {
                const float t2 = (c * R(i,i1)) - (s * R(i,i2));
                R(i,i2)        = (s * R(i,i1)) + (c * R(i,i2));
                R(i,i1)        = t2;
		    }
	    }    
    }

    // From: http://www.terathon.com/code/linear.php
    void Matrix3::FindEigenspace(const Matrix3& M, Vector3& arValues, Matrix3& arVectors)
    {
        float m11 = M.M11; float m12 = M.M12; float m13 = M.M13;
                           float m22 = M.M22; float m23 = M.M23;
                                              float m33 = M.M33;

        Matrix3& R = arVectors;    	
        R = Matrix3::kIdentity;

        for (int i = 0; i < gskEigenspaceFindIterations; i++)
        {
            if (AboutZero(jz::Abs(m12), Constants<float>::kJacobiTolerance) &&
                AboutZero(jz::Abs(m13), Constants<float>::kJacobiTolerance) &&
                AboutZero(jz::Abs(m23), Constants<float>::kJacobiTolerance))
            {
                break;
            }
            
            EigenspaceHelper(R, m11, m12, m13, m22, m23, 0, 1);
            EigenspaceHelper(R, m11, m13, m12, m33, m23, 0, 2);
            EigenspaceHelper(R, m22, m23, m12, m33, m13, 1, 2);
	    }
    	
    	arValues.X = m11;
    	arValues.Y = m22;
    	arValues.Z = m33;
    }

    Matrix3 Matrix3::CreateFromUpperLeft(const Matrix4& m)
    {
        Matrix3 ret = Matrix3(m.M11, m.M12, m.M13,
                              m.M21, m.M22, m.M23,
                              m.M31, m.M32, m.M33);

        return ret;
    }

    Matrix3 Matrix3::CreateScale(float s)
    {
        return Matrix3(s, 0, 0,
                       0, s, 0,
                       0, 0, 1);
    }

    Matrix3 Matrix3::CreateScale(float sx, float sy)
    {
        return Matrix3(sx, 0, 0,
                       0, sy, 0,
                       0,  0, 1);
    }

    Matrix3 Matrix3::CreateScale(const Vector2& s)
    {
        return Matrix3(s.X, 0, 0,
                       0, s.Y, 0,
                       0, 0,   1);
    }

    Matrix3 Matrix3::CreateTranslation(float tx, float ty)
    {
        return Matrix3(1, 0, 0,
                       0, 1, 0,
                       tx, ty, 1);
    }

    Matrix3 Matrix3::CreateTranslation(const Vector2& t)
    {
        return Matrix3(1, 0, 0,
                       0, 1, 0,
                       t.X, t.Y, 1);
    }

    void FromMatrix(const Matrix3& b, Quaternion& q)
    {
        static const natural skaNext[3] = { 1, 2, 0 };

        const float d = b.M11 + b.M22 + b.M33;
        float r;
        
        if (d > 0.0f)
        {
            r = Sqrt(d + 1.0f);
            q.W = 0.5f * r;
            
            r   = 0.5f / r;
            q.X = (b.M23 - b.M32) * r;
            q.Y = (b.M31 - b.M13) * r;
            q.Z = (b.M12 - b.M21) * r;
        }
        else
        {
            natural i = 0;
            
            if (b.M22 > b.M11) { i = 1; }
            if (b.M33 > b(i, i)) { i = 2; }

            natural j = skaNext[i];
            natural k = skaNext[j];

            r = Sqrt(b(i, i) - b(j, j) - b(k, k) + 1.0f);

            float* pa[3] = { &(q.X), &(q.Y), &(q.Z) };
            
            *pa[i] = 0.5f * r;
            
            r = (0.5f / r);
            q.W = (b(j, k) - b(k, j)) * r;

            *pa[j] = (b(i, j) + b(j, i)) * r;
            *pa[k] = (b(i, k) + b(k, i)) * r;
        }
    }
    
    void ToMatrix(const Quaternion& q, Matrix3& m)
    {
        const float x = 2.0f * q.X;
        const float y = 2.0f * q.Y;
        const float z = 2.0f * q.Z;

        const float wx = q.W * x;
        const float wy = q.W * y;
        const float wz = q.W * z;
        const float xx = q.X * x;
        const float xy = q.X * y;
        const float xz = q.X * z;
        const float yy = q.Y * y;
        const float yz = q.Y * z;
        const float zz = q.Z * z;

        m.M11 = 1.0f - (yy + zz);
        m.M21 = (xy - wz);
        m.M31 = (xz + wy);

        m.M12 = (xy + wz);
        m.M22 = 1.0f - (xx + zz);
        m.M32 = (yz - wx);

        m.M13 = (xz - wy);
        m.M23 = (yz + wx);
        m.M33 = 1.0f - (xx + yy);
    }

}
