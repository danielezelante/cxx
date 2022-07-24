// YAL zeldan

#ifndef CXU_STATS_HXX
#define CXU_STATS_HXX

#include "defs.hxx"
#include "types.hxx"
#include "exception.hxx"
#include "defs.hxx"

#undef min
#undef max

CXU_NS_BEGIN

template <typename T> class Minimal
{
private:
    T _value;
    bool _init;

public:

    Minimal() : _value(), _init(false)
    {
    }

    void operator|=(const T & x)
    {
        _value = _init ? std::min(_value, x) : x;
        _init = true;
    }

    operator const T & () const
    {
        if (!_init) CXU_THROWDATAEXCEPTION("no data");
        return _value;
    }

    bool is() const
    {
        return _init;
    }
};

template <typename T> class Maximal
{
private:
    T _value;
    bool _init;

public:

    Maximal() : _init(false)
    {
    }

    void operator|=(const T & x)
    {
        _value = _init ? std::max(_value, x) : x;
        _init = true;
    }

    operator const T & () const
    {
        if (!_init) CXU_THROWDATAEXCEPTION("no data");
        return _value;
    }

    bool is() const
    {
        return _init;
    }
};








CXU_NS_END

#endif


//.
