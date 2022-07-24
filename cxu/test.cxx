// YAL zeldan

#include <string>

#include <float.h>
#include <math.h>
#include <stdio.h>


#if __sun__
#include <ieeefp.h>
#endif

#include "test.hxx"
#include "string.hxx"
#include "exception.hxx"
#include "mystd.hxx"

CXU_NS_BEGIN


void Test::checkreal(const char * f, const char * s0, const char * s1, double v0, double v1)
{
    bool fail = (fabs(v0 - v1) > sqrt(DBL_EPSILON)) || !finite(v0) || !finite(v1);
    printf("@%c %s: checkreal(%s=%g,%s=%g)\n", fail ? '?' : '!', f, s0, v0, s1, v1);
    _fail |= fail;
}

void Test::checkptr(const char * f, const char * s0, const char * s1, void * v0, void * v1)
{
    bool fail = (v0 != v1);
    printf("@%c %s: checkptr(%s=%p,%s=%p)\n", fail ? '?' : '!', f, s0, v0, s1, v1);
    _fail |= fail;
}

void Test::checkint(const char * f, const char * s0, const char * s1, int v0, int v1)
{
    bool fail = (v0 != v1);
    printf("@%c %s: checkint(%s=%d,%s=%d)\n", fail ? '?' : '!', f, s0, v0, s1, v1);
    _fail |= fail;
}

void Test::checksize(const char * f, const char * s0, const char * s1, size_t v0, size_t v1)
{
    bool fail = (v0 != v1);
    const char * msg = "@%c %s: checkint(%s=%zu,%s=%zu)\n";
    printf(msg, fail ? '?' : '!', f, s0, v0, s1, v1);
    _fail |= fail;
}

void Test::checklong(const char * f, const char * s0, const char * s1, long v0, long v1)
{
    bool fail = (v0 != v1);
    printf("@%c %s: checklong(%s=%ld,%s=%ld)\n", fail ? '?' : '!', f, s0, v0, s1, v1);
    _fail |= fail;
}

void Test::checkchar(const char * f, const char * s0, const char * s1, char v0, char v1)
{
    bool fail = (v0 != v1);
    printf("@%c %s: checkchar(%s='%c',%s='%c')\n", fail ? '?' : '!', f, s0, v0, s1, v1);
    _fail |= fail;
}

void Test::checkbool(const char * f, const char * s0, const char * s1, bool v0, bool v1)
{
    bool fail = (v0 != v1);
    printf("@%c %s: checkbool(%s=%s,%s=%s)\n", fail ? '?' : '!', f, s0, v0 ? "true" : "false", s1, v1 ? "true" : "false");
    _fail |= fail;
}

void Test::checkstring(const char * f, const char * s0, const char * s1, const std::string & v0, const std::string & v1)
{
    bool fail = (v0 != v1);
    printf("@%c %s: checkstring(%s=%s,%s=%s)\n", fail ? '?' : '!', f, s0, quote(v0).data(), s1, quote(v1).data());
    _fail |= fail;
}

void Test::checkstring(const char * f, const char * s0, const char * s1, const std::wstring & v0, const std::wstring & v1)
{
    bool fail = (v0 != v1);
    printf("@%c %s: checkstring(%s=%s,%s=%s)\n", fail ? '?' : '!', f, s0, quote(v0).data(), s1, quote(v1).data());
    _fail |= fail;
}

void Test::info(const char * f, const char * s) const
{
    printf("# %s : %s\n", f, s);
}

void Test::info(const char * f, const std::string & s) const
{
    printf("# %s : %s\n", f, s.data());
}

bool Test::run()
{
    _fail = false;
    try
    {
        vrun();
    }
    catch (Exception & xe)
    {
        printf("@? Exception: %s\n", xe.str().data());
        xe.printStackTrace(stderr);
        _fail = true;
    }
    catch (std::exception & xe)
    {
        printf("@? std::exception: %s\n", xe.what());
        _fail = true;
    }
    catch (...)
    {
        printf("@? c++ exception\n");
        _fail = true;
    }

    return _fail;
}

void TestSuite::add(Test * t)
{
    t->_suite = this;
    _tests.push_back(t);
}

bool TestSuite::run()
{
    bool fail = false;

    puts("# begin TestSuite");
    for (const auto & it : _tests)
        fail |= it->run();

    if (fail)
        puts("# end TestSuite: FAILED");
    else
        puts("# end TestSuite: OK");

    return fail;
}



CXU_NS_END


//.
