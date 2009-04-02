#include <jz_core/Vector3.h>
#include <jz_test/Tests.h>

namespace tut
{
    DUMMY(TestsVector3);

    using namespace jz;

    template<> template<>
    void Object::test<1>()
    {
        Vector3 v;
        ensure_equals(v.X, 0);
        ensure_equals(v.Y, 0);
        ensure_equals(v.Z, 0);
    }

    template<> template<>
    void Object::test<2>()
    {
        float f = UniformRandomf();
        Vector3 v(f);
        ensure_equals(v.X, f);
        ensure_equals(v.Y, f);
        ensure_equals(v.Z, f);
    }

    template<> template<>
    void Object::test<3>()
    {
        float x = UniformRandomf();
        float y = UniformRandomf();
        float z = UniformRandomf();

        Vector3 v(x, y, z);
        ensure_equals(v.X, x);
        ensure_equals(v.Y, y);
        ensure_equals(v.Z, z);
    }

    template<> template<>
    void Object::test<4>()
    {
        float x = UniformRandomf();
        float y = UniformRandomf();
        float z = UniformRandomf();

        Vector3 v(x, y, z);
        Vector3 u(v);

        ensure_equals(u.X, x);
        ensure_equals(u.Y, y);
        ensure_equals(u.Z, z);
    }

    template<> template<>
    void Object::test<5>()
    {
        float x = UniformRandomf();
        float y = UniformRandomf();
        float z = UniformRandomf();

        Vector3 v(x, y, z);
        Vector3 u;

        u = v;

        ensure_equals(u.X, x);
        ensure_equals(u.Y, y);
        ensure_equals(u.Z, z);
    }

    template<> template<>
    void Object::test<6>()
    {
        float x = UniformRandomf();
        float y = UniformRandomf();
        float z = UniformRandomf();

        Vector3 u(x, y, z);

        ensure_equals(u[0], x);
        ensure_equals(u[1], y);
        ensure_equals(u[2], z);

        x = UniformRandomf();
        y = UniformRandomf();
        z = UniformRandomf();

        u[0] = x;
        u[1] = y;
        u[2] = z;

        ensure_equals(u.X, x);
        ensure_equals(u.Y, y);
        ensure_equals(u.Z, z);
    }

    template<> template<>
    void Object::test<7>()
    {
        float x = UniformRandomf();
        float y = UniformRandomf();
        float z = UniformRandomf();

        Vector3 u(x, y, z);
        Vector3 v(x, y, z);

        ensure(u == v);
    }

    template<> template<>
    void Object::test<8>()
    {
        Vector3 u(125, 456, -120);
        Vector3 v(125, -5328, 823);

        ensure(u != v);
    }

    template<> template<>
    void Object::test<9>()
    {
        Vector3 u(125, 456, -120);
        Vector3 v(777, -5328, 823);
        Vector3 uv(125 - 777, 456 - -5328, -120 - 823);

        ensure((u - v)  == uv);
        ensure((u -= v) == uv);
    }

    template<> template<>
    void Object::test<10>()
    {
        Vector3 u(125, 456, -120);
        Vector3 uv(125 / 0.25f, 456 / 0.25f, -120 / 0.25f);

        ensure((u / 0.25f)  == uv);
        ensure((u /= 0.25f) == uv);
    }

    template<> template<>
    void Object::test<11>()
    {
        Vector3 u(125, 456, -120);
        Vector3 v(777, -5328, 823);
        Vector3 uv(125 / 777.0f, 456 / -5328.0f, -120 / 823.0f);

        ensure((u /  v) == uv);
        ensure((u /= v) == uv);
    }

    template<> template<>
    void Object::test<12>()
    {
        Vector3 u(125, 456, -120);
        Vector3 uv(125 * 0.25f, 456 * 0.25f, -120 * 0.25f);

        ensure((u *  0.25f) == uv);
        ensure((u *= 0.25f) == uv);
    }

    template<> template<>
    void Object::test<13>()
    {
        Vector3 u(125, 456, -120);
        Vector3 v(777, -5328, 823);
        Vector3 uv(125 * 777.0f, 456 * -5328.0f, -120 * 823.0f);

        ensure((u *  v) == uv);
        ensure((u *= v) == uv);
    }

    template<> template<>
    void Object::test<14>()
    {
        Vector3 u(125, 456, -120);
        Vector3 v(777, -5328, 823);
        Vector3 uv(125 + 777, 456 + -5328, -120 + 823);

        ensure((u +  v) == uv);
        ensure((u += v) == uv);
    }

    template<> template<>
    void Object::test<15>()
    {
        Vector3 u(125, 456, -120);
        Vector3 uv(-125, -456, 120);

        ensure(-u == uv);
    }
}
