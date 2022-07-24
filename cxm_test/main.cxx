// YAL zeldan

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cxm/cxm.hxx>

using namespace std;
using namespace cxu;
using namespace cxm;

#include "cxm_test.hxx"

#include "test_linear2d.hxx"
#include "test_linear3d.hxx"
#include "test_conic2d.hxx"
#include "test_misc.hxx"
#include "test_dim.hxx"
#include "test_dxf.hxx"

int mymain(int argc, char ** argv)
{
    CXU_LOGGER.addout(stderr, Logger::e_debug);

    CXU_LOGGER.usesyslog(Logger::e_debug);

    CXU_TRACE("mymain");

    const char * tmpdir = "/tmp/cxm_test";

    LibInfo::printList();
    CXU_LOGGER.info(Format("# argc=%$ argv[0]=%$") % argc % quote(argv[0]));

    if (-1 == mkdir(tmpdir, 0777))
        if (errno != EEXIST)
            CXU_THROWERREXCEPTIONHERE();

    TestSuite ts(tmpdir);
    ts.add(test_linear2d());
    ts.add(test_conic2d());
    ts.add(test_linear3d());
    ts.add(test_dim());
    ts.add(test_dxf());
    ts.add(test_misc());
    return ts.run();
}



CXU_MAIN(mymain)



//.
