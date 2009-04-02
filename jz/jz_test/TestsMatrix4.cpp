#include <jz_core/Matrix4.h>
#include <jz_test/Tests.h>

namespace tut
{
    DUMMY(TestsMatrix4);

    using namespace jz;

    template<> template<>
    void Object::test<1>()
    {
        Matrix4 m;
        ensure_equals(m.M11, 0); ensure_equals(m.M12, 0); ensure_equals(m.M13, 0); ensure_equals(m.M14, 0);
        ensure_equals(m.M21, 0); ensure_equals(m.M22, 0); ensure_equals(m.M23, 0); ensure_equals(m.M24, 0);
        ensure_equals(m.M31, 0); ensure_equals(m.M32, 0); ensure_equals(m.M33, 0); ensure_equals(m.M34, 0);
        ensure_equals(m.M41, 0); ensure_equals(m.M42, 0); ensure_equals(m.M43, 0); ensure_equals(m.M44, 0);
    }

    template<> template<>
    void Object::test<2>()
    {
        float f = UniformRandomf();

        Matrix4 m(f);
        ensure_equals(m.M11, f); ensure_equals(m.M12, f); ensure_equals(m.M13, f); ensure_equals(m.M14, f);
        ensure_equals(m.M21, f); ensure_equals(m.M22, f); ensure_equals(m.M23, f); ensure_equals(m.M24, f);
        ensure_equals(m.M31, f); ensure_equals(m.M32, f); ensure_equals(m.M33, f); ensure_equals(m.M34, f);
        ensure_equals(m.M41, f); ensure_equals(m.M42, f); ensure_equals(m.M43, f); ensure_equals(m.M44, f);
    }

    template<> template<>
    void Object::test<3>()
    {
        float m11 = UniformRandomf(); float m12 = UniformRandomf(); float m13 = UniformRandomf(); float m14 = UniformRandomf();
        float m21 = UniformRandomf(); float m22 = UniformRandomf(); float m23 = UniformRandomf(); float m24 = UniformRandomf();
        float m31 = UniformRandomf(); float m32 = UniformRandomf(); float m33 = UniformRandomf(); float m34 = UniformRandomf();
        float m41 = UniformRandomf(); float m42 = UniformRandomf(); float m43 = UniformRandomf(); float m44 = UniformRandomf();

        Matrix4 m(m11, m12, m13, m14,
                  m21, m22, m23, m24,
                  m31, m32, m33, m34,
                  m41, m42, m43, m44);

        ensure_equals(m.M11, m11); ensure_equals(m.M12, m12); ensure_equals(m.M13, m13); ensure_equals(m.M14, m14);
        ensure_equals(m.M21, m21); ensure_equals(m.M22, m22); ensure_equals(m.M23, m23); ensure_equals(m.M24, m24);
        ensure_equals(m.M31, m31); ensure_equals(m.M32, m32); ensure_equals(m.M33, m33); ensure_equals(m.M34, m34);
        ensure_equals(m.M41, m41); ensure_equals(m.M42, m42); ensure_equals(m.M43, m43); ensure_equals(m.M44, m44);
    }

    template<> template<>
    void Object::test<4>()
    {
        float m11 = UniformRandomf(); float m12 = UniformRandomf(); float m13 = UniformRandomf(); float m14 = UniformRandomf();
        float m21 = UniformRandomf(); float m22 = UniformRandomf(); float m23 = UniformRandomf(); float m24 = UniformRandomf();
        float m31 = UniformRandomf(); float m32 = UniformRandomf(); float m33 = UniformRandomf(); float m34 = UniformRandomf();
        float m41 = UniformRandomf(); float m42 = UniformRandomf(); float m43 = UniformRandomf(); float m44 = UniformRandomf();

        Matrix4 n(m11, m12, m13, m14,
                  m21, m22, m23, m24,
                  m31, m32, m33, m34,
                  m41, m42, m43, m44);
        Matrix4 m(n);

        ensure_equals(m.M11, m11); ensure_equals(m.M12, m12); ensure_equals(m.M13, m13); ensure_equals(m.M14, m14);
        ensure_equals(m.M21, m21); ensure_equals(m.M22, m22); ensure_equals(m.M23, m23); ensure_equals(m.M24, m24);
        ensure_equals(m.M31, m31); ensure_equals(m.M32, m32); ensure_equals(m.M33, m33); ensure_equals(m.M34, m34);
        ensure_equals(m.M41, m41); ensure_equals(m.M42, m42); ensure_equals(m.M43, m43); ensure_equals(m.M44, m44);
    }

