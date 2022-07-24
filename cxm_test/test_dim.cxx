// YAL zeldan

#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>

#include <cxm/cxm.hxx>
#include <cxu/mymacros.hxx>
#include <cxm/mymacros.hxx>



using namespace std;
using namespace cxu;
using namespace cxm;

class TestDim : public Test
{
public:

    TestDim()
    {
    }

    void vrun() override
    {
        user_dim_0000();
    }

    void user_dim_0000()
    {
        CXU_TESTINFO("begin");
        CXU_TESTINFO("Length");
        const dim::Length l1 = VaD::vd(1, 0.01) * dim::m;
        CXU_TESTINFO("Time");
        const dim::Time t1 = VaD::vd(1, 0.1) * dim::s;

        CXU_TESTINFO("Speed");
        const dim::Speed v1 = l1 / t1;

        CXU_TESTINFO("format");
        cout << string(cxu::Format("# speed = %$") % v1) << endl;

        const VaD n1 = VaD::vd(1.23456789, 0.0001);
        cout << string(cxu::Format("# num = %$") % n1) << endl;

        const dim::Force gforce = VaD::vd(9.8, 0.1) * dim::kg * dim::m / (dim::s * dim::s);
        cout << string(cxu::Format("# gforce = %$") % gforce) << endl;


    }


};


static TestDim s_test;

Test * test_dim()
{
    return &s_test;
}


//.
