// YAL zeldan

#ifndef CXU_INTEGER_HXX
#define CXU_INTEGER_HXX

#include <stdint.h>
#include <math.h>

#include "defs.hxx"
#include "mystd.hxx"


CXU_NS_BEGIN


unsigned int makepowerof2(int n);
unsigned int nextpowerof2(unsigned int);
int log2sup(unsigned int n, bool * exact = 0);

//! Sign

/*! -1 or 0 or +1 */
template <class T> int sgn(const T & x)
{
    if (x > 0) return +1;
    if (x < 0) return -1;
    return 0;
}

//! Absolute value by type

template <class T> T absT(const T & x)
{
    return (x >= 0 ? x : -x);
}

//! Square

template <class T> const T sqr(const T & x)
{
    return x * x;
}

//! Exchange

template <class T> void xchg(T & a, T & b)
{
    T c;
    c = a, a = b, b = c;
}

template <typename T> bool inoo(T x, T a, T b)
{
    return (x > a)&&(x < b);
}

template <typename T> bool inco(T x, T a, T b)
{
    return (x >= a)&&(x < b);
}

template <typename T> bool inoc(T x, T a, T b)
{
    return (x > a)&&(x <= b);
}

template <typename T> bool incc(T x, T a, T b)
{
    return (x >= a)&&(x <= b);
}

template <> double absT(const double & x);

uint64_t htonq(uint64_t);
uint64_t ntohq(uint64_t);


#if (BYTE_ORDER == LITTLE_ENDIAN)

inline void htole_here(int &)
{
}

inline void htole_here(unsigned int &)
{
}

inline void htole_here(long &)
{
}

inline void htole_here(unsigned long &)
{
}
#ifndef _LP64

inline void htole_here(int64_t &)
{
}

inline void htole_here(uint64_t &)
{
}
#endif

inline void letoh_here(int &)
{
}

inline void letoh_here(unsigned int &)
{
}

inline void letoh_here(long &)
{
}

inline void letoh_here(unsigned long &)
{
}
#ifndef _LP64

inline void letoh_here(int64_t &)
{
}

inline void letoh_here(uint64_t &)
{
}
#endif


#else

#define CXU_MACRO_HTOLE_HERE    \
{       \
        char * p = reinterpret_cast<char*>(&x);  \
        const size_t z = sizeof(x);   \
        CXU_LOOP(j, z) xchg(p[j],p[z-j-1]);  \
}       \

inline void htole_here(int & x) CXU_MACRO_HTOLE_HERE
inline void htole_here(unsigned int & x) CXU_MACRO_HTOLE_HERE
inline void htole_here(long & x) CXU_MACRO_HTOLE_HERE
inline void htole_here(unsigned long & x) CXU_MACRO_HTOLE_HERE

#ifndef _LP64
inline void htole_here(int64_t & x) CXU_MACRO_HTOLE_HERE
inline void htole_here(uint64_t & x) CXU_MACRO_HTOLE_HERE
#endif

#undef CXU_MACRO_HTOLE_HERE


#define CXU_MACRO_LETOH_HERE    \
{       \
        char * p = reinterpret_cast<char*>(&x);  \
        const size_t z = sizeof(x);   \
        CXU_LOOP(j, z) xchg(p[j],p[z-j-1]);  \
}       \

inline void letoh_here(int & x) CXU_MACRO_LETOH_HERE
inline void letoh_here(unsigned int & x) CXU_MACRO_LETOH_HERE
inline void letoh_here(long & x) CXU_MACRO_LETOH_HERE
inline void letoh_here(unsigned long & x) CXU_MACRO_LETOH_HERE

#ifndef _LP64
inline void letoh_here(int64_t & x) CXU_MACRO_LETOH_HERE
inline void letoh_here(uint64_t & x) CXU_MACRO_LETOH_HERE
#endif

#undef CXU_MACRO_LETOH_HERE

#endif

bool isPrime(unsigned int);
unsigned int nextPrime(unsigned int);



CXU_NS_END


#endif

//.
