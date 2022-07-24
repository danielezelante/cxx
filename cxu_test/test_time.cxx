// YAL zeldan

#include <stdlib.h>

#include <iostream>
#include <iomanip>


#include <cxu/cxu.hxx>
#include <cxu/mymacros.hxx>

using namespace std;
using namespace cxu;

class TestTime : public Test
{
public:

    TestTime()
    {
    }

    void vrun() override
    {
        delta_units();
        null_time();
        compare(100);
        compare(2500);
        waiter(100);
        waiter(2500);
        sumdiff(100);
        sumdiff(2500);
        fields();
        deltananoeqsum();
        deltaseceqsum();
        deltananosum();
        deltasecsum();
    }



protected:

    void delta_units()
    {
        TESTINFO("comparing equivalent deltas");
        const DeltaTime d1 = DeltaTime::s(1);
        const DeltaTime d2 = DeltaTime::ms(1000);
        const DeltaTime d3 = DeltaTime::us(1000000);

        CHECKBOOL(true, (d1 == d1));
        CHECKBOOL(true, (d1 == d2));
        CHECKBOOL(true, (d1 == d3));

        const DeltaTime d4 = DeltaTime::ns(1000000000);
        CHECKBOOL(true, (d1 == d4));
    }

    void null_time()
    {
        TESTINFO("null / nonnull");
        const Time t0;
        const Time t1 = Time::now();
        CHECKBOOL(false, (t0.is()));
        CHECKBOOL(true, (t1.is()));
        CHECKBOOL(true, (t0 != t1));
        CHECKBOOL(false, (t0 == t1));
    }

    void compare(int dtms)
    {
        TESTINFO(Format("compare %$ ms") % dtms);
        const Time t0 = Time::now();
        TESTINFO(Format("sleep %$ ms.") % dtms);
        Thread::sleepit(DeltaTime::ms(dtms));
        const Time t1 = Time::now();
        CHECKBOOL(true, (t0 != t1));
        CHECKBOOL(false, (t0 == t1));
        CHECKBOOL(true, (t1 > t0));
        CHECKBOOL(false, (t1 < t0));
        CHECKBOOL(true, (t0 < t1));
        CHECKBOOL(false, (t0 > t1));

        DeltaTime dt = t1 - t0;
        CHECKBOOL(true, (dt != DeltaTime()));
        CHECKBOOL(true, (dt > DeltaTime()));
        CHECKBOOL(false, (dt == DeltaTime()));
        CHECKBOOL(false, (dt < DeltaTime()));
    }

    void waiter(int dtms)
    {
        TESTINFO(Format("waiter(%$)") % dtms);
        Notification no;
        Waiter wa(no);
        const DeltaTime dt = DeltaTime::ms(dtms);
        const Time t0 = Time::now();
        const bool x = wa.wait(dt);
        CHECKBOOL(false, x);
        const Time t1 = Time::now();
        const DeltaTime dtk = t1 - t0;
        TESTINFO(Format("dtk=%$; dt=%$") % dtk % dt);
        //CHECKBOOL(true, (dtk >= dt));
        //CHECKBOOL(false, (dtk < dt));
        // TODO here test may fail cause system clock drift to adjust to ntp server
    }

    void sumdiff(int dtms)
    {
        const Time t0 = Time::now();
        Thread::sleepit(DeltaTime::ms(dtms));
        const Time t1 = Time::now();
        const DeltaTime dt = t1 - t0;
        const Time t2 = t0 + dt;
        CHECKBOOL(true, (t1 == t2));
        CHECKBOOL(false, (t1 != t2));
    }

    void fields()
    {
        const int64_t one64 = 1;
        const DeltaTime dt =
                DeltaTime::ns((((((((one64 * 2 * 24) + 3) * 60 + 4) * 60 + 5) * 1000 + 6) * 1000) + 7) * 1000 + 8);

        CHECKINT(2, dt.par_d());
        CHECKINT(3, dt.par_h());
        CHECKINT(4, dt.par_m());
        CHECKINT(5, dt.par_s());
        CHECKINT(6, dt.par_ms());
        CHECKINT(7, dt.par_us());
        CHECKINT(8, dt.par_ns());

        CHECKINT(2, dt.parq_d());
        CHECKINT(3, dt.parq_h());
        CHECKINT(4, dt.parq_m());
        CHECKINT(5, dt.parq_s());
        CHECKINT(6, dt.parq_ms());
        CHECKINT(7, dt.parq_us());
        CHECKINT(8, dt.parq_ns());

    }

    void deltananoeqsum()
    {
        const DeltaTime d0 = DeltaTime::ms(15);
        DeltaTime dx = DeltaTime::ms(20);
        dx += d0;
        CHECKINT(35, dx.tot_ms());
    }

    void deltaseceqsum()
    {
        const DeltaTime d0 = DeltaTime::s(15);
        DeltaTime dx = DeltaTime::s(20);
        dx += d0;
        CHECKINT(35, dx.tot_s());
    }

    void deltananosum()
    {
        const DeltaTime d0 = DeltaTime::ms(15);
        DeltaTime dx = DeltaTime::ms(20);
        dx = dx + d0;
        CHECKINT(35, dx.tot_ms());
    }

    void deltasecsum()
    {
        const DeltaTime d0 = DeltaTime::s(15);
        DeltaTime dx = DeltaTime::s(20);
        dx = dx + d0;
        CHECKINT(35, dx.tot_s());
    }


};


static TestTime s_test_time;

Test * test_time()
{
    return &s_test_time;
}

//.
