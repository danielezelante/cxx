// YAL zeldan


#include <cxu/cxu.hxx>
#include <cxu/mymacros.hxx>


using namespace std;
using namespace cxu;

class TestMisc : public Test
{
public:

    TestMisc()
    {
    }

    void vrun() override
    {
        minimal_0000();
        check_nan_0000();
        users_nan_0000();
    }


protected:

    void minimal_0000()
    {
        Minimal<double> m;
        m |= 3;
        m |= -5;
        m |= 7;
        CHECKREAL(-5, m);
    }

    void users_nan_0000()
    {
        printf("# ");
        double x = std::numeric_limits<double>::quiet_NaN();
        const unsigned char * cx = reinterpret_cast<const unsigned char *> (&x);
        CXU_LOOP(j, sizeof (x))
        printf("%02X ", static_cast<unsigned> (cx[j]));
        puts("");
    }

    void check_nan_0000()
    {
        const double n = std::numeric_limits<double>::quiet_NaN();
        CHECKBOOL(false, ::finite(n));
        CHECKBOOL(false, ::finite(-n));

    }


};

static TestMisc s_test;

Test * test_misc()
{
    return &s_test;
}


//.
