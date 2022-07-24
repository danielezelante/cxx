// YAL zeldan


#include <sys/wait.h>
#include <sys/types.h>

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>


#include "process.hxx"


#include "defs.hxx"
#include "exception.hxx"
#include "mystd.hxx"
#include "string.hxx"
#include "thread.hxx"

#include "mymacros.hxx"

using namespace std;

CXU_NS_BEGIN



// *** Process ***
Process::Process(
                 ProcessPool * pool,
                 const std::string &path)
: _pool(pool)
, _usepath(false)
, _path(path)
, _started(false)
, _exitStatus(-1)
, _pid(0)
{
}

Process::Process(
                 ProcessPool * pool,
                 const std::string &path,
                 const std::vector<std::string> &argv
                 )
: _pool(pool)
, _usepath(false)
, _path(path)
, _argv(argv)
, _started(false)
, _exitStatus(-1)
, _pid(0)
{
}

Process::Process(
                 ProcessPool * pool,
                 const std::string &path,
                 const std::vector<std::string> &argv,
                 const std::vector<std::string> &env
                 )
: _pool(pool)
, _usepath(false)
, _path(path)
, _argv(argv)
, _env(env)
, _started(false)
, _exitStatus(-1)
, _pid(0)
{
}

Process::~Process()
{
    ASSERT(!_pid); //forse e` meglio questa per simmetria con ctor/start ?
}

void Process::start(Pipe * pIn, Pipe * pOut, Pipe * pErr)
{
    if (_started)
        THROWDATAEXCEPTION("Process already running");

    _exitStatus = -1;

    SignalBlocker XVAR; // just to be safe

    // Crea il figlio
    const pid_t pid = ::fork();
    if (pid == -1)
        THROWERREXCEPTIONHERE();

    if (pid == 0)
    {
        // Variabili di Enviroment
        const char * e_all[_env.size() + 1];
        LOOP(j, _env.size())
        e_all[j] = _env[j].c_str();

        e_all[_env.size()] = NULL;

        // Parametri: come primo parametro c'e' il nome dell'eseguibile
        const char * argv[_argv.size() + 1 + 1];
        argv[0] = _path.c_str();
        LOOP(j, _argv.size())
        argv[j + 1] = _argv[j].c_str();

        argv[_argv.size() + 1] = NULL;


        // Ripristina la normale mascheratura dei segnali
        sigset_t xmask;
        memset(&xmask, 0, sizeof (xmask));
        sigfillset(&xmask);

        const int x = ::sigprocmask(SIG_UNBLOCK, &xmask, 0);
        if (x)
            LOGERREXCEPTION(Logger::e_warning, x);

        // Gestisce in, out, err
        if (pIn)
        {
            ERRCALL(::close(0)); // chiude stdin del child
            ERRCALL(::dup2(+pIn->rdep().fd(), 0)); // setta stdin con read endpoint
        }
        if (pOut)
        {
            ERRCALL(::close(1)); // chiude stdout del child
            ERRCALL(::dup2(+pOut->wrep().fd(), 1)); // setta stdout con write endpoint
        }
        if (pErr)
        {
            ERRCALL(::close(2)); // chiude stderr del child
            ERRCALL(::dup2(+pErr->wrep().fd(), 2)); // setta stderr con write endpoint
        }


#if __linux___
        // TODO valutare se meglio usare posix_spawn
        const int e = _usepath
                ? ::execvpe(_path.c_str(), const_cast<char **> (argv), const_cast<char **> (e_all))
                : ::execve(_path.c_str(), const_cast<char **> (argv), const_cast<char **> (e_all))
                ;
#else
        ASSERT(!_usepath);
        // TODO see how to implement path and envirnment in BSD
        const int e = ::execve(_path.c_str(), const_cast<char **> (argv), const_cast<char **> (e_all));
#endif
        // qui il cast a non-const e` innocuo perche`
        // se exec ha successo parte il nuovo processo
        // se invece exec fallisce termina qui il figlio appena forkato
        if (e == -1)
        {
            perror("execve");
            _exit(1); // TODO controllare con C++ e threads 
        }
    }
    else
    {
        _pid = pid;
        _started = true;
        if (_pool) _pool->add(this);

        if (pIn) pIn->rdep().close();
        if (pOut) pOut->wrep().close();
        if (pErr) pErr->wrep().close();

    }

}

void Process::start()
{
    return start(0, 0, 0);
}

/**
 * Valori di ritorno:
 * < 0: il processo e' terminato a causa di una signal
 * = 0: il processo e' in esecuzione
 * > 0: il processo e' terminato regolarmente con valore di ritorno
 */
