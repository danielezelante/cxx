// YAL zeldan

#include <memory>

#include "bstorage.hxx"

#include "memory.hxx"
#include "exception.hxx"
#include "mymacros.hxx"
#include "logger.hxx"



CXU_NS_BEGIN


    static const uint8_t s_bstreamuuid[] ={
    0xb2, 0x07, 0x05, 0xe0,
    0x43, 0x25,
    0x4a, 0x3b,
    0x90, 0x43,
    0xf9, 0x64, 0x67, 0xbc, 0xe8, 0x97
};

//**** BIStorage

BIStorage::BIStorage(const Data & data)
: _data(data)
, _ptr(0)
{
    uint8_t myuuid[sizeof (s_bstreamuuid)];
    ZERO(myuuid);
    const ConstNativeData oknda(s_bstreamuuid);
    NativeData mynda(myuuid);
    read(mynda);
    if (oknda != mynda)
        THROWDATAEXCEPTION("invalid bstream uuid");

    uint8_t model[1];
    NativeData modelnda(model);
    read(modelnda);

    if (model[0] != DataModel::getSysSize())
        THROWDATAEXCEPTION("incompatible wordsize");
}

BIStorage::~BIStorage()
{
    if (!_blocks.empty())
    {
        LOGINTERFACEEXCEPTION(Logger::e_error, "unclosed block");
    }

    if (_ptr != _data.size())
    {
        LOGINTERFACEEXCEPTION(Logger::e_warning, "not EOF");
    }
}

const char * BIStorage::cur() const
{
    return _data.cptr() + _ptr;
}

BIStorage & BIStorage::operator>>(Cmd cmd)
{
    switch (cmd)
    {
    case BEGIN:
    {
        size_t base = _ptr;
        size_t z;
        (*this) >> z;
        _blocks.push(base + z);
    };
        break;

    case END:
    {
        ASSERT(!_blocks.empty());
        size_t shld = _blocks.top();
        if (_ptr != shld)
            THROWDATAEXCEPTION(std::string("BIStorage stack underflow"));
        _blocks.pop();
    };
        break;

    case ABORT:
    {
        ASSERT(!_blocks.empty());
        size_t shld = _blocks.top();
        _ptr = shld;
        _blocks.pop();
    };
        break;

    default: ASSERT(false);
        break;
    }
    return *this;
}

void BIStorage::read(const Segment & a)
{
    if (_ptr + a.size() > _data.size())
        THROWDATAEXCEPTION(std::string("BIstorage EOF"));
    a.copy((ConstSegment(_data) + _ptr) & a.size());
    _ptr += a.size();
}

void BIStorage::skip(size_t n)
{
    if (_ptr + n > _data.size())
        THROWDATAEXCEPTION(std::string("BIstorage EOF"));
    _ptr += n;
}

BIStorage & BIStorage::operator>>(unsigned char & n)
{
    NativeData da(n);
    read(da);
    return *this;
}

BIStorage & BIStorage::operator>>(char & n)
{
    NativeData da(n);
    read(da);
    return *this;
}

BIStorage & BIStorage::operator>>(unsigned int & n)
{
    NativeData da(n);
    read(da);
    return *this;
}

BIStorage & BIStorage::operator>>(int & n)
{
    NativeData da(n);
    read(da);
    return *this;
}

BIStorage & BIStorage::operator>>(unsigned long & n)
{
    NativeData da(n);
    read(da);
    return *this;
}

BIStorage & BIStorage::operator>>(long & n)
{
    NativeData da(n);
    read(da);
    return *this;
}

BIStorage & BIStorage::operator>>(bool & n)
{
    NativeData da(n);
    read(da);
    return *this;
}

BIStorage & BIStorage::operator>>(double & n)
{
    NativeData da(n);
    read(da);
    return *this;
}

BIStorage & BIStorage::operator>>(std::string & a)
{
    size_t sz;
    (*this) >> sz;
    a = std::string(cur(), sz);
    skip(sz);
    return *this;
}

