// YAL zeldan

#ifndef CXU_GLOBALS_HXX
#define CXU_GLOBALS_HXX

#include <stdlib.h>
#include <math.h>

#include <vector>
#include <cmath>
#include <string>



#include "defs.hxx"
#include "types.hxx"


CXU_NS_BEGIN


void assertdbg(bool condition, const char * description, const char * file, int line);
void tracedbg(const char * msg, const char * file, int line);
void tracedbg(const std::string & msg, const char * file, int line);


std::vector<std::string> stacktrace(size_t n = 4096);

const char * zeldan();

inline unsigned char unsign(char a)
{
    return static_cast<unsigned char> (a);
}

inline unsigned short unsign(short a)
{
    return static_cast<unsigned short> (a);
}

inline unsigned int unsign(int a)
{
    return static_cast<unsigned int> (a);
}

inline unsigned long unsign(long a)
{
    return static_cast<unsigned long> (a);
}

inline unsigned char unsign(unsigned char a)
{
    return a;
}

inline unsigned short unsign(unsigned short a)
{
    return a;
}

inline unsigned int unsign(unsigned int a)
{
    return a;
}

inline unsigned long unsign(unsigned long a)
{
    return a;
}

#if !defined(_LP64) || __CYGWIN__

inline unsigned long long unsign(long long a)
{
    return static_cast<unsigned long long> (a);
}

inline unsigned long long unsign(unsigned long long a)
{
    return a;
}
#endif



CXU_NS_END

#endif

//.
