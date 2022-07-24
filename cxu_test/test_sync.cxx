// YAL zeldan

#include <stdlib.h>

#include <iostream>
#include <iomanip>


#include <cxu/cxu.hxx>
#include <cxu/mymacros.hxx>

using namespace std;
using namespace cxu;

class TestSync : public Test
{
public:

    TestSync()
    {
    }

    void vrun() override
    {
        thread1();
        thread2();
        killzombie();
        threadintr();
        thexmm();
        //threadnotif();
        //threadpipe();
        rrw();
    }



protected:

    class Thread1 : public WorkerThread
    {
    public:

        Thread1() : WorkerThread("Thread1"), _x(0)
        {
        }

        virtual ~Thread1()
        {
        }

        virtual t_retval run()
        {
            puts("#Thread1::run begin");
            ++_x;
            Thread::sleepit(DeltaTime::ms(200));
            puts("#thread1::run end");
            return expectedRV();
        }

        t_retval expectedRV() const
        {
            return reinterpret_cast<t_retval> (77);
        }

        int getX() const
        {
            return _x;
        }

    protected:
        int _x;
    };

    void thread1()
    {
        puts("#thread1");
        Thread1 th;
        puts("#starting");
        th.start();
        puts("#joining");
        th.join();
        puts("#joint");
        Thread::t_retval rv = th.getRV();
        CHECKPTR(th.expectedRV(), rv);
        CHECK(1, th.getX());
    }

    class Thread2 : public WorkerThread
    {
    public:

        Thread2() : WorkerThread("Thread2"), _x(0)
        {
        }

        virtual ~Thread2()
        {
        }

        virtual t_retval run()
        {
            TRACE("Thread2::run");
            ++_x;
            halt();
            TRACE("Thread2::run return");
            return 0;
        }

        int getX() const
        {
            return _x;
        }

    protected:
        int _x;
    };

    void thread2()
    {
        puts("#thread2");
        Thread2 th;
        try
        {
            puts("# created");
            th.start();
            std::string s = Format("# started, id=%$") % threadidtoa(th.getID());
            puts(s.data());
            Thread::sleepit(DeltaTime::s(1));
            puts("# slept");
            th.cancel();
            puts("# cancelling");
            th.join();
            puts("# joint");
        }
        catch (const Exception & ex)
        {
            printf("# exception: %s\n", ex.str().c_str());
            ;
            CHECKBOOL(true, false);
        }

    }

    class SleepThread : public WorkerThread
    {
    public:

        SleepThread(unsigned int ms) : WorkerThread("SleepThread"), _ms(ms)
        {
        }

        virtual ~SleepThread()
        {
        }

        virtual t_retval run()
        {
            Thread::sleepit(DeltaTime::ms(_ms));
            return 0;
        }

    protected:
        unsigned int _ms;
    };

    class EmptyThread : public WorkerThread
    {
    public:

        EmptyThread() : WorkerThread("EmptyThread")
        {
        }

        virtual ~EmptyThread()
        {
        }

        virtual t_retval run()
        {
            return 0;
        }
    };

    void killzombie()
    {
        TESTINFO("begin");
        EmptyThread th;
        th.start();
        Thread::sleepit(DeltaTime::s(1)); // si suppone che th intanto termini
        th.cancel(); //qui non deve tirare eccezione
        try
        {
            th.cancel();
        }
        catch (const ErrException & ex)
        {
            ex.log(Logger::e_info);
        }

        th.join();

        TESTINFO("end");
    }

    class FrulloThread : public WorkerThread
    {
    public:

        FrulloThread() : WorkerThread("FrulloThread")
        {
        }

        virtual ~FrulloThread()
        {
        }

        virtual t_retval run()
        {
            for (;;)
            {
                Thread::sleepit(DeltaTime::ms(1)); // 1ms di frullo
                test();
            }
            return 0;
        }
    };

    void threadintr()
    {
        TESTINFO("begin");
        FrulloThread th;
        th.start();
        Thread::sleepit(DeltaTime::s(1)); // 1s di frullo
        th.cancel();
        th.join();
        TESTINFO("end");
    }

    class ThexmmThread : public WorkerThread
    {
    public:

        ThexmmThread() : WorkerThread("ThexmmThread"), _exc(false)
        {
        }

        virtual ~ThexmmThread()
        {
        }

