// YAL zeldan


#include <pthread.h>
#include <signal.h>
#include <sched.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include <cstdlib>

#include "thread.hxx"

#include "defs.hxx"
#include "exception.hxx"
#include "mystd.hxx"
#include "memory.hxx"
#include "mymacros.hxx"
#include "time.hxx"



using namespace std;

CXU_NS_BEGIN



//** Thread

Thread::Thread()
: _setup(false)
{
    ZERO(_id);
}




::pthread_key_t Thread::s_key_self;

void Thread::sleepit(const DeltaTime & dt)
{
    const struct timespec & ts = dt;
    ERRCALLS(::nanosleep(&ts, 0), Format("nanosleep(%$)") % dt);
}

void Thread::halt()
{
    for (;;) sleepit(DeltaTime::s(INT_MAX));
}

void Thread::yield()
{
    ERRCALLS(::sched_yield(), "sched_yeld");
}

void Thread::test()
{
    pthread_testcancel();
}

Thread * Thread::self()
{
    return reinterpret_cast<Thread *> (
            *reinterpret_cast<void**> (::pthread_getspecific(s_key_self)));
}

void WorkerThread::cancel()
{
    ASSERT(!internal());

    if (!_started)
        THROWINTERFACEEXCEPTION("trying to interrupt never started thread");

    if (isJoint())
    {
        LOGGER.warning("canceling a joint thread is ignored");
        return;
    }

    ERRCALLS(::pthread_cancel(getID()), "pthread_cancel(...)");
}

void Thread::setup(pthread_t x)
{
    if (_setup)
        THROWINTERFACEEXCEPTION("already setup");
    _id = x;
    _setup = true;
}

void Thread::setdown()
{
    if (!_setup)
        THROWINTERFACEEXCEPTION("not setup");
    //_id = 0; // non e` detto che si possa fare
    _setup = false;
}




//** WorkerThread

WorkerThread::WorkerThread(const std::string & na)
: _name(na)
, _started(false)
, _joint(false)
, _rv(0)
{
    TRACE(Format("WorkerThread::WorkerThread() : %$ %$") % name() % ptrtoa(this));
}

WorkerThread::~WorkerThread()
{
    TRACE(Format("WorkerThread::~WorkerThread() : %$") % ptrtoa(this));
    ASSERT(_joint || !_started);

    // chiude tutto il chiudibile
    // va bene in default di Pipe
}

void WorkerThread::start()
{
    ASSERT(!_started);

    SignalBlocker XVAR; // so that new thread has all signals masked

    pthread_t id;
    if (::pthread_create(&id, NULL, s_proc, this) != 0)
        THROWERREXCEPTIONHERES("pthread_create(?, NULL, ?, ?)");
    setup(id);

    _started = true;

}

Thread::t_retval WorkerThread::getRV() const
{
    ASSERT(!internal());
    ASSERT(_joint);
    return _rv;
}

void WorkerThread::join()
{
    ASSERT(!internal());
    TRACE("join");
    {
        ASSERT(!_joint);
        _rv = 0;
    }

    TRACE(Format("id=%$") % threadidtoa(getID()));
    const int x = ::pthread_join(getID(), &_rv);
    TRACE(Format("pthread_join => %$") % x);
    if (x)
        THROWERREXCEPTIONS(x, "pthread_join");

    _joint = true;
    setdown();

}

bool WorkerThread::isJoint() const
{
    ASSERT(!internal());
    return _joint;
}

bool WorkerThread::isStarted() const
{
    ASSERT(!internal());
    return _started;
}

WorkerThread * WorkerThread::specself()
{
    return reinterpret_cast<WorkerThread *> (self());
}

const FileDescriptor & WorkerThread::fd() const
{
    ASSERT(!internal());
    ASSERT(!_joint);
    return _endingPipe.rdep().fd();
}

short WorkerThread::events() const
{
    return POLLIN;
}

bool WorkerThread::match(short x) const
{
    return x & POLLHUP;
}

class z_RealFile_close
{
public:

    z_RealFile_close(RealFile & f) : _file(f)
    {
    }

    virtual ~z_RealFile_close()
    {
        LOGGER.debug("closing pipe wrep in thread closer");
        _file.close();
    }
protected:
    RealFile & _file;
};

void * WorkerThread::s_proc(void * self)
{
    {
        void **p_key = reinterpret_cast<void**> (std::malloc(sizeof (void *)));
        ASSERT(p_key != NULL);
        *p_key = self;

        const int x = ::pthread_setspecific(s_key_self, p_key);
        if (x) THROWERREXCEPTIONS(x, "pthread_setspecific");
    }

    void * retval = 0;

    WorkerThread * thread = reinterpret_cast<WorkerThread*> (self);
    z_RealFile_close XVAR(thread->_endingPipe.wrep());

    try
    {
        retval = thread->run();
    }
    catch (Exception & ex)
    {
        ex.log(Logger::e_error);
        ex.printStackTrace(stderr);
    }
    catch (std::exception & ex)
    {
        LOGGER.error(Format("std::exception: what=%$") % quote(ex.what()));
    }
    catch (...)
    {
        TRACE("uncatched exception in this thread");
        throw;
    }

    return retval;
}

const std::string WorkerThread::name() const
{
    return _name;
}



//** MainThread

