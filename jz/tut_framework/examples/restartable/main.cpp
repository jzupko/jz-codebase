#include <iostream>
#include <exception>

#include <tut_restartable.h>
#include <tut_reporter.h>

using std::cerr;
using std::endl;
using std::exception;

using tut::reporter;
using tut::restartable_wrapper;

namespace tut
{
    
test_runner_singleton runner;

}

int main()
{
    cerr << "NB: this application will be terminated by OS four times\n"
        "before you'll get test results, be patient restarting it.\n";

    try
    {
        reporter visi;
        restartable_wrapper restartable;

        restartable.set_callback(&visi);
        restartable.run_tests();
    }
    catch (const exception& ex)
    {
        cerr << "tut raised ex: " << ex.what() << endl;
    }

    return 0;
}


