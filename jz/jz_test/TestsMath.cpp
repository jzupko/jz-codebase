#include <jz_core/Math.h>
#include <jz_test/Tests.h>

namespace tut
{
    DUMMY(TestsMath);

    using namespace jz;

    template<> template<>
    void Object::test<1>()
    {
        ensure(AboutEqual(0.0f, 0.0f));
        ensure(!AboutEqual(0.0f, Constants<float>::kZeroTolerance));
    }

    template<> template<>
    void Object::test<2>()
    {
        ensure(AboutZero(0.0f));
        ensure(AboutZero(0.0));
    }

    template<> template<>
    void Object::test<3>()
    {
        ensure_equals(Abs(-1), 1);
        ensure_equals(Abs(1), 1);
    }

    template<> template<>
    void Object::test<4>()
    {
        ensure_equals(Clamp(2, -1, 1), 1);
        ensure_equals(Clamp(-2, -1, 1), -1);
    }

    template<> template<>
    void Object::test<5>()
    {
        ensure_equals(Lerp(0.0f, 1.0f, 0.5f), 0.5f);
        ensure_equals(Lerp(0.0, 1.0, 0.5), 0.5);
    }

    template<> template<>
    void Object::test<6>()
    {
        ensure_equals(Max(0, 1), 1);
        ensure_equals(Max(2, 1, 0), 2);
        ensure_equals(Min(0, 1, 2), 0);
        ensure_equals(Min(1, 0), 0);
    }

    template<> template<>
    void Object::test<7>()
    {
        ensure_equals(Sign(-500), -1);
        ensure_equals(Sign(878), 1);
        ensure_equals(Sign(0), 0);
        ensure_equals(Sign(Constants<float>::kZeroTolerance), 1.0f);
        ensure_equals(Sign(Constants<double>::kZeroTolerance), 1.0);
        ensure_equals(Sign(-Constants<float>::kZeroTolerance), -1.0f);
        ensure_equals(Sign(-Constants<double>::kZeroTolerance), -1.0);
    }

}