int Process::wait(int opt)
{
    //TRACE("Process::wait");

    ASSERT(_pid);
    ASSERT(_started);

    for (;;)
    {
        int status = 0;

        //TRACE("before waitpid");
        const pid_t p = ::waitpid(_pid, &status, opt);
        //TRACE(Format("pid_t=%$, status=%$") %p %utox(status));

        if (p == -1) // Error
            THROWERREXCEPTIONHERE();
        if (p == 0) // Status not changed (only with WNOHANG)
            return 0;

        if (p != _pid)
            THROWOSEXCEPTION(
                             Format("waitpid pid mismatch: exp=%$, got=%$")
                             % _pid
                             % p
                             );

        if (WIFEXITED(status) || WIFSIGNALED(status))
        {
            _exitStatus = status;
            break;
        }
    };

    _pid = 0;
    if (_pool) _pool->del(this);

    return WIFSIGNALED(_exitStatus) ? -1 : +1;
}

/**
 * Per i valori di ritorno vedi wait(int)
 */
int Process::wait()
{
    return wait(0);
}

// Per i valori di ritorno vedi wait(int)

int Process::waitNoHang()
{
    return wait(WNOHANG);
}

void Process::kill(int sig)
{
    ERRCALL(::kill(_pid, sig));
}

int Process::getExitCode() const
{
    if (WIFEXITED(_exitStatus))
        return WEXITSTATUS(_exitStatus);

    THROWINTERFACEEXCEPTION("process not exited");
}

int Process::getSignal() const
{
    if (WIFSIGNALED(_exitStatus))
        return WTERMSIG(_exitStatus);

    THROWDATAEXCEPTION("process not signaled");
}

#if __linux__

void Process::usePath(bool x)
{
    ASSERT(!_started);
    _usepath = x;
}
#endif

// ProcessPool

ProcessPool::~ProcessPool()
{
    ASSERT(empty());
    try
    {
        SignalBlocker XVAR(SIGCHLD);
        MAINTHREAD.delPoolPipe(_pipe.wrep().fd());
    }
    catch (const Exception & ex)
    {
        ex.log(Logger::e_error);
    }
}

ProcessPool::ProcessPool()
{
    SignalBlocker XVAR(SIGCHLD);
    MAINTHREAD.addPoolPipe(_pipe.wrep().fd());
}

Process * ProcessPool::waitAny()
{
    return waitAny(DeltaTime::s(-1));
}

Process * ProcessPool::waitAny(const DeltaTime & to)
{
    for (;;)
    {
        //TRACE("waitAny before wait NOHANG iteration");
        for (auto const & it : _procs)
        {
            //TRACE("iterating ...");
            Process * p = it;
            //TRACE(Format("p=%$") %ptrtoa(p));
            const int x = p->wait(WNOHANG);
            // la wait se torna != 0, fa erase di iteratore corrente
            if (x) return p;
        }

        //TRACE("waitAny after wait NOHANG iteration");

        Poller pox;
        pox |= _pipe.rdep();

        //TRACE("waitAny pox.wait ...");
        if (!pox.wait(to)) break;
        //TRACE("waitAny pox.wait complete");

        if (!pox.is_ok(_pipe.rdep()))
            LOGGER.error(Format("not ok: %$") % _pipe.rdep().fd());
        if (pox / _pipe.rdep())
        {
            char k;
            NativeData ndk(k);
            if (_pipe.rdep().read(ndk) != sizeof (k))
                LOGGER.error(Format("mismatch in read pool pipe"));
            if (k != static_cast<char> (SIGCHLD))
                LOGGER.error(
                             Format("signal type mismatch in pipe: exp=%$ got=%$")
                             % static_cast<char> (SIGCHLD)
                             % k
                             );
        }
        else
        {
            LOGGER.warning("ghost wakeup from poll on pool pipe");
        }
    }

    return 0;

}

void ProcessPool::add(Process * p)
{
    ASSERT(p);
    if (!_procs.insert(p).second)
        THROWINTERFACEEXCEPTION(
                                Format("multiple insertion in map: Process=%$")
                                % ptrtoa(p)
                                );
}

void ProcessPool::del(Process * p)
{
    ASSERT(p);
    if (!_procs.erase(p))
        THROWINTERFACEEXCEPTION(
                                Format("not found in map: Process=%$")
                                % ptrtoa(p)
                                );
}

void ProcessPool::killAll(int n)
{
    for (auto const & it : _procs) it->kill(n);
}

CXU_NS_END


//.
