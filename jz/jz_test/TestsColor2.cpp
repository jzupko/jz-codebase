#include <jz_core/Color.h>
#include <jz_test/Tests.h>

namespace tut
{
    DUMMY(TestsColor2);

    using namespace jz;

    template<> template<>
    void Object::test<1>()
    {
        //--------
        ColorRGB rgb(0);
        ColorHSV hsv(rgb);
        ensure_equals(hsv.V, float(0));
        //--------
        rgb = ColorRGB::kWhite;
        hsv.SetFromRGB(rgb);
        ensure(rgb == ColorRGB(1));
        ensure_equals(hsv.S, float(0));
        ensure_equals(hsv.V, float(1));
        //--------
        for (natural i = 0; i < 256; i++)
        {
            rgb.Assign(float(i) / float(255));
            hsv.SetFromRGB(rgb);
            ensure_equals(natural(hsv.V * float(255)), i);
            ensure_equals(hsv.S, float(0));
        }
        //--------
    }

    template<> template<>
    void Object::test<2>()
    {
        for (natural i = 0; i < 256; i++)
        {
            float v = GetValue(ColorRGB(float(i) / float(255)));
            ensure_equals(natural(v * float(255)), i);
        }
    }
}