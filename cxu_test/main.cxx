// YAL zeldan

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cxu/cxu.hxx>

using namespace cxu;

#include "cxu_test.hxx"

#include "test_bstorage.hxx"
#include "test_sync.hxx"
#include "test_time.hxx"
#include "test_string.hxx"
#include "test_misc.hxx"
#include "test_process.hxx"
#include "test_demo.hxx"
#include "test_misc.hxx"

int mymain(int argc, char ** argv)
{
    CXU_LOGGER.addout(stderr, Logger::e_debug);

    CXU_LOGGER.usesyslog(Logger::e_debug);

    CXU_TRACE("mymain");

    const char * tmpdir = "/tmp/cxu_test";

    LibInfo::printList();
    CXU_LOGGER.info(Format("# argc=%$ argv[0]=%$") % argc % quote(argv[0]));

    if (-1 == mkdir(tmpdir, 0777))
        if (errno != EEXIST)
            CXU_THROWERREXCEPTIONHERE();

    TestSuite ts(tmpdir);
    ts.add(test_bstorage());
    ts.add(test_sync());
    ts.add(test_time());
    ts.add(test_string());
    ts.add(test_process());
    ts.add(test_misc());

    ts.add(test_misc());

    ts.add(test_demo());

    return ts.run();
}



CXU_MAIN(mymain)



//.
