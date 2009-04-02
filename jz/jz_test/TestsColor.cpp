#include <jz_core/Color.h>
#include <jz_test/Tests.h>

namespace tut
{

    DUMMY(TestsColor);

    using namespace jz;

    template<> template<>
    void Object::test<1>()
    {
        ColorXYZ v;
        ensure_equals(v.X, 0);
        ensure_equals(v.Y, 0);
        ensure_equals(v.Z, 0);
    }

    template<> template<>
    void Object::test<2>()
    {
        ColorXYZ v(253);
        ensure_equals(v.X, 253);
        ensure_equals(v.Y, 253);
        ensure_equals(v.Z, 253);
    }

    template<> template<>
    void Object::test<3>()
    {
        ColorXYZ v(777, -5328, 823);
        ensure_equals(v.X, 777);
        ensure_equals(v.Y, -5328);
        ensure_equals(v.Z, 823);
    }

    template<> template<>
    void Object::test<4>()
    {
        ColorXYZ v(777, -5328, 823);
        ColorXYZ u(v);

        ensure_equals(u.X, 777);
        ensure_equals(u.Y, -5328);
        ensure_equals(u.Z, 823);
    }

    template<> template<>
    void Object::test<5>()
    {
        ColorXYZ v(777, -5328, 823);
        ColorXYZ u;

        u = v;

        ensure_equals(u.X, 777);
        ensure_equals(u.Y, -5328);
        ensure_equals(u.Z, 823);
    }

    template<> template<>
    void Object::test<6>()
    {
        ColorXYZ v(777, -5328, 823);

        ensure_equals(v[0], 777);
        ensure_equals(v[1], -5328);
        ensure_equals(v[2], 823);

        v[0] = -3847;
        v[1] = 8766;
        v[2] = -246;

        ensure_equals(v.X, -3847);
        ensure_equals(v.Y, 8766);
        ensure_equals(v.Z, -246);
    }

    template<> template<>
    void Object::test<7>()
    {
        ColorXYZ u(777, -5328, 823);
        ColorXYZ v(777, -5328, 823);

        ensure(u == v);
    }

    template<> template<>
    void Object::test<8>()
    {
        ColorXYZ u(125, 456, -120);
        ColorXYZ v(125, -5328, 823);

        ensure(u != v);
    }

    template<> template<>
    void Object::test<9>()
    {
        ColorXYZ u(125, 456, -120);
        ColorXYZ v(777, -5328, 823);
        ColorXYZ uv(125 - 777, 456 - -5328, -120 - 823);

        ensure((u - v)  == uv);
        ensure((u -= v) == uv);
    }

    template<> template<>
    void Object::test<10>()
    {
        ColorXYZ u(125, 456, -120);
        ColorXYZ uv(125 / 0.25f, 456 / 0.25f, -120 / 0.25f);

        ensure((u / 0.25f)  == uv);
        ensure((u /= 0.25f) == uv);
    }

    template<> template<>
    void Object::test<11>()
    {
        ColorXYZ u(125, 456, -120);
        ColorXYZ v(777, -5328, 823);
        ColorXYZ uv(125 / 777.0f, 456 / -5328.0f, -120 / 823.0f);

        ensure((u /  v) == uv);
        ensure((u /= v) == uv);
    }

    template<> template<>
    void Object::test<12>()
    {
        ColorXYZ u(125, 456, -120);
        ColorXYZ uv(125 * 0.25f, 456 * 0.25f, -120 * 0.25f);

        ensure((u *  0.25f) == uv);
        ensure((u *= 0.25f) == uv);
    }

    template<> template<>
    void Object::test<13>()
    {
        ColorXYZ u(125, 456, -120);
        ColorXYZ v(777, -5328, 823);
        ColorXYZ uv(125 * 777.0f, 456 * -5328.0f, -120 * 823.0f);

        ensure((u *  v) == uv);
        ensure((u *= v) == uv);
    }

    template<> template<>
    void Object::test<14>()
    {
        ColorXYZ u(125, 456, -120);
        ColorXYZ v(777, -5328, 823);
        ColorXYZ uv(125 + 777, 456 + -5328, -120 + 823);

        ensure((u +  v) == uv);
        ensure((u += v) == uv);
    }

    template<> template<>
    void Object::test<15>()
    {
        ColorXYZ u(125, 456, -120);
        ColorXYZ uv(-125, -456, 120);

        ensure(-u == uv);
    }
    
}
