// YAL zeldan

#ifndef CXU_TEST_HXX
#define CXU_TEST_HXX

#include <stdio.h>
#include <set>

#include "types.hxx"
#include "object.hxx"
#include "string.hxx"

#define CXU_CHECK(X,F) check(__PRETTY_FUNCTION__,#X,#F,X,F)
#define CXU_CHECKPTR(X,F) checkptr(__PRETTY_FUNCTION__,#X,#F,X,F)
#define CXU_CHECKINT(X,F) checkint(__PRETTY_FUNCTION__,#X,#F,X,F)
#define CXU_CHECKSIZE(X,F) checksize(__PRETTY_FUNCTION__,#X,#F,X,F)
#define CXU_CHECKLONG(X,F) checklong(__PRETTY_FUNCTION__,#X,#F,X,F)
#define CXU_CHECKCHAR(X,F) checkchar(__PRETTY_FUNCTION__,#X,#F,X,F)
#define CXU_CHECKREAL(X,F) checkreal(__PRETTY_FUNCTION__,#X,#F,X,F)
#define CXU_CHECKBOOL(X,F) checkbool(__PRETTY_FUNCTION__,#X,#F,X,F)
#define CXU_CHECKSTRING(X,F) checkstring(__PRETTY_FUNCTION__,#X,#F,X,F)
#define CXU_TESTINFO(X) info(__PRETTY_FUNCTION__, X)

CXU_NS_BEGIN

class Test : Entity
{
    friend class TestSuite;
public:
    virtual ~Test() = default;

    Test() : _suite(0), _fail(false)
    {
    }
    virtual void vrun() = 0;
    bool run();

    template <typename T> void check(const char * f, const char * expectedname, const char * effectivename, const T & expected, const T & effective)
    {
        bool fail = (expected != effective);
        std::string s = Format("@%$ %$: check(%$=%$, %$=%$)")
                % (fail ? "?" : "!") % f
                % expectedname % expected
                % effectivename % effective;

        printf("%s\n", s.data());
        _fail |= fail;

    }

    void checkptr(const char * f, const char * expectedname, const char * effectivename, void * expected, void * effective);
    void checkint(const char * f, const char * expectedname, const char * effectivename, int expected, int effective);
    void checksize(const char * f, const char * expectedname, const char * effectivename, size_t expected, size_t effective);
    void checklong(const char * f, const char * expectedname, const char * effectivename, long expected, long effective);
    void checkchar(const char * f, const char * expectedname, const char * effectivename, char expected, char effective);
    void checkreal(const char * f, const char * expectedname, const char * effectivename, double expected, double effective);
    void checkbool(const char * f, const char * expectedname, const char * effectivename, bool expected, bool effective);
    void checkstring(const char * f, const char * expectedname, const char * effectivename, const std::string & expected, const std::string & effective);

    void checkstring(const char * f, const char * expectedname, const char * effectivename, const std::wstring & expected, const std::wstring & effective);

    void info(const char * f, const char * s) const;
    void info(const char * f, const std::string &) const;


protected:

    const TestSuite * _suite;

private:
    bool _fail;

};

class TestSuite : Entity
{
public:
    virtual ~TestSuite() = default;

    explicit TestSuite(const char * t) : _tmpdir(t)
    {
    }

    void add(Test *);
    bool run();

    std::string tmpdir() const
    {
        return _tmpdir;
    }

protected:
    std::string _tmpdir;

private:
    std::vector<Test*> _tests;
};




CXU_NS_END

#endif


//.