        virtual t_retval run()
        {
            try
            {
                throw 1;
            }
            catch (int)
            {
                _exc = true;
            }
            return 0;
        }

        bool _exc;
    };

    void thexmm()
    {
        TESTINFO("begin");
        bool wrong = false;
        ThexmmThread th;
        try
        {
            th.start();
            th.join();
        }
        catch (int x)
        {
            TESTINFO("thread exception mismatch");
            wrong = true;
        }
        CHECKBOOL(th._exc, true);
        CHECKBOOL(wrong, false);
        TESTINFO("end");
    }

    class Flipper
    {
    public:

        Flipper(volatile int & x) : _value(x)
        {
            _value = 1;
        }

        virtual ~Flipper()
        {
            _value = 2;
        }

    protected:
        volatile int & _value;
    };

    class ThreadNotif : public WorkerThread
    {
    public:

        ThreadNotif(Notification & nof, volatile bool & flag1, volatile bool & flag2, volatile int & x)
        : WorkerThread("ThreadNotif")
        , _nof(nof), _flag1(flag1), _flag2(flag2), _value(x)
        {
        }

        virtual ~ThreadNotif()
        {
        }

        virtual t_retval run()
        {
            Flipper flipper(_value);
            {
                TRACE(Format("before Notifier, flag1=%$") % _flag1);
                Notifier nnn(_nof);
                _flag1 = true;
                TRACE(Format("before notify, flag1=%$") % _flag1);
                nnn.notify();
            }
            {
                Waiter wat(_nof);
                while (!_flag2) wat.wait();
            }
            TRACE("before halt");
            halt();
            return 0;
        }

        Notification & _nof;
        volatile bool & _flag1;
        volatile bool & _flag2;
        volatile int & _value;

    };

    void threadnotif()
    {
        TESTINFO("begin");
        volatile bool flag1 = false;
        volatile bool flag2 = false;
        volatile int j = 0;

        Notification nof;
        ThreadNotif tf(nof, flag1, flag2, j);
        tf.start();

        {
            TRACE(Format("before Waiter, flag1=%$") % flag1);
            Waiter wat(nof);
            while (!flag1)
            {
                TRACE(Format("before wait, flag1=%$") % flag1);
                wat.wait();
                TRACE(Format("after wait, flag1=%$") % flag1);
            }
        }

        {
            Notifier nnn(nof);
            flag2 = true;
            nnn.notify();
        }
        TRACE("before cancel");
        tf.cancel();
        TRACE("before join");
        tf.join();
        CHECKINT(j, 2);
        TESTINFO("end");
    }

    class SleepThread2 : public WorkerThread
    {
    public:

        SleepThread2(const DeltaTime & to) : WorkerThread("SleepThread2"), _to(to)
        {
        }

        virtual ~SleepThread2()
        {
        }

        virtual t_retval run()
        {
            Thread::sleepit(_to);
            return 0;
        }

    protected:
        const DeltaTime _to;
    };

    void threadpipe()
    {
        TESTINFO("begin");
        const DeltaTime to = DeltaTime::s(1);
        SleepThread2 tp(to);
        Poller pox;
        pox |= tp;

        TRACE("before start");
        tp.start();
        TRACE("after start");
        for (;;)
        {
            TRACE("before timeout computing");
            const DeltaTime to2 = to + to;
            TRACE("after timeout computing");
            TRACE(Format("waiting %$ ...") % to2);
            pox.wait(to2);
            if (!pox.is_ok(tp)) break;
            TRACE("warning should not happen under unloaded system");
        }
        TRACE("join");
        tp.join();
        TESTINFO("end");
    }

    void rrw()
    {
        TESTINFO("rrw");
        VarObjSync<int> x;

        {
            ObjLock<int> kx(x);
            *kx = 0;
        }

        {
            ConstObjLock<int> kx(x);
            CHECKINT(0, *kx);
        }

        {
            ObjLock<int> kx(x);
            CHECKINT(0, *kx);
            *kx = 1;
        }

        {
            ConstObjLock<int> kx(x);
            CHECKINT(1, *kx);
            {
                ConstObjLock<int> kx2(x);
                CHECKINT(1, *kx2);
            }
        }


    }

};

static TestSync s_test_sync;

Test * test_sync()
{
    return &s_test_sync;
}

//.
