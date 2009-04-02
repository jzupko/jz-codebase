#ifndef _TESTS_H_
#define _TESTS_H_

#include <tut_framework/tut.h>
#include <tut_framework/tut_reporter.h>

#define DUMMY(name)                           \
    struct name##DummyStruct {};              \
    typedef test_group<name##DummyStruct> TG; \
    typedef TG::object        Object;         \
    TG name(#name)

#endif