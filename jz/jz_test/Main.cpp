// 
// Copyright (C) 2009 Joe Zupko. All Rights Reserved.
//   (jaz147@psu.edu, http://www.personal.psu.edu/~jaz147/)
//

#include <jz_core/Prereqs.h>
#include <jz_test/Tests.h>
#include <cstring>
#include <iostream>
#include <fstream>

namespace jz
{
    static const char* gskpLogFilename = "jz-test.log";
    static const char* gskpErrorLogFilename = "jz-test_error.log";
}
    
int main(int argc, char** argv)
{
    using namespace jz;

#   ifndef NDEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#   endif
 
    std::ofstream ocf(jz::gskpLogFilename);
    std::ofstream oef(jz::gskpErrorLogFilename);

    if (ocf.is_open() && !ocf.fail()) { std::cout.rdbuf(ocf.rdbuf()); }
    if (oef.is_open() && !oef.fail()) { std::cerr.rdbuf(oef.rdbuf()); }

    try
    {
        tut::reporter r;
        tut::runner.get().set_callback(&r);
        tut::runner.get().run_tests();  
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
