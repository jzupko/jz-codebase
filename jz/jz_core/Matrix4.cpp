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
#include <jz_core/Plane.h>
#include <jz_core/Quaternion.h>

namespace jz
{

    Matrix4 Matrix4::Invert(const Matrix4& m)
    {
        const float m00 = m(0, 0);
        const float m01 = m(0, 1);
        const float m02 = m(0, 2);
        const float m03 = m(0, 3);
        const float m10 = m(1, 0);
        const float m11 = m(1, 1);
        const float m12 = m(1, 2);
        const float m13 = m(1, 3);
        const float m20 = m(2, 0);
        const float m21 = m(2, 1);
        const float m22 = m(2, 2);
        const float m23 = m(2, 3);
        const float m30 = m(3, 0);
        const float m31 = m(3, 1);
        const float m32 = m(3, 2);
        const float m33 = m(3, 3);

        float p0 = (m20 * m31) - (m21 * m30);
        float p1 = (m20 * m32) - (m22 * m30);
        float p2 = (m20 * m33) - (m23 * m30);
        float p3 = (m21 * m32) - (m22 * m31);
        float p4 = (m21 * m33) - (m23 * m31);
        float p5 = (m22 * m33) - (m23 * m32);

        const float t00 =  ((p5 * m11) - (p4 * m12) + (p3 * m13));
        const float t10 = -((p5 * m10) - (p2 * m12) + (p1 * m13));
        const float t20 =  ((p4 * m10) - (p2 * m11) + (p0 * m13));
        const float t30 = -((p3 * m10) - (p1 * m11) + (p0 * m12));

        const float invDet = 1.0f / ((t00 * m00) + (t10 * m01) + (t20 * m02) + (t30 * m03));

        const float d00 = (t00 * invDet);
        const float d10 = (t10 * invDet);
        const float d20 = (t20 * invDet);
        const float d30 = (t30 * invDet);

        const float d01 = -((p5 * m01) - (p4 * m02) + (p3 * m03)) * invDet;
        const float d11 =  ((p5 * m00) - (p2 * m02) + (p1 * m03)) * invDet;
        const float d21 = -((p4 * m00) - (p2 * m01) + (p0 * m03)) * invDet;
        const float d31 =  ((p3 * m00) - (p1 * m01) + (p0 * m02)) * invDet;

        p0 = (m10 * m31) - (m11 * m30);
        p1 = (m10 * m32) - (m12 * m30);
        p2 = (m10 * m33) - (m13 * m30);
        p3 = (m11 * m32) - (m12 * m31);
        p4 = (m11 * m33) - (m13 * m31);
        p5 = (m12 * m33) - (m13 * m32);

        const float d02 =  ((p5 * m01) - (p4 * m02) + (p3 * m03)) * invDet;
        const float d12 = -((p5 * m00) - (p2 * m02) + (p1 * m03)) * invDet;
        const float d22 =  ((p4 * m00) - (p2 * m01) + (p0 * m03)) * invDet;
        const float d32 = -((p3 * m00) - (p1 * m01) + (p0 * m02)) * invDet;

        p0 = (m21 * m10) - (m20 * m11);
        p1 = (m22 * m10) - (m20 * m12);
        p2 = (m23 * m10) - (m20 * m13);
        p3 = (m22 * m11) - (m21 * m12);
        p4 = (m23 * m11) - (m21 * m13);
        p5 = (m23 * m12) - (m22 * m13);

        const float d03 = -((p5 * m01) - (p4 * m02) + (p3 * m03)) * invDet;
        const float d13 =  ((p5 * m00) - (p2 * m02) + (p1 * m03)) * invDet;
        const float d23 = -((p4 * m00) - (p2 * m01) + (p0 * m03)) * invDet;
        const float d33 =  ((p3 * m00) - (p1 * m01) + (p0 * m02)) * invDet;

        return Matrix4(d00, d01, d02, d03,
                       d10, d11, d12, d13,
                       d20, d21, d22, d23,
                       d30, d31, d32, d33);
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
        Matrix4 ret = Matrix4::Transpose(Matrix4::Invert(m));

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

    // DC = (n.x * v.x + n.y * v.y + n.z * v.z + [1.0|0.0] * d)
    // ret = (v - (2.0f * DC * n))

    // ret.x = v.x - (2.0 * DC * n.x)
    // ret.y = v.y - (2.0 * DC * n.y)
    // ret.z = v.z - (2.0 * DC * n.z)

    // ret.x = v.x - ((2.0 * n.x) * (n.x * v.x + n.y * v.y + n.z * v.z + [1.0|0.0] * d))
    // ret.y = v.y - ((2.0 * n.y) * (n.x * v.x + n.y * v.y + n.z * v.z + [1.0|0.0] * d))
    // ret.z = v.z - ((2.0 * n.z) * (n.x * v.x + n.y * v.y + n.z * v.z + [1.0|0.0] * d))

    // ret.x = v.x - ((2.0 * n.x * n.x * v.x) +
    //                (2.0 * n.x * n.y * v.y) +
    //                (2.0 * n.x * n.z * v.z) +
    //                (2.0 * n.x * [1|0] * d))
    // ret.y = v.y - ((2.0 * n.y * n.x * v.x) +
    //                (2.0 * n.y * n.y * v.y) +
    //                (2.0 * n.y * n.z * v.z) +
    //                (2.0 * n.y * [1|0] * d))
    // ret.z = v.z - ((2.0 * n.z * n.x * v.x) +
    //                (2.0 * n.z * n.y * v.y) +
    //                (2.0 * n.z * n.z * v.z) +
    //                (2.0 * n.z * [1|0] * d))

    // ret.x = v.x - (2.0 * n.x * n.x * v.x) -
    //               (2.0 * n.x * n.y * v.y) -
    //               (2.0 * n.x * n.z * v.z) -
    //               (2.0 * n.x * [1|0] * d)
    // ret.y = v.y - (2.0 * n.y * n.x * v.x) -
    //               (2.0 * n.y * n.y * v.y) -
    //               (2.0 * n.y * n.z * v.z) -
    //               (2.0 * n.y * [1|0] * d)
    // ret.z = v.z - (2.0 * n.z * n.x * v.x) -
    //               (2.0 * n.z * n.y * v.y) -
    //               (2.0 * n.z * n.z * v.z) -
    //               (2.0 * n.z * [1|0] * d)

    // ret.x = v.x * (1 - (2 * n.x * n.x)) -
    //               (2.0 * n.x * n.y * v.y) -
    //               (2.0 * n.x * n.z * v.z) -
    //               (2.0 * n.x * [1|0] * d)
    // ret.y =     - (2.0 * n.y * n.x * v.x) -
    //         v.y * (1 - (2 * n.y * n.y)) -
    //               (2.0 * n.y * n.z * v.z) -
    //               (2.0 * n.y * [1|0] * d)
    // ret.z =     - (2.0 * n.z * n.x * v.x) -
    //               (2.0 * n.z * n.y * v.y) -
    //         v.z * (1 - (2 * n.z * n.z)) -
    //               (2.0 * n.z * [1|0] * d)

    // Transform = 
    //     [1 - (2 * n.x * n.x)] [  - (2 * n.y * n.x)] [  - (2 * n.z * n.x)] [0]
    //     [  - (2 * n.x * n.y)] [1 - (2 * n.y * n.y)] [  - (2 * n.z * n.y)] [0]
    //     [  - (2 * n.x * n.z)] [  - (2 * n.y * n.z)] [1 - (2 * n.z * n.z)] [0]
    //     [  - (2 * n.x * d)  ] [  - (2 * n.y * d)  ] [  - (2 * n.z * d)  ] [1]
    Matrix4 Matrix4::CreateReflection(const Plane& p)
    {
        float d = p.GetD();
        const Vector3& n = p.GetNormal();

        Matrix4 ret;
        ret.M11 = 1.0f - (2.0f * n.X * n.X); ret.M12 =      - (2.0f * n.Y * n.X); ret.M13 =      - (2.0f * n.Z * n.X); ret.M14 = 0.0f;
        ret.M21 =      - (2.0f * n.X * n.Y); ret.M22 = 1.0f - (2.0f * n.Y * n.Y); ret.M23 =      - (2.0f * n.Z * n.Y); ret.M24 = 0.0f;
        ret.M31 =      - (2.0f * n.X * n.Z); ret.M32 =      - (2.0f * n.Y * n.Z); ret.M33 = 1.0f - (2.0f * n.Z * n.Z); ret.M34 = 0.0f;
        ret.M41 =      - (2.0f * n.X * d);   ret.M42 =      - (2.0f * n.Y * d);   ret.M43 =      - (2.0f * n.Z * d);   ret.M44 = 1.0f;

        return ret;
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