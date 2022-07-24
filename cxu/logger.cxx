// YAL zeldan

#include <syslog.h>
#include <sys/time.h>

#include <stdlib.h>
#include <stdio.h>

#include <iostream>

#include "logger.hxx"

#include "exception.hxx"
#include "string.hxx"
#include "defs.hxx"
#include "globals.hxx"
#include "mystd.hxx"
#include "sync.hxx"
#include "thread.hxx"

#include "mymacros.hxx"

using namespace std;

CXU_NS_BEGIN


Logger::Logger(const char * progname) :
_sysloglevel(e_nil),
_progname(progname)
{
    _default = stderr;
#ifdef NDEBUG
    addout(_default, Logger::e_info);
#else
    addout(_default, Logger::e_debug);
#endif


}

Logger::~Logger()
{
    try
    {
        usesyslog(e_nil);
    }
    catch (Exception & ex)
    {
        ex.log(Logger::e_warning);
    }
}

const char * Logger::s_levelstr(t_level w)
{
    switch (w)
    {
    case e_nil: return "NIL";
    case e_error: return "ERR";
    case e_warning: return "WRN";
    case e_info: return "INF";
    case e_debug: return "DBG";
    }

    return "???";
}

bool Logger::parselevel(int x, t_level & l)
{
    switch (x)
    {
    case 0: l = e_nil;
        return true;
    case 1: l = e_error;
        return true;
    case 2: l = e_warning;
        return true;
    case 3: l = e_info;
        return true;
    case 4: l = e_debug;
        return true;
    }

    return false;
}

bool Logger::parselevel(const char * x, t_level & l)
{
    const int n = atoi(x);
    if (n == 0 && !isdigit(x[0])) return false;
    return parselevel(n, l);
}

void Logger::printout(const char * message, t_level w)
{
    std::string x;

    for (auto const & it : _out)
    {
        if (static_cast<int> (w) <= static_cast<int> (it.second))
        {
            if (x.empty()) // so that x is computed only if needed
                x = Format("%$ %$ @%$ %$\n")
                % timestamp()
                % s_levelstr(w)
                % Thread::self()->name()
                % std::string(message)
                ;

            fputs(x.c_str(), it.first);
            fflush(it.first);
        }
    }
}

void Logger::log(const char * str, t_level level)
{
    printout(str, level);
    Thread * th = Thread::self();
    if (static_cast<int> (level) <= static_cast<int> (_sysloglevel))
    {
        int w = 0;
        switch (level)
        {
        case e_nil: return;
        case e_error: w = LOG_ERR;
            break;
        case e_warning: w = LOG_WARNING;
            break;
        case e_info: w = LOG_INFO;
            break;
        case e_debug: w = LOG_DEBUG;
            break;
        default: ASSERT(false);
        }

        ::syslog(w, "%s @%s %s", s_levelstr(level), th->name().c_str(), str);
    }
}

void Logger::log(const std::string & str, t_level level)
{
    log(str.data(), level);
}

void Logger::error(const char * str)
{
    log(str, e_error);
}

void Logger::error(const std::string & str)
{
    log(str.data(), e_error);
}

void Logger::warning(const char * str)
{
    log(str, e_warning);
}

void Logger::warning(const std::string & str)
{
    log(str.data(), e_warning);
}

void Logger::info(const char * str)
{
    log(str, e_info);
}

void Logger::info(const std::string & str)
{
    log(str.data(), e_info);
}

void Logger::debug(const char * str)
{
    log(str, e_debug);
}

void Logger::debug(const std::string & str)
{
    log(str.data(), e_debug);
}

void Logger::fatal(const char * str)
{
    std::cerr << "FATAL: " << str;
    ASSERT(false);
    abort(); //when ASSERT is no-op
}

bool Logger::addout(FILE * f, t_level w)
{
    return _out.insert(std::make_pair(f, w)).second;
}

bool Logger::delout(FILE * f)
{
    return _out.erase(f);
}

bool Logger::deldefault()
{
    bool x = _out.erase(_default);
    if (_default && _default != stderr) fclose(_default);
    _default = 0;
    return x;
}

std::string Logger::timestamp()
{
    struct timeval tv;
    ::gettimeofday(&tv, 0);
    struct tm stm;
    time_t tvsec = tv.tv_sec;
    ::localtime_r(&tvsec, &stm);

    char str1[64];
    ::strftime(str1, 64, "%FT%T", &stm);

    char str2[8];
    ::strftime(str2, 8, "%z", &stm);

    return Format("%$.%$%$")
            % str1
            % rjus(utod(tv.tv_usec / 1000), 3, '0')
            % str2;


}

void Logger::usesyslog(t_level x)
{
    if (_sysloglevel != e_nil && x == e_nil)
        ::closelog();

    if (_sysloglevel == e_nil && x != e_nil)
        ::openlog(_progname.data(), LOG_PID | LOG_CONS, LOG_USER);

    _sysloglevel = x;
}

std::string xstrerror(int e)
{
    char str[256] = {'?', '\0'};
    strerror_r(e, str, DIM(str));
    return str;
}


CXU_NS_END


//.
