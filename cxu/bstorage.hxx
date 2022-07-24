// YAL zeldan

#ifndef CXU_BSTORAGE_HXX
#define CXU_BSTORAGE_HXX

#include <stack>
#include <set>
#include <vector>
#include <list>
#include <map>
#include <string>

#include "defs.hxx"
#include "types.hxx"
#include "object.hxx"
#include "mystd.hxx"
#include "memory.hxx"

CXU_NS_BEGIN

//! Binary Input Storage
class BIStorage : Entity
{
public:
    explicit BIStorage(const Data &);
    virtual ~BIStorage();

    enum Cmd
    {
        BEGIN, END, ABORT
    };

    template <class T> BIStorage & operator>>(Serialization<T> & o)
    {
        o.serialize(*this);
        return *this;
    }

    BIStorage & operator>>(Cmd);

    BIStorage & operator>>(char &);
    BIStorage & operator>>(unsigned char &);
    BIStorage & operator>>(short &);
    BIStorage & operator>>(unsigned short &);
    BIStorage & operator>>(int &);
    BIStorage & operator>>(unsigned int &);
    BIStorage & operator>>(long &);
    BIStorage & operator>>(unsigned long &);

    BIStorage & operator>>(double &);
    BIStorage & operator>>(bool &);
    BIStorage & operator>>(std::string &);

    BIStorage & operator>>(std::wstring &);

#ifndef _LP64
    BIStorage & operator>>(int64_t &);
    BIStorage & operator>>(uint64_t &);
#endif


    void read(const Segment &); // serve public per buffer


protected:

    void checkBitFormat();
    void skip(size_t);
    const char * cur() const;

private:
    const Data & _data;
    size_t _ptr;
    std::stack<size_t> _blocks;
};

class BOStorage : Entity
{
public:

    explicit BOStorage(Buffer &);

    virtual ~BOStorage();

    enum Cmd
    {
        BEGIN, END
    };

    template <class T> BOStorage & operator<<(const Serialization<T> & o)
    {
        o.serialize(*this);
        return *this;
    }

    BOStorage & operator<<(Cmd);

    BOStorage & operator<<(char);
    BOStorage & operator<<(unsigned char);
    BOStorage & operator<<(short);
    BOStorage & operator<<(unsigned short);
    BOStorage & operator<<(int);
    BOStorage & operator<<(unsigned int);
    BOStorage & operator<<(long);
    BOStorage & operator<<(unsigned long);

    BOStorage & operator<<(double);
    BOStorage & operator<<(bool);
    BOStorage & operator<<(const std::string &);

    BOStorage & operator<<(const std::wstring &);

#ifndef _LP64
    BOStorage & operator<<(int64_t);
    BOStorage & operator<<(uint64_t);
#endif

    void write(const ConstSegment &); // serve public per Buffer


protected:

    void writeBitFormat();

private:
    Buffer & _data;
    std::stack<size_t> _blocks;
};

template <class X, class Y> BOStorage & operator<<(BOStorage & s, const std::pair<X, Y> & a)
{
    return s << a.first << a.second;
}

template <class X, class Y> BIStorage & operator>>(BIStorage & s, std::pair<X, Y> & a)
{
    return s >> a.first >> a.second;
}

template <class T> BOStorage & operator<<(BOStorage & s, const std::vector<T> & a)
{
    s << a.size();
    for (auto const & i : a) s << i;
    return s;
}

template <class T> BIStorage & operator>>(BIStorage & s, std::vector<T> & a)
{
    size_t z;
    s>>z;
    a.resize(z);
    for (auto & i : a) s >> i;
    return s;
}

template <class T> BOStorage & operator<<(BOStorage & s, const std::list<T> & a)
{
    s << a.size();
    for (auto const & i : a) s << i;
    return s;
}

template <class T> BIStorage & operator>>(BIStorage & s, std::list<T> & a)
{
    a.clear();
    size_t z;
    s>>z;
    T x;

    CXU_LOOP(j, z)
    {
        s >> x;
        a.push_back(x);
    };
    return s;
}

template <class T> BOStorage & operator<<(BOStorage & s, const std::set<T> & a)
{
    s << a.size();
    for (auto const & i : a) s << i;
    return s;
}

template <class T> BIStorage & operator>>(BIStorage & s, std::set<T> & a)
{
    a.clear();
    size_t z;
    s>>z;
    T x;

    CXU_LOOP(j, z)
    {
        s>>x;
        a.insert(x);
    };
    return s;
}

template <class X, class Y> BOStorage & operator<<(BOStorage & s, const std::map<X, Y> & a)
{
    s << a.size();
    for (auto & i : a) s << i;
    return s;
}

template <class X, class Y> BIStorage & operator>>(BIStorage & s, std::map<X, Y> & a)
{
    a.clear();
    size_t z;
    s>>z;
    std::pair<X, Y> t;

    CXU_LOOP(j, z)
    {
        s>>t;
        a.insert(t);
    };
    return s;
}




CXU_NS_END

#endif

//.