BIStorage & BIStorage::operator>>(std::wstring & a)
{
    size_t sz;
    (*this) >> sz;
#ifndef __ARMEL__ 
    a = std::wstring(reinterpret_cast<const wchar_t *> (cur()), sz);
#else
    // per il problema dell'allineamento
    MemArray<wchar_t> x(sz);
    ::memcpy(x.ptr(), cur(), sz * sizeof (wchar_t));
    a = std::wstring(x.cptr());
#endif
    skip(sz * sizeof (wchar_t));
    return *this;
}

#ifndef _LP64

BIStorage & BIStorage::operator>>(int64_t & n)
{
    NativeData da(n);
    read(da);
    return *this;
}

BIStorage & BIStorage::operator>>(uint64_t & n)
{
    NativeData da(n);
    read(da);
    return *this;
}
#endif



//**** BOStorage

BOStorage::BOStorage(Buffer & data)
: _data(data)
{
    _data.clear();

    const ConstNativeData cnd(s_bstreamuuid);
    _data |= cnd;
    const uint8_t model[1] = {
        static_cast<uint8_t> (DataModel::getSysSize())
    };

    const ConstNativeData cmnda(model);
    _data |= cmnda;
}

BOStorage::~BOStorage()
{
    if (!_blocks.empty())
        LOGINTERFACEEXCEPTION(Logger::e_error, "some block left open");
}

void BOStorage::write(const ConstSegment & s)
{
    _data |= s;
}

BOStorage & BOStorage::operator<<(Cmd cmd)
{
    switch (cmd)
    {
    case BEGIN:
    {
        _blocks.push(_data.size());
        const size_t base = 0; // viene impostato da END
        const ConstNativeData nd(base);
        _data |= nd;
    };
        break;

    case END:
    {
        ASSERT(!_blocks.empty());
        size_t prev = _blocks.top();
        _blocks.pop();
        const size_t dist = _data.size() - prev;

        struct t_size
        {
            size_t _;
        } __attribute__ ((packed));
        t_size * p = reinterpret_cast<t_size*> (_data.ptr() + prev);
        p->_ = dist;
    };
        break;

    default: ASSERT(false);
    }
    return *this;
}

BOStorage & BOStorage::operator<<(unsigned char n)
{
    const ConstNativeData nd(n);
    write(nd);
    return *this;
}

BOStorage & BOStorage::operator<<(char n)
{
    const ConstNativeData nd(n);
    write(nd);
    return *this;
}

BOStorage & BOStorage::operator<<(unsigned int n)
{
    const ConstNativeData nd(n);
    write(nd);
    return *this;
}

BOStorage & BOStorage::operator<<(int n)
{
    const ConstNativeData nd(n);
    write(nd);
    return *this;
}

BOStorage & BOStorage::operator<<(unsigned long n)
{
    const ConstNativeData nd(n);
    write(nd);
    return *this;
}

BOStorage & BOStorage::operator<<(long n)
{
    const ConstNativeData nd(n);
    write(nd);
    return *this;
}

BOStorage & BOStorage::operator<<(bool n)
{
    const ConstNativeData nd(n);
    write(nd);
    return *this;
}

BOStorage & BOStorage::operator<<(double n)
{
    const ConstNativeData nd(n);
    write(nd);
    return *this;
}

BOStorage & BOStorage::operator<<(const std::string & a)
{
    const size_t sz = a.size();
    (*this) << sz;
    ConstStringData csd(a);
    write(csd);
    return *this;
}

BOStorage & BOStorage::operator<<(const std::wstring & a)
{
    const size_t sz = a.size();
    (*this) << sz;
    ConstWStringData csd(a);
    write(csd);
    return *this;
}

#ifndef _LP64

BOStorage & BOStorage::operator<<(uint64_t n)
{
    const ConstNativeData nd(n);
    write(nd);
    return *this;
}

BOStorage & BOStorage::operator<<(int64_t n)
{
    const ConstNativeData nd(n);
    write(nd);
    return *this;
}

#endif

CXU_NS_END

//.
