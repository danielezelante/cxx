// YAL zeldan

#include <sys/stat.h>
#include <sys/types.h>

#include <unistd.h>

#include <cxm/cxm.hxx>
#include <cxu/mymacros.hxx>
#include <cxm/mymacros.hxx>


using namespace std;
using namespace cxu;
using namespace cxm;

class TestDXF : public Test
{
public:

    TestDXF()
    {
    }

    void vrun() override
    {
        user_dxf_0000();
        user_dxf_0001();
    }

    void user_dxf_0000()
    {
        const string filename =
                Format("/tmp/%$-0000.dxf") % utod(getpid());
        FileMappingBuffer fmb(filename.c_str(), true, 64 * 1024);
        TextOutputBuffer file(fmb);
        Dxf dxf(file);
    }

    void user_dxf_0001()
    {
        const string filename =
                Format("/tmp/%$-0001.dxf") % utod(getpid());
        FileMappingBuffer fmb(filename.c_str(), true, 64 * 1024);
        TextOutputBuffer file(fmb);
        Dxf dxf(file);
        Point2D p(1.0 / 3.0, 1.234E70);
        dxf.draw(p);
    }


};


static TestDXF s_test;

Test * test_dxf()
{
    return &s_test;
}


//.
