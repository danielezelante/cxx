// YAL zeldan

#include <cstdlib>
#include <string>

#include "init.hxx"

#include "defs.hxx"
#include "socket.hxx"
#include "logger.hxx"
#include "thread.hxx"
#include "mymacros.hxx"


CXU_NS_BEGIN


Init * Init::s_s = 0;

Init::Init(const char * progname)
{
    ASSERT(!s_s);
    _logger = new Logger(progname);
    // indirect use with std::malloc
    const int x = ::pthread_key_create(&CXU_NS::Thread::s_key_self, std::free);
    if (x)
        THROWERREXCEPTIONS(x, "pthread_key_create");

    s_s = this; // MainThread wants the logger

    _mainthread = new MainThread();


}

Init::~Init()
{
    ASSERT(s_s);

    delete _mainthread;

    const int x = ::pthread_key_delete(CXU_NS::Thread::s_key_self);
    if (x) LOGERREXCEPTION(Logger::e_warning, x);

    delete _logger;
    s_s = 0;
}

Logger & Init::logger()
{
    return *_logger;
}

MainThread & Init::mainthread()
{
    return *_mainthread;
}

int cxumain(int argc, char ** argv, int (*func)(int, char**))
{
    Init init_(argv[0]);
    try
    {
        return func(argc, argv);
    }
    catch (const Exception & ex)
    {
        ex.log(Logger::e_error);
        FILE * f = CXU_LOGGER.getdefault();
        if (f) ex.printStackTrace(f, "# ");
        else
        {
#ifndef NDEBUG
            ex.printStackTrace(stderr, "# ");
#endif
        }
    }
    catch (const std::exception & ex)
    {
        CXU_LOGGER.error(Format("std::exception: what=%$") % quote(ex.what()));
    }
    catch (...)
    {
        CXU_LOGGER.debug("unknown exception in main");
        throw;
    }

    return -1;

}


CXU_NS_END

//.