    template<> template<>
    void Object::test<5>()
    {
        float m11 = UniformRandomf(); float m12 = UniformRandomf(); float m13 = UniformRandomf(); float m14 = UniformRandomf();
        float m21 = UniformRandomf(); float m22 = UniformRandomf(); float m23 = UniformRandomf(); float m24 = UniformRandomf();
        float m31 = UniformRandomf(); float m32 = UniformRandomf(); float m33 = UniformRandomf(); float m34 = UniformRandomf();
        float m41 = UniformRandomf(); float m42 = UniformRandomf(); float m43 = UniformRandomf(); float m44 = UniformRandomf();

        Matrix4 n(m11, m12, m13, m14,
                  m21, m22, m23, m24,
                  m31, m32, m33, m34,
                  m41, m42, m43, m44);
        Matrix4 m = n;

        ensure_equals(m.M11, m11); ensure_equals(m.M12, m12); ensure_equals(m.M13, m13); ensure_equals(m.M14, m14);
        ensure_equals(m.M21, m21); ensure_equals(m.M22, m22); ensure_equals(m.M23, m23); ensure_equals(m.M24, m24);
        ensure_equals(m.M31, m31); ensure_equals(m.M32, m32); ensure_equals(m.M33, m33); ensure_equals(m.M34, m34);
        ensure_equals(m.M41, m41); ensure_equals(m.M42, m42); ensure_equals(m.M43, m43); ensure_equals(m.M44, m44);

    }

    template<> template<>
    void Object::test<6>()
    {
        float m11 = UniformRandomf(); float m12 = UniformRandomf(); float m13 = UniformRandomf(); float m14 = UniformRandomf();
        float m21 = UniformRandomf(); float m22 = UniformRandomf(); float m23 = UniformRandomf(); float m24 = UniformRandomf();
        float m31 = UniformRandomf(); float m32 = UniformRandomf(); float m33 = UniformRandomf(); float m34 = UniformRandomf();
        float m41 = UniformRandomf(); float m42 = UniformRandomf(); float m43 = UniformRandomf(); float m44 = UniformRandomf();

        Matrix4 m(m11, m12, m13, m14,
                  m21, m22, m23, m24,
                  m31, m32, m33, m34,
                  m41, m42, m43, m44);
        Matrix4 n(m11, m12, m13, m14,
                  m21, m22, m23, m24,
                  m31, m32, m33, m34,
                  m41, m42, m43, m44);

        ensure(m == n);
    }

    template<> template<>
    void Object::test<7>()
    {
        float m11 = UniformRandomf(); float m12 = UniformRandomf(); float m13 = UniformRandomf(); float m14 = UniformRandomf();
        float m21 = UniformRandomf(); float m22 = UniformRandomf(); float m23 = UniformRandomf(); float m24 = UniformRandomf();
        float m31 = UniformRandomf(); float m32 = UniformRandomf(); float m33 = UniformRandomf(); float m34 = UniformRandomf();
        float m41 = UniformRandomf(); float m42 = UniformRandomf(); float m43 = UniformRandomf(); float m44 = UniformRandomf();

        Matrix4 m(m11, m12, m13, m14,
                  m21, m22, m23, m24,
                  m31, m32, m33, m34,
                  m41, m42, m43, m44);

        ensure_equals(m[0], m11); ensure_equals(m[4], m21); ensure_equals(m[8],  m31); ensure_equals(m[12], m41);
        ensure_equals(m[1], m12); ensure_equals(m[5], m22); ensure_equals(m[9],  m32); ensure_equals(m[13], m42);
        ensure_equals(m[2], m13); ensure_equals(m[6], m23); ensure_equals(m[10], m33); ensure_equals(m[14], m43);
        ensure_equals(m[3], m14); ensure_equals(m[7], m24); ensure_equals(m[11], m34); ensure_equals(m[15], m44);

        m11 = UniformRandomf(); m12 = UniformRandomf(); m13 = UniformRandomf(); m14 = UniformRandomf();
        m21 = UniformRandomf(); m22 = UniformRandomf(); m23 = UniformRandomf(); m24 = UniformRandomf();
        m31 = UniformRandomf(); m32 = UniformRandomf(); m33 = UniformRandomf(); m34 = UniformRandomf();
        m41 = UniformRandomf(); m42 = UniformRandomf(); m43 = UniformRandomf(); m44 = UniformRandomf();

        m[0] = m11; m[4] = m21; m[8] =  m31; m[12] = m41;
        m[1] = m12; m[5] = m22; m[9] =  m32; m[13] = m42;
        m[2] = m13; m[6] = m23; m[10] = m33; m[14] = m43;
        m[3] = m14; m[7] = m24; m[11] = m34; m[15] = m44;

        ensure_equals(m[0], m11); ensure_equals(m[4], m21); ensure_equals(m[8],  m31); ensure_equals(m[12], m41);
        ensure_equals(m[1], m12); ensure_equals(m[5], m22); ensure_equals(m[9],  m32); ensure_equals(m[13], m42);
        ensure_equals(m[2], m13); ensure_equals(m[6], m23); ensure_equals(m[10], m33); ensure_equals(m[14], m43);
        ensure_equals(m[3], m14); ensure_equals(m[7], m24); ensure_equals(m[11], m34); ensure_equals(m[15], m44);
    }

