// YAL zeldan

#include "integer.hxx"
#include <string>

CXU_NS_BEGIN

unsigned int makepowerof2(int n)
{
    if (n < 0) return 0;
    return 1 << n;
}

unsigned int nextpowerof2(unsigned int n)
{
    return makepowerof2(log2sup(n, 0));
}

int log2sup(unsigned int n, bool * exact)
{
    if (exact) *exact = true;
    if (n == 0) return -1;
    int c = 0;
    while (n)
    {
        if (exact && (n & 1) && n > 1) *exact = false;
        n = n >> 1;
        ++c;
    }
    return c;
}

uint64_t htonq(uint64_t hq)
{
#if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t nq = 0;

    const unsigned char *phq = reinterpret_cast<const unsigned char*> (&hq);
    unsigned char *pnq = reinterpret_cast<unsigned char*> (&nq);

    for (size_t i = 0; i<sizeof (uint64_t); ++i)
        pnq[sizeof (uint64_t) - 1 - i] = phq[i];

    return nq;
#else
    return hq;
#endif
}

uint64_t ntohq(uint64_t nq)
{
#if (BYTE_ORDER == LITTLE_ENDIAN) 
    uint64_t hq = 0;

    unsigned char *phq = reinterpret_cast<unsigned char*> (&hq);
    const unsigned char *pnq = reinterpret_cast<const unsigned char*> (&nq);

    for (size_t i = 0; i<sizeof (uint64_t); ++i)
        phq[sizeof (uint64_t) - 1 - i] = pnq[i];

    return hq;
#else
    return nq;
#endif
}



CXU_NS_END

//.
