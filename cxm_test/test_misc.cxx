// YAL zeldan


#include <cxm/cxm.hxx>
#include <cxu/mymacros.hxx>
#include <cxm/mymacros.hxx>


using namespace std;
using namespace cxu;
using namespace cxm;

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
        vad_cos();
        vad_inv();
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

    void vad_cos()
    {
        {
            const VaD x = VaD::vd(0, 0.01);
            const VaD y = cos(x);
            const string s = Format("cos(%$) = %$") % x % y;
            printf("# %s \n", s.c_str());
        }
        {
            const VaD x = VaD::vd(M_PI / 2, 0.01);
            const VaD y = cos(x);
            const string s = Format("cos(%$) = %$") % x % y;
            printf("# %s \n", s.c_str());
        }

    }

    void vad_inv()
    {
        const VaD x = VaD::vd(13, 0.1);
        const VaD y = ~x;
        const VaD p = x*y;

        const string s = Format("x=%$ y=1/x=%$ x*y=%$") % x % y % p;
        printf("# %s \n", s.c_str());
    }

};

static TestMisc s_test;

Test * test_misc()
{
    return &s_test;
}


//.