    template<> template<>
    void Object::test<8>()
    {
        float m11 = UniformRandomf(); float m12 = UniformRandomf(); float m13 = UniformRandomf(); float m14 = UniformRandomf();
        float m21 = UniformRandomf(); float m22 = UniformRandomf(); float m23 = UniformRandomf(); float m24 = UniformRandomf();
        float m31 = UniformRandomf(); float m32 = UniformRandomf(); float m33 = UniformRandomf(); float m34 = UniformRandomf();
        float m41 = UniformRandomf(); float m42 = UniformRandomf(); float m43 = UniformRandomf(); float m44 = UniformRandomf();

        Matrix4 m(m11, m12, m13, m14,
                  m21, m22, m23, m24,
                  m31, m32, m33, m34,
                  m41, m42, m43, m44);

        ensure_equals(m(0,0), m11); ensure_equals(m(0,1), m12); ensure_equals(m(0,2), m13); ensure_equals(m(0,3), m14);
        ensure_equals(m(1,0), m21); ensure_equals(m(1,1), m22); ensure_equals(m(1,2), m23); ensure_equals(m(1,3), m24);
        ensure_equals(m(2,0), m31); ensure_equals(m(2,1), m32); ensure_equals(m(2,2), m33); ensure_equals(m(2,3), m34);
        ensure_equals(m(3,0), m41); ensure_equals(m(3,1), m42); ensure_equals(m(3,2), m43); ensure_equals(m(3,3), m44);

        m11 = UniformRandomf(); m12 = UniformRandomf(); m13 = UniformRandomf(); m14 = UniformRandomf();
        m21 = UniformRandomf(); m22 = UniformRandomf(); m23 = UniformRandomf(); m24 = UniformRandomf();
        m31 = UniformRandomf(); m32 = UniformRandomf(); m33 = UniformRandomf(); m34 = UniformRandomf();
        m41 = UniformRandomf(); m42 = UniformRandomf(); m43 = UniformRandomf(); m44 = UniformRandomf();

        m(0,0) = m11; m(0,1) = m12; m(0,2) = m13; m(0,3) = m14;
        m(1,0) = m21; m(1,1) = m22; m(1,2) = m23; m(1,3) = m24;
        m(2,0) = m31; m(2,1) = m32; m(2,2) = m33; m(2,3) = m34;
        m(3,0) = m41; m(3,1) = m42; m(3,2) = m43; m(3,3) = m44;

        ensure_equals(m(0,0), m11); ensure_equals(m(0,1), m12); ensure_equals(m(0,2), m13); ensure_equals(m(0,3), m14);
        ensure_equals(m(1,0), m21); ensure_equals(m(1,1), m22); ensure_equals(m(1,2), m23); ensure_equals(m(1,3), m24);
        ensure_equals(m(2,0), m31); ensure_equals(m(2,1), m32); ensure_equals(m(2,2), m33); ensure_equals(m(2,3), m34);
        ensure_equals(m(3,0), m41); ensure_equals(m(3,1), m42); ensure_equals(m(3,2), m43); ensure_equals(m(3,3), m44);
    }
}
