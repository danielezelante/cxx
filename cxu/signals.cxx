// YAL zeldan

#include <string.h>

#include "signals.hxx"
#include "exception.hxx"
#include "string.hxx"

#include "mymacros.hxx"

using namespace std;

CXU_NS_BEGIN


SignalBlocker::~SignalBlocker()
{
    const int x = ::pthread_sigmask(SIG_SETMASK, &_ss, 0);
    if (x < 0)
        LOGERREXCEPTIONHERES(
                             Logger::e_error,
                             "pthread_sigmask(SIG_SETMASK, ?, 0)"
                             );
}

SignalBlocker::SignalBlocker()
{
    sigset_t ss;
    ZERO(ss);
    ERRCALLS(::sigfillset(&ss), "sigfillset");
    ERRCALLS(::pthread_sigmask(SIG_BLOCK, &ss, &_ss),
             "pthread_sigmask(SIG_BLOCK, {all}, ?)"
             );
}

SignalBlocker::SignalBlocker(int n)
{
    sigset_t ss;
    ZERO(ss);
    ERRCALLS(::sigaddset(&ss, n), Format("sigaddset(?, %$)") % n);
    ERRCALLS(::pthread_sigmask(SIG_BLOCK, &ss, &_ss),
             Format("pthread_sigmask(SIG_BLOCK, {%$}, ?)") % n
             );
}

SignalBlocker::SignalBlocker(const set<int> & q)
{
    sigset_t ss;
    ZERO(ss);
    for (auto n : q)
        ERRCALLS(::sigaddset(&ss, n), Format("sigaddset(?, %$)") % n);
    ERRCALLS(::pthread_sigmask(SIG_BLOCK, &ss, &_ss),
             "pthread_sigmask(SIG_BLOCK, #?, ?)");
}

CXU_NS_END

//.
