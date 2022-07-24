// YAL zeldan
//
#include <stdlib.h>
#include <stdio.h>

#include <syslog.h>
#include <signal.h>

#if __linux__
#include <execinfo.h>
#include <cxxabi.h>
#endif

#include "globals.hxx"

#include "libinfo.hxx"
#include "string.hxx"
#include "mystd.hxx"
#include "logger.hxx"
#include "mymacros.hxx"
#include "defs.hxx"
#include "exception.hxx"

CXU_NS_BEGIN


const char * zeldan()
{
    return "\033[1;31mz\033[1;33me\033[1;32ml\033[1;36md\033[1;34ma\033[1;35mn\033[0;39m";
}

void tracedbg(const char * msg, const char * file, int line)
{
    tracedbg(std::string(msg), file, line);
}

void tracedbg(const std::string & msg, const char * file, int line)
{
    LOGGER.debug(Format("%$(%$):%$") % file % itod(line) % msg);
}

void assertdbg(bool c, const char * d, const char * f, int l)
{
    if (!c)
    {
        fprintf(stderr, "ASSERT(%s) : %s : %s(%d)\n", d, threadidtoa(pthread_self()).data(), f, l);
        const std::vector<std::string> s = stacktrace();
        fputs("[\n", stderr);
        for (auto const & it : s)
            fprintf(stderr, "\t%s\n", it.data());
        fputs("]\n", stderr);

        raise(SIGABRT);
    }

}

#if !defined(__SSP__) && defined(__linux__)

std::vector<std::string> stacktrace(size_t t)
{
    void ** stack = static_cast<void**> (malloc(t * sizeof (void*)));
    int n = backtrace(stack, t);

    // pulisce ricorsioni "infinite"
    for (int j = n - 1; j > 0; --j)
        if (stack[j] == stack[j - 1])
            n = j;

    std::vector<std::string> retval;
    char ** names = backtrace_symbols(stack, n);

    for (int j = 0; j < n; ++j)
    {
        const char * name = names[j];
        const int namelen = strlen(name);
        int a1 = 0;
        while (name[a1] && name[a1] != '(') ++a1;
        int a2 = a1;
        while (name[a2] && name[a2] != '+' && name[a2] != ')') ++a2;
        int a3 = a2;
        if (name[a3] == '+') // per gestire il caso "()"
            while (name[a3] && name[a3] != ')') ++a3;

        char file[a1 + 16];
        strncpy(file, name, a1);
        file[a1] = 0;

        char func[a2 - (a1 + 1) + 16];
        if ((a1 != namelen) && (a2 != namelen))
        {
            strncpy(func, name + a1 + 1, a2 - (a1 + 1));
            func[a2 - (a1 + 1)] = 0;
        }
        else
            func[0] = '\0';

        char line[a3 - (a2 + 1) + 16];
        if ((a2 != namelen) && (a3 != namelen) && (a2 != a3))
        {
            strncpy(line, name + a2 + 1, a3 - (a2 + 1));
            line[a3 - (a2 + 1)] = 0;
        }
        else
            line[0] = '\0';

        unsigned int iline = 0;
        if (strlen(line) > 2)
            iline = strtoul(line + 2, 0, 16);

        int status = -1;
        size_t length = strlen(func) * 2 + 256;
        char deman[length];
        if (strcmp(func, "main"))
            __cxxabiv1::__cxa_demangle(func, deman, &length, &status);

        if (!status && !strcmp(deman, "cxu::stacktrace(unsigned)")) continue;

        const std::string current = Format("%$(%$+%$)%$")
                % file
                % (status ? func : deman)
                % utod(iline)
                % (name + a3 + 1)
                ;

        retval.push_back(current);
    }

    free(names);
    free(stack);

    return retval;

}
#else

std::vector<std::string> stacktrace(size_t)
{
    std::vector<std::string> a;
    a.push_back("???");
    return a;
}
#endif


CXU_NS_END

//.
