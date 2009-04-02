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

#include <jz_core/Matrix3.h>
#include <jz_core/Matrix4.h>
#include <jz_core/Quaternion.h>

namespace jz
{

    Matrix4 Matrix4::Invert(const Matrix4& m)
    {
        float a0 = (m[0] * m[5]) - (m[1] * m[4]);
        float a1 = (m[0] * m[6]) - (m[2] * m[4]);
        float a2 = (m[0] * m[7]) - (m[3] * m[4]);
        float a3 = (m[1] * m[6]) - (m[2] * m[5]);
        float a4 = (m[1] * m[7]) - (m[3] * m[5]);
        float a5 = (m[2] * m[7]) - (m[3] * m[6]);
        float b0 = (m[8] * m[13]) - (m[9] * m[12]);
        float b1 = (m[8] * m[14]) - (m[10] * m[12]);
        float b2 = (m[8] * m[15]) - (m[11] * m[12]);
        float b3 = (m[9] * m[14]) - (m[10] * m[13]);
        float b4 = (m[9] * m[15]) - (m[11] * m[13]);
        float b5 = (m[10] * m[15]) - (m[11] * m[14]);

        float det = (a0 * b5) - (a1 * b4) 
                  + (a2 * b3) + (a3 * b2) 
                  - (a4 * b1) + (a5 * b0);

        if (AboutZero(det)) { return Matrix4::kZero; }

        const float kInvDet = (1.0f / det);

        Matrix4 ret;
        ret[0]  =  (m[5] * b5)  - (m[6] * b4)  + (m[7] * b3);
        ret[1]  = -(m[1] * b5)  + (m[2] * b4)  - (m[3] * b3);
        ret[2]  =  (m[13] * a5) - (m[14] * a4) + (m[15] * a3);
        ret[3]  = -(m[9] * a5)  + (m[10] * a4) - (m[11] * a3);
        ret[4]  = -(m[4] * b5)  + (m[6] * b2)  - (m[7] * b1);
        ret[5]  =  (m[0] * b5)  - (m[2] * b2)  + (m[3] * b1);
        ret[6]  = -(m[12] * a5) + (m[14] * a2) - (m[15] * a1);
        ret[7]  =  (m[8] * a5)  - (m[10] * a2) + (m[11] * a1);
        ret[8]  =  (m[4] * b4)  - (m[5] * b2)  + (m[7] * b0);
        ret[9]  = -(m[0] * b4)  + (m[1] * b2)  - (m[3] * b0);
        ret[10] =  (m[12] * a4) - (m[13] * a2) + (m[15] * a0);
        ret[11] = -(m[8] * a4)  + (m[9] * a2)  - (m[11] * a0);
        ret[12] = -(m[4] * b3)  + (m[5] * b1)  - (m[6] * b0);
        ret[13] =  (m[0] * b3)  - (m[1] * b1)  + (m[2] * b0);
        ret[14] = -(m[12] * a3) + (m[13] * a1) - (m[14] * a0);
        ret[15] =  (m[8] * a3)  - (m[9] * a1)  + (m[10] * a0);
        ret *= kInvDet;

        return ret;
    }

    Matrix3 Matrix4::GetOrientation() const
    {
        return Matrix3::CreateFromUpperLeft(*this);
    }

    void Matrix4::SetOrientation(const Matrix3& m)
    {
        M11 = m.M11; M12 = m.M12; M13 = m.M13;
        M21 = m.M21; M22 = m.M22; M23 = m.M23;
        M31 = m.M31; M32 = m.M32; M33 = m.M33;
    }

    const Matrix4 Matrix4::kIdentity = Matrix4(1, 0, 0, 0,
                                               0, 1, 0, 0,
                                               0, 0, 1, 0,
                                               0, 0, 0, 1);
    const Matrix4 Matrix4::kZero = Matrix4(0, 0, 0, 0,
                                           0, 0, 0, 0,
                                           0, 0, 0, 0,
                                           0, 0, 0, 0);

    Matrix4 Matrix4::CreateFromRotation(const Matrix3& m)
    {
        return Matrix4(m.M11, m.M12, m.M13, 0,
                       m.M21, m.M22, m.M23, 0,
                       m.M31, m.M32, m.M33, 0,
                       0,     0,     0,     1);
    }

    Matrix4 Matrix4::CreateNormalTransform(const Matrix4& m)
    {
        Matrix3 m3 = Matrix3::Transpose(Matrix3::Invert(Matrix3::CreateFromUpperLeft(m)));
        
        Matrix4 ret = Matrix4(
            m3.M11, m3.M12, m3.M13, m.M14,
            m3.M21, m3.M22, m3.M23, m.M24,
            m3.M31, m3.M32, m3.M33, m.M34,
             m.M41,  m.M42,  m.M43, m.M44);

        return ret;
    }

    Matrix4 Matrix4::CreatePerspectiveOffCenterDirectX(float l, float r, float b, float t, float n, float f)
    {
        Matrix4 m;

        m.M11 = (2.0f * n) / (r - l);
        m.M21 = 0;
        m.M31 = (r + l) / (r - l);
        m.M41 = 0;

        m.M12 = 0;
        m.M22 = (2.0f * n) / (t - b);
        m.M32 = (t + b) / (t - b);
        m.M42 = 0;

        m.M13 = 0;        
        m.M23 = 0;
        m.M33 = -(f / (f - n));
        m.M43 = (-f * n) / (f - n);
        
        m.M14 = 0;
        m.M24 = 0;
        m.M34 = -1;
        m.M44 = 0;

        return m;
    }