MainThread::MainThread()
: _mysigint(0)
, _mysigquit(0)
, _mysigterm(0)
, _mysighup(0)
{
    setup(::pthread_self());
    void **p_key = reinterpret_cast<void**> (std::malloc(sizeof (void *)));
    ASSERT(p_key != NULL);
    *p_key = this;

    int x = ::pthread_setspecific(s_key_self, p_key);
    if (x) THROWERREXCEPTIONS(x, "pthread_setspecific");

    ignoreSIG(SIGPIPE); // per usare correttamente exc in Pipe

    struct sigaction sa;
    ZERO(sa);
    sa.sa_handler = s_child;
    sa.sa_flags = SA_NOCLDSTOP | SA_RESTART;
    ERRCALL(::sigaction(SIGCHLD, &sa, 0));
}

void MainThread::s_child(int s)
{
    const char k = static_cast<char> (s);

    FileDescriptor fd;
    for (;;)
    {
        fd = MAINTHREAD._poolpipes.next(fd);
        if (!fd) break;
        const int x = ::write(+fd, &k, sizeof (k));
        if (x < 0) perror("write");
        if (!x) fprintf(stderr, "pipe closed: %d", +fd);
    }

}

MainThread::~MainThread()
{
    try
    {
        struct sigaction sa;
        ZERO(sa);
        sa.sa_handler = SIG_DFL;
        const int x = ::sigaction(SIGCHLD, &sa, 0);
        if (x < 0)
            LOGERREXCEPTIONHERES(Logger::e_error, "sigaction(SIGCHLD, ?, 0)");

        trapSIGINT(0);
        trapSIGQUIT(0);
        trapSIGTERM(0);
        trapSIGHUP(0);

        void * p = ::pthread_getspecific(s_key_self);
        std::free(p);
        ::pthread_setspecific(s_key_self, 0);
    }
    catch (Exception & ex)
    {
        ex.log(Logger::e_warning);
    };

}

const MainThread * MainThread::specself()
{
    return reinterpret_cast<const MainThread *> (self());
}

bool MainThread::isJoint() const
{
    ASSERT(!internal());
    return false;
}

bool MainThread::isStarted() const
{
    ASSERT(!internal());
    return true;
}

void MainThread::trapSIG(int s, bool t)
{
    struct sigaction newact;
    ::memset(&newact, 0, sizeof (newact));
    newact.sa_handler = t ? s_sighandler : SIG_DFL;
    sigemptyset(&newact.sa_mask);
    //newact.sa_flags = t ? SA_RESTART : 0;
    // perche` SA_RESTART ???
    // le syscall devono comunque terminare con EINTR anche nel MainThread
    newact.sa_flags = 0;
    ERRCALLS(::sigaction(s, &newact, 0), Format("sigaction(%$, ...)") % s);
}

void MainThread::ignoreSIG(int s)
{
    struct sigaction newact;
    ::memset(&newact, 0, sizeof (newact));
    newact.sa_handler = SIG_IGN;
    sigemptyset(&newact.sa_mask);
    newact.sa_flags = 0;
    ERRCALLS(::sigaction(s, &newact, 0), Format("sigaction(%$, ...)") % s);
}

void MainThread::trapSIGINT(void (*funct)(void))
{
    _mysigint = funct;
    trapSIG(SIGINT, _mysigint != 0);
}

void MainThread::trapSIGQUIT(void (*funct)(void))
{
    _mysigquit = funct;
    trapSIG(SIGQUIT, _mysigquit != 0);
}

void MainThread::trapSIGTERM(void (*funct)(void))
{
    _mysigterm = funct;
    trapSIG(SIGTERM, _mysigterm != 0);
}

void MainThread::trapSIGHUP(void (*funct)(void))
{
    _mysighup = funct;
    trapSIG(SIGHUP, _mysighup != 0);
}


#if 0

void MainThread::ignoreSIGPIPE()
{
    maskSIG(SIGPIPE);
}
#endif

void MainThread::s_sighandler(int x)
{
    switch (x)
    {
    case SIGINT:
        if (MAINTHREAD._mysigint)
            MAINTHREAD._mysigint();
        break;

    case SIGQUIT:
        if (MAINTHREAD._mysigquit)
            MAINTHREAD._mysigquit();
        break;

    case SIGTERM:
        if (MAINTHREAD._mysigterm)
            MAINTHREAD._mysigterm();
        break;

    case SIGHUP:
        if (MAINTHREAD._mysighup)
            MAINTHREAD._mysighup();
        break;


    default:
        ASSERT(false);
    }
}

const std::string MainThread::name() const
{
    return "^";
}

void MainThread::addPoolPipe(const FileDescriptor & fd)
{
    if (!_poolpipes.add(fd))
        THROWINTERFACEEXCEPTION(
                                Format("multiple insert in pool pipe: %$") % fd
                                )
        ;
}

void MainThread::delPoolPipe(const FileDescriptor & fd)
{
    if (!_poolpipes.del(fd))
        THROWINTERFACEEXCEPTION(
                                Format("not found in pool pipe: %$") % fd
                                )
        ;
}


#if 0

//** ExternalThread

ExternalThread::ExternalThread(const std::string & na)
: _name(na)
{
    TRACE(Format("ExternalThread::ExternalThread() : %$ %$") % na % ptrtoa(this));

    _id = ::pthread_self();

    void **p_key = reinterpret_cast<void**> (std::malloc(sizeof (void *)));
    ASSERT(p_key != NULL);
    *p_key = this;

    int x = ::pthread_setspecific(s_key_self, p_key);
    if (x) THROWERREXCEPTION(x);
    TRACE(Format("ExternalThread::ExternalThread() : _id=%$ self()=%$") % threadidtoa(_id) % ptrtoa(self()));
}

ExternalThread::~ExternalThread()
{
    TRACE(Format("ExternalThread::~ExternalThread() : %$") % ptrtoa(this));
}

bool ExternalThread::isJoint() const
{
    return false;
}


#endif

CXU_NS_END

//.
