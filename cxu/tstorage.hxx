// YAL zeldan

#ifndef CXU_TSTORAGE_HXX
#define CXU_TSTORAGE_HXX

#include <stack>

#include "object.hxx"
#include "mystd.hxx"
#include "memory.hxx"

CXU_NS_BEGIN


class TIStorage : Entity
{
public:
    explicit TIStorage(const Data &);
    virtual ~TIStorage();

    enum Cmd
    {
        BEGIN, END, ABORT
    };

    template <class T> TIStorage & operator>>(Serialization<T> & o)
    {
        o.serialize(*this);
        return *this;
    }

    //TIStorage & operator >> (char &);
    //TIStorage & operator >> (unsigned char &);
    //TIStorage & operator >> (short &);
    //TIStorage & operator >> (unsigned short &);
    TIStorage & operator>>(int &);
    TIStorage & operator>>(unsigned int &);
    TIStorage & operator>>(long &);
    TIStorage & operator>>(unsigned long &);

    TIStorage & operator>>(double &);
    TIStorage & operator>>(bool &);
    TIStorage & operator>>(std::string &);

    TIStorage & operator>>(std::wstring &);

#ifndef _LP64
    TIStorage & operator>>(int64_t &);
    TIStorage & operator>>(uint64_t &);
#endif

    TIStorage & operator>>(const char *);
    TIStorage & operator>>(Cmd);



    void checkSignature();

    char read();
    char peek();


protected:
    void eatWhite();

    char getCurrent() const
    {
        return *(_data.cptr() + _ptr);
    }

private:
    const Data & _data;
    size_t _ptr;
    std::vector<char> _nesting;
    char qread();
    char qpeek();
    const char * ptr() const;
    void qskip(size_t);
    void qskip();

};

class TOStorage : Entity
{
public:
    explicit TOStorage(Buffer &);
    virtual ~TOStorage();

    enum Cmd
    {
        POP, NL, BLANK, BEGIN, END
    };

    enum Radix
    {
        DEC, HEX, BIN
    };

    template <class T> TOStorage & operator<<(const Serialization<T> & o)
    {
        o.serialize(*this);
        return *this;
    }

    //TOStorage & operator << (char);
    //TOStorage & operator << (unsigned char);
    //TOStorage & operator << (short);
    //TOStorage & operator << (unsigned short);
    TOStorage & operator<<(int);
    TOStorage & operator<<(unsigned int);
    TOStorage & operator<<(long);
    TOStorage & operator<<(unsigned long);

    TOStorage & operator<<(double);
    TOStorage & operator<<(bool);
    TOStorage & operator<<(const std::string &);

    TOStorage & operator<<(const std::wstring &);

#ifndef _LP64
    TOStorage & operator<<(int64_t);
    TOStorage & operator<<(uint64_t);
#endif

    TOStorage & operator<<(const char *);
    TOStorage & operator<<(Cmd);
    TOStorage & operator<<(Radix);

    void writeSignature();

    void write(char);
    void write(const char *);


protected:

    char & refCurrent()
    {
        return *(_data.ptr() + _ptr);
    }

    /*
    void throws(const char * exc, const char * file, int line);
    void throws(const char *);
     */

private:
    Buffer & _data;
    size_t _ptr;
    std::stack<Radix> _radix;
    std::stack<char> _nesting;
    bool _needspaceafter;
    bool _newline;
    bool _error;

    void qwrite(char);
    void qwrite(const char *);
    char * cur() const;
    void qskip(size_t);
    void qreserve(size_t);
    void indent();
};

template <class X, class Y> TOStorage & operator<<(TOStorage & s, const std::pair<X, Y> & a)
{
    return s << "(" << a.first << "," << a.second << ")";
}

template <class X, class Y> TIStorage & operator>>(TIStorage & s, std::pair<X, Y> & a)
{
    return s >> "(" >> a.first >> "," >> a.second >> ")";
}

template <class T> TOStorage & operator<<(TOStorage & s, const std::vector<T> & a)
{
    s << "@[" << a.size() << "]" << s.BEGIN;
    for (auto const & it : a) s << s.NL << it << ";";
    s << s.END;
    return s;
}

template <class T> TIStorage & operator>>(TIStorage & s, std::vector<T> & a)
{
    size_t z;
    s >> "@ [" >> z >> "]" >> s.BEGIN;
    a.resize(z);
    for (auto & it : a) s >> it >> ";";
    s >> s.END;
    return s;
}





CXU_NS_END

#endif


//.
