// YAL zeldan

#include <stdlib.h>

#include <iostream>
#include <iomanip>


#include <cxu/cxu.hxx>
#include <cxu/mymacros.hxx>

using namespace std;
using namespace cxu;

class TestProcess : public Test
{
public:

    TestProcess()
    {
    }

    void vrun() override
    {
        single1();
        pool1();
        poolprog(10);
        poolsame(100, 1);
        poolsame(100, 0);
    }



protected:

    void single1()
    {
        puts("#single1");
        vector<string> a;
        a.push_back("1");
        Process p(0, "/bin/sleep", a);
        p.start();
        p.wait();
    }

    void pool1()
    {
        puts("#pool1");
        ProcessPool pp;
        vector<string> a;
        a.push_back("1");
        Process p(&pp, "/bin/sleep", a);
        p.start();
        Process * x = pp.waitAny();
        CHECKPTR(&p, x);
    }

    void poolprog(size_t n)
    {
        printf("#poolprog(%zu) \n", n);
        ProcessPool pp;
        vector<Process*> p(n);

        LOOP(j, n)
        {
            vector<string> a;
            a.push_back(utod(j));
            p[j] = new Process(&pp, "/bin/sleep", a);
        }

        LOOP(j, n) p[j]->start();

        while (!pp.empty())
        {
            Process * x = pp.waitAny();
            if (x)
            {

                LOOP(j, n)
                {
                    if (p[j] == x)
                    {
                        printf("# p[%zu] => %d \n", j, x->getStatus());
                        p[j] = 0;
                    }
                }
                delete x;
            }
        }

        LOOP(j, n) CHECKPTR(0, p[j]);

    }

    void poolsame(size_t n, int slto)
    {
        printf("#poolsame(%zu, %d) \n", n, slto);
        ProcessPool pp;
        vector<Process*> p(n);

        LOOP(j, n)
        {
            vector<string> a;
            a.push_back(utod(slto));
            p[j] = new Process(&pp, "/bin/sleep", a);
        }

        LOOP(j, n) p[j]->start();

        while (!pp.empty())
        {
            Process * x = pp.waitAny();
            if (x)
            {

                LOOP(j, n)
                {
                    if (p[j] == x)
                    {
                        printf("# p[%zu] => %d \n", j, x->getStatus());
                        p[j] = 0;
                    }
                }
                delete x;
            }
        }

        LOOP(j, n) CHECKPTR(0, p[j]);

    }

};

static TestProcess s_test_process;

Test * test_process()
{
    return &s_test_process;
}

//.