    Matrix4 Matrix4::CreatePerspectiveFieldOfViewDirectX(Radian aFov, float aAspectRatio, float n, float f)
    {
        Matrix4 m;
        
        const float k = jz::Tan(aFov * 0.5f);

        const float t = n * k;
        const float b = -t;
        const float r = aAspectRatio * t;
        const float l = -r;

        m = CreatePerspectiveOffCenterDirectX(l, r, b, t, n, f);

        return m;
    }

    Matrix4 Matrix4::CreatePerspectiveOffCenterOpenGL(float l, float r, float b, float t, float n, float f)
    {
        Matrix4 m;

        m.M11 = (2.0f * n) / (r - l);
        m.M21 = 0;
        m.M31 = (r + l) / (r - l);
        m.M41 = 0;

        m.M12 = 0;
        m.M22 = (2.0f * n) / (t - b);
        m.M32 = (t + b) / (t - b);
        m.M42 = 0;

        m.M13 = 0;        
        m.M23 = 0;
        m.M33 = -((f + n) / (f - n));
        m.M43 = (-2.0f * f * n) / (f - n);
        
        m.M14 = 0;
        m.M24 = 0;
        m.M34 = -1;
        m.M44 = 0;

        return m;
    }

    Matrix4 Matrix4::CreatePerspectiveFieldOfViewOpenGL(Radian aFov, float aAspectRatio, float n, float f)
    {
        Matrix4 m;
        
        const float k = jz::Tan(aFov * 0.5f);

        const float t = n * k;
        const float b = -t;
        const float r = aAspectRatio * t;
        const float l = -r;

        m = CreatePerspectiveOffCenterOpenGL(l, r, b, t, n, f);

        return m;
    }

    Matrix4 Matrix4::CreateRotationX(Radian a)
    {
        float cos = jz::Cos(a);
        float sin = jz::Sin(a);

        return Matrix4(
            1,    0,   0, 0,
            0,  cos, sin, 0,
            0, -sin, cos, 0,
            0,    0,   0, 1);
    }

    Matrix4 Matrix4::CreateRotationY(Radian a)
    {
        float cos = jz::Cos(a);
        float sin = jz::Sin(a);

        return Matrix4(
            cos, 0, -sin, 0,
            0,   1,    0, 0,
            sin, 0,  cos, 0,
            0,   0,    0, 1);
    }

    Matrix4 Matrix4::CreateRotationZ(Radian a)
    {
        float cos = jz::Cos(a);
        float sin = jz::Sin(a);

        return Matrix4(
            cos, sin, 0, 0,
           -sin, cos, 0, 0,
            0,     0, 1, 0,
            0,     0, 0, 1);
    }

    Matrix4 Matrix4::CreateScale(float s)
    {
        return Matrix4(s, 0, 0, 0,
                       0, s, 0, 0,
                       0, 0, s, 0,
                       0, 0, 0, 1);
    }

    Matrix4 Matrix4::CreateScale(float sx, float sy, float sz)
    {
        return Matrix4(sx, 0,  0, 0,
                       0, sy,  0, 0,
                       0,  0, sz, 0,
                       0,  0,  0, 1);
    }

    Matrix4 Matrix4::CreateScale(const Vector3& s)
    {
        return Matrix4(s.X, 0, 0, 0,
                       0, s.Y, 0, 0,
                       0, 0, s.Z, 0,
                       0, 0, 0, 1);
    }

    Matrix4 Matrix4::CreateTranslation(const Vector3& t)
    {
        return Matrix4(1, 0, 0, 0,
                       0, 1, 0, 0,
                       0, 0, 1, 0,
                       t.X, t.Y, t.Z, 1);
    }

    void Matrix4::ExtractNearFarDirectX(const Matrix4& aProjectionTransform, float& arNear, float& arFar)
    {
        arNear = aProjectionTransform.M43 / aProjectionTransform.M33;
        arFar = aProjectionTransform.M43 / (1.0f + aProjectionTransform.M33);
    }

    void Matrix4::ExtractNearFarOpenGL(const Matrix4& aProjectionTransform, float& arNear, float& arFar)
    {
        arNear = aProjectionTransform.M43 / (aProjectionTransform.M33 - 1.0f);
        arFar = aProjectionTransform.M43 / (1.0f + aProjectionTransform.M33);
    }

    Radian Matrix4::ExtractFov(const Matrix4& aProjectionTransform)
    {
        Radian ret = 2.0f * (jz::ATan(1.0f / aProjectionTransform.M22));

        return ret;
    }

    float Matrix4::ExtractAspectRatio(const Matrix4& aProjectionTransform)
    {
        float ret = (aProjectionTransform.M22 / aProjectionTransform.M11);

        return ret;
    }

    void Matrix4::UpdateAspectRatioDirectX(float aNewAspectRatio, Matrix4& m)
    {
        float near;
        float far;
        ExtractNearFarDirectX(m, near, far);
        Radian fov = ExtractFov(m);

        m = CreatePerspectiveFieldOfViewDirectX(fov, aNewAspectRatio, near, far);
    }

    void Matrix4::UpdateAspectRatioOpenGL(float aNewAspectRatio, Matrix4& m)
    {
        float near;
        float far;
        ExtractNearFarOpenGL(m, near, far);
        Radian fov = ExtractFov(m);

        m = CreatePerspectiveFieldOfViewOpenGL(fov, aNewAspectRatio, near, far);
    }

    void ToQuaternion(const Matrix4& b, Quaternion& q)
    {
        static const natural skaNext[3] = { 1, 2, 0 };

        const float d = b.M11 + b.M22 + b.M33;
        float r;
        
        if (GreaterThan(d, 0.0f))
        {
            r   = Sqrt(d + 1.0f);
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
    
    void ToTransform(const Quaternion& q, Matrix4& m)
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