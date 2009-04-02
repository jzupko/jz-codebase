#include <jz_core/Matrix.h>
#include <jz_test/Tests.h>

namespace tut
{
    DUMMY(TestsMatrix);

    using namespace jz;

    template<> template<>
    void Object::test<1>()
    {
        Matrix m;

        ensure_equals(m.size(), 0);
        m.resize(3, 3);
        ensure_equals(m.size(), 9);
        ensure_equals(m.GetCols(), 3);
        ensure_equals(m.GetRows(), 3);
    }

    template<> template<>
    void Object::test<2>()
    {
        Matrix m(1,  0, 0,
                 0,  1, 0,
                 0,  0, 1);
        Matrix n(2,  1, 4,
                 5, -4, 2,
                 0,  2, 5);
        Matrix o;

        ensure_equals(m(0,0), 1); ensure_equals(m(0,1), 0); ensure_equals(m(0,2), 0);
        ensure_equals(m(1,0), 0); ensure_equals(m(1,1), 1); ensure_equals(m(1,2), 0);
        ensure_equals(m(2,0), 0); ensure_equals(m(2,1), 0); ensure_equals(m(2,2), 1);
        ensure_equals(m.GetRows(), 3);
        ensure_equals(m.GetCols(), 3);
        ensure_equals(m.size(), 9);

        o = m + n;
        ensure_equals(o(0,0), 3); ensure_equals(o(0,1),  1); ensure_equals(o(0,2),  4);
        ensure_equals(o(1,0), 5); ensure_equals(o(1,1), -3); ensure_equals(o(1,2),  2);
        ensure_equals(o(2,0), 0); ensure_equals(o(2,1),  2); ensure_equals(o(2,2),  6);
        ensure_equals(o.GetRows(), 3);
        ensure_equals(o.GetCols(), 3);
        ensure_equals(o.size(), 9);

        o = m - n;
        ensure_equals(o(0,0),-1); ensure_equals(o(0,1), -1); ensure_equals(o(0,2), -4);
        ensure_equals(o(1,0),-5); ensure_equals(o(1,1),  5); ensure_equals(o(1,2), -2);
        ensure_equals(o(2,0), 0); ensure_equals(o(2,1), -2); ensure_equals(o(2,2), -4);
        ensure_equals(o.GetRows(), 3);
        ensure_equals(o.GetCols(), 3);
        ensure_equals(o.size(), 9);

        o = -n;
        ensure_equals(o(0,0), -2); ensure_equals(o(0,1),  -1); ensure_equals(o(0,2), -4);
        ensure_equals(o(1,0), -5); ensure_equals(o(1,1), 4); ensure_equals(o(1,2), -2);
        ensure_equals(o(2,0), 0); ensure_equals(o(2,1),  -2); ensure_equals(o(2,2), -5);
        ensure_equals(o.GetRows(), 3);
        ensure_equals(o.GetCols(), 3);
        ensure_equals(o.size(), 9);

        o = m * n;
        ensure_equals(o(0,0), 2); ensure_equals(o(0,1),  1); ensure_equals(o(0,2), 4);
        ensure_equals(o(1,0), 5); ensure_equals(o(1,1), -4); ensure_equals(o(1,2), 2);
        ensure_equals(o(2,0), 0); ensure_equals(o(2,1),  2); ensure_equals(o(2,2), 5);
        ensure_equals(o.GetRows(), 3);
        ensure_equals(o.GetCols(), 3);
        ensure_equals(o.size(), 9);

        o = Abs(n);
        ensure_equals(o(0,0), 2); ensure_equals(o(0,1),  1); ensure_equals(o(0,2), 4);
        ensure_equals(o(1,0), 5); ensure_equals(o(1,1),  4); ensure_equals(o(1,2), 2);
        ensure_equals(o(2,0), 0); ensure_equals(o(2,1),  2); ensure_equals(o(2,2), 5);
        ensure_equals(o.GetRows(), 3);
        ensure_equals(o.GetCols(), 3);
        ensure_equals(o.size(), 9);
    }

}
