// YAL zeldan

#ifndef CXU_MEMORY_HXX
#define CXU_MEMORY_HXX

#include <string>
#include <map>
#include <limits>
#include <stdlib.h>
#include <string.h>

#include "object.hxx"
#include "exception.hxx"
#include "defs.hxx"
#include "mystd.hxx"
#include "string.hxx"

#undef min
#undef max


CXU_NS_BEGIN

        extern size_t g_totalCopyCount;

static inline size_t totalCopyCount()
{
    return g_totalCopyCount;
}

template <class T> void constructor(T * ptr)
{
    ::new(ptr) T();
}

template <class T> void constructor(T * ptr, const T & a)
{
    ::new(ptr) T(a);
}

template <class T> void destructor(T * ptr)
{
    ptr->~T();
}

template <class T> T * typealloc()
{
    return reinterpret_cast<T*> (::malloc(sizeof (T)));
}

template <class S, class B> S * subtyperealloc(B * p)
{
    CXU_ASSERT(sizeof (B) <= sizeof (S));
    return reinterpret_cast<S*> (::realloc(p, sizeof (S)));
}

template <class S, class B> B * basesubtyperealloc(B * p)
{
    CXU_ASSERT(sizeof (B) <= sizeof (S));
    return reinterpret_cast<B*> (::realloc(p, sizeof (S)));
}

template <class T> T * typealloc(size_t n)
{
    return reinterpret_cast<T*> (::malloc(n * sizeof (T)));
}

template <class T> T * typerealloc(T * p, size_t n)
{
    return reinterpret_cast<T*> (::realloc(p, n * sizeof (T)));
}

template <class T> void typefree(T * p)
{
    return ::free(p);
}


// new templated and simpler stuff
// for POD only since it has no mean to take base pointer to non-POD array

template <typename T> class Slice;
template <typename T> class ConstSlice;

template <typename T> class Array : public Set<Array<T>, true >
{
    static_assert(__is_pod (T), "Array of non-POD");

public:

    T * ptr()
    {
        if (!_ptr && _size) CXU_THROWINTERFACEEXCEPTION("Array of const");
        CXU_ASSERT(_ptr == _cptr);
        return _ptr;
    }

    const T * cptr() const
    {
        return _cptr;
    }

    size_t size() const
    {
        return _size;
    }

    T & operator[](size_t j)
    {
        CXU_ASSERT(j < size());
        return *(ptr() + j);
    }

    const T & operator[](size_t j) const
    {
        CXU_ASSERT(j < size());
        return *(cptr() + j);
    }

    static bool sdo_Set_equals(const Array<T> & a, const Array<T> & b)
    {
        if (a.size() != b.size()) return false;
        return !::memcmp(a.cptr(), b.cptr(), a.size() * sizeof (T));
    }

    void copyat(size_t k, const ConstSlice<T> & a)
    {
        CXU_ASSERT(size() >= k + a.size());
        ::memcpy(ptr() + k, a.ptr(), a.size() * sizeof (T));
        ++g_totalCopyCount;
    }

protected:

    Array()
    : _ptr(0)
    , _cptr(0)
    , _size(0)
    {
    }

    Array(Array && x)
    : _ptr(x._ptr)
    , _cptr(x._cptr)
    , _size(x._size)
    {
        x._ptr = 0;
        x._cptr = 0;
        x._size = 0;
    }

    Array(T * p, size_t z)
    : _ptr(p)
    , _cptr(p)
    , _size(z)
    {
        CXU_ASSERT(p || !z);
    }

    Array(const T * p, size_t z)
    : _ptr(0)
    , _cptr(p)
    , _size(z)
    {
        CXU_ASSERT(p || !z);
    }

    void setup(T * p, size_t z)
    {

        CXU_ASSERT(p || !z);
        _ptr = p, _cptr = p, _size = z;
    }

    void csetup(const T * p, size_t z)
    {

        CXU_ASSERT(p || !z);
        _ptr = 0, _cptr = p, _size = z;
    }

private:
    T * _ptr; // may be null
    const T * _cptr;
    size_t _size;

};

template <typename T> class DynArray : public Array<T>
{
public:

    virtual ~DynArray() = default;
    DynArray() = default;

    DynArray(DynArray && x) : Array<T>(std::move(x))
    {
    }

    virtual void clear() = 0;
    virtual void clear(size_t) = 0;
    virtual void resize(size_t) = 0;

    void copyin(const ConstSlice<T> &);
    DynArray & operator|=(const ConstSlice<T> &);


protected:

    DynArray(T * p, size_t z) : Array<T>(p, z)
    {
    }


};


#define _CXU_StackArray_magic 0x85cf2b1fadcd332ell

template <typename T> class StackArray : public Array<T>
{
public:

    StackArray(T * p, size_t z, uint64_t magic)
    : Array<T>(p, z)
    {
        if (magic != _CXU_StackArray_magic) abort();
    }
};


#define CXU_STACKARRAY(T,V,Z) \
T CXU_APPLY2(CXU_CAT,V,_cxu_stackarray)[Z]; StackArray<T> V(CXU_APPLY2(CXU_CAT,V,_cxu_stackarray), Z, _CXU_StackArray_magic);

template <typename T> class MemArray final : public DynArray<T>
{
public:

    using Array<T>::ptr;
    using Array<T>::size;

    MemArray() : DynArray<T>(0, 0)
    {
    }

    explicit MemArray(size_t n)
    : DynArray<T>(reinterpret_cast<T*> (::calloc(n, sizeof (T))), n)
    {
        CXU_ASSERT(ptr() || !size());
    }

    virtual ~MemArray()
    {
        ::free(ptr());
    }

    MemArray & operator=(const MemArray & x)
    {
        this->copyin(ConstSlice<T>(x));
        return *this;
    }

    MemArray(const MemArray & x) : DynArray<T>(0, 0)
    {
        this->copyin(ConstSlice<T>(x));
    }

    MemArray(MemArray && x)
    : DynArray<T>(std::move(x))
    {
    }

    virtual void clear() override
    {
        ::free(ptr());
        this->setup(0, 0);
    }

    virtual void clear(size_t n) override
    {
        ::free(ptr());
        this->setup(static_cast<T*> (::calloc(n, sizeof (T))), n);
    }

    virtual void resize(size_t n) override
    {
        T * q = static_cast<T*> (::realloc(ptr(), n * sizeof (T)));
        if (n > size())
            memset(q + size(), 0, (n - size()) * sizeof (T));
        this->setup(q, n);
    }

};

template <class T> class StringArray : public Array<const T>
{
public:

    using Array<T>::ptr;

    StringArray(std::basic_string<T> & s)
    : Array<const T>(s.c_str(), s.size())
    {
    }
    // \0 terminator is not included into array
};

template <class T> class Slice
{
    friend class ConstSlice<T>;

public:


    Slice(const Slice & s) = default;

    Slice(Array<T> & a)
    : _array(a)
    , _base(0)
    , _len(a.size())
    {
    }

    Slice(Array<T> & a, size_t base, size_t len)
    : _array(a)
    , _base(base)
    , _len(len)
    {
        CXU_ASSERT(base + len <= _array.size());
    }

    T & operator[](size_t j) const
    {
        CXU_ASSERT(j < _len);
        return _array[_base + j];
    }

    size_t size() const
    {
        return _len;
    }

    T * ptr() const
    {
        return _array.ptr() + _base;
    }

    size_t top() const
    {
        return _base + _len;
    }

    Slice operator+(size_t k) const
    {
        CXU_ASSERT(k < _len);
        return Slice(_array, _base + k, _len - k);
    }

    Slice & operator+=(size_t k)
    {
        CXU_ASSERT(k < _len);
        _base += k;
        _len -= k;
        return *this;
    }

    Slice operator&(size_t k) const
    {
        CXU_ASSERT(k < _len);
        return Slice(_array, _base, k);
    }

    T & last() const
    {
        CXU_ASSERT(_len);
        return (_array)[_base + _len - 1];
    }

    Slice & operator|=(size_t n)
    {
        CXU_ASSERT(_array.size() >= _base + _len + n);
        _len += n;
        return *this;
    }

    void copy(const ConstSlice<T> & a) const
    {
        CXU_ASSERT(_len == a.size());
        _array.copyat(_base, a);
    }

protected:
    Array<T> & _array;
    size_t _base;
    size_t _len;
};

template <class T> class ConstSlice
{
public:

    ConstSlice(const ConstSlice & s) = default;

    ConstSlice(const Slice<T> & s)
    : _array(s._array)
    , _base(s._base)
    , _len(s._len)
    {
    }

    ConstSlice(const Array<T> & a)
    : _array(a)
    , _base(0)
    , _len(a.size())
    {
    }

    ConstSlice(const Array<T> & a, size_t base, size_t len)
    : _array(a)
    , _base(base)
    , _len(len)
    {
        CXU_ASSERT(base + len <= _array.size());
    }

    const T & operator[](size_t j) const
    {
        CXU_ASSERT(j < _len);
        return _array[_base + j];
    }

    size_t size() const
    {
        return _len;
    }

    const T * ptr() const
    {
        return _array.cptr() + _base;
    }

    size_t top() const
    {
        return _base + _len;
    }

    ConstSlice operator+(size_t k) const
    {
        CXU_ASSERT(_len >= k);
        return ConstSlice(_array, _base + k, _len - k);
    }

    ConstSlice & operator+=(size_t k)
    {
        CXU_ASSERT(k < _len);
        _base += k;
        _len -= k;
        return *this;
    }

    ConstSlice operator&(size_t k) const
    {
        CXU_ASSERT(_len >= k);
        return ConstSlice(_array, _base, k);
    }

    const T & last() const
    {
        CXU_ASSERT(_len);
        return _array[_base + _len - 1];
    }

    ConstSlice & operator|=(size_t n)
    {
        CXU_ASSERT(_array->size() >= _base + _len + n);
        _len += n;
        return *this;
    }



protected:
    const Array<T> & _array;
    size_t _base;
    size_t _len;
};

template <typename T> void DynArray<T>::copyin(const ConstSlice<T> & x)
{
    clear(x.size());
    Array<T>::copyat(0, x);
}

template <typename T> DynArray<T> & DynArray<T>::operator|=(const ConstSlice<T> & x)
{
    const size_t z = this->size();
    resize(z + x.size());
    Array<T>::copyat(z, x);
    return *this;
}


typedef Array<char> Data;
typedef DynArray<char> Buffer;
typedef MemArray<char> Memory;
typedef Slice<char> Segment;
typedef ConstSlice<char> ConstSegment;

class NativeData : public Data
{
public:

    template <class T> NativeData(T & p)
    : Data(reinterpret_cast<char *> (&p), sizeof (T))
    {
        static_assert(__is_pod (T), "NativeData !POD");
    }
};

class ConstNativeData : public Data
{
public:

    template <class T> ConstNativeData(const T & p)
    : Data(reinterpret_cast<const char *> (&p), sizeof (T))
    {
        static_assert(__is_pod (T), "ConstNativeData !POD");
    }

};

class ConstStrData : public Data
{
public:

    ConstStrData(const char * s)
    : Data(s, ::strlen(s))
    {
    }

};

class ConstStringData : public Data
{
public:

    ConstStringData(const std::string & s)
    : Data(s.c_str(), s.size())
    {
    }

};

class ConstWStringData : public Data
{
public:

    ConstWStringData(const std::wstring & s)
    : Data(reinterpret_cast<const char *> (s.c_str()), s.size() * sizeof (wchar_t))
    {
    }

};

class DataModel
{
public:

    enum t_size
    {
        e_SYS = 0, // system word size
        e_ILP32 = 32, // i686 Linux
        e_LP64 = 64, // x86_64 (AMD64 EMT64) Linux
    };

    explicit DataModel(t_size z);
    DataModel();

    size_t sizeof_char() const
    {
        return _sizeof_char;
    }

    size_t sizeof_short() const
    {
        return _sizeof_short;
    }

    size_t sizeof_int() const
    {
        return _sizeof_int;
    }

    size_t sizeof_long() const
    {
        return _sizeof_long;
    }

    size_t sizeof_double() const
    {
        return _sizeof_double;
    }

    size_t sizeof_size() const
    {
        return _sizeof_size;
    }

    static t_size getSysSize();

    t_size getRealSize() const; // never returns e_SYS

    t_size getSimpleSize() const; // returns e_SYS when possible



protected:
    void fillsizes();

    t_size _size;

    size_t _sizeof_char;
    size_t _sizeof_short;
    size_t _sizeof_int;
    size_t _sizeof_long;
    size_t _sizeof_double;
    size_t _sizeof_size;
};

class ConstStruct : Entity
{
public:

    virtual ~ConstStruct();
    ConstStruct(const Data &, const DataModel &);

    void seek(size_t);

    uint8_t getU8();
    uint16_t getU16();
    uint32_t getU32();
    uint64_t getU64();

    int8_t getI8();
    int16_t getI16();
    int32_t getI32();
    int64_t getI64();

    int getINT();
    long getLONG();
    unsigned int getUINT();
    unsigned long getULONG();

    size_t getSIZE();

    double getDOUBLE();

protected:
    const Data & _cdata;
    const DataModel & _model;
    size_t _ofs;

    template <typename T> T getPOD()
    {
        const size_t z = sizeof (T);

        struct t_POD
        {
            T _;
        } __attribute__ ((packed));
        const t_POD * pp = reinterpret_cast<const t_POD *> (_cdata.cptr() + _ofs);
        _ofs += z;
        return pp->_;
    }

    template<typename T> T getUZAD(size_t z)
    {
        if (sizeof (T) == z)
            return getPOD<T>();

        const char * vtl = "value too large";

        switch (z)
        {
        case 1:
        {
            uint8_t x = getPOD<uint8_t>();
            if (sizeof (T) >= sizeof (x)) return static_cast<T> (x);
            if (x > std::numeric_limits<T>::max())
                CXU_THROWDATAEXCEPTION(vtl);
            return static_cast<T> (x);
        };
            break;

        case 2:
        {
            uint16_t x = getPOD<uint16_t>();
            if (sizeof (T) >= sizeof (x)) return static_cast<T> (x);
            if (x > std::numeric_limits<T>::max())
                CXU_THROWDATAEXCEPTION(vtl);
            return static_cast<T> (x);
        };
            break;

        case 4:
        {
            uint32_t x = getPOD<uint32_t>();
            if (sizeof (T) >= sizeof (x)) return static_cast<T> (x);
            if (x > std::numeric_limits<T>::max())
                CXU_THROWDATAEXCEPTION(vtl);
            return static_cast<T> (x);
        };
            break;

        case 8:
        {
            uint64_t x = getPOD<uint64_t>();
            if (sizeof (T) >= sizeof (x)) return x;
            if (x > std::numeric_limits<T>::max())
                CXU_THROWDATAEXCEPTION(vtl);
            return static_cast<T> (x);
        };
            break;

        default: CXU_ASSERT(false);
        }
        return 0;

    }

    template<typename T> T getSZAD(size_t z)
    {
        if (sizeof (T) == z)
            return getPOD<T>();

        const char * vtl = "value too large";

        switch (z)
        {
        case 1:
        {
            int8_t x = getPOD<int8_t>();
            if (sizeof (T) >= sizeof (x)) return static_cast<T> (x);
            if (x < std::numeric_limits<T>::min() || x > std::numeric_limits<T>::max())
                CXU_THROWDATAEXCEPTION(vtl);
            return static_cast<T> (x);
        };
            break;

        case 2:
        {
            int16_t x = getPOD<int16_t>();
            if (sizeof (T) >= sizeof (x)) return static_cast<T> (x);
            if (x < std::numeric_limits<T>::min() || x > std::numeric_limits<T>::max())
                CXU_THROWDATAEXCEPTION(vtl);
            return static_cast<T> (x);
        };
            break;

        case 4:
        {
            int32_t x = getPOD<int32_t>();
            if (sizeof (T) >= sizeof (x)) return static_cast<T> (x);
            if (x < std::numeric_limits<T>::min() || x > std::numeric_limits<T>::max())
                CXU_THROWDATAEXCEPTION(vtl);
            return static_cast<T> (x);
        };
            break;

        case 8:
        {
            int64_t x = getPOD<int64_t>();
            if (sizeof (T) >= sizeof (x)) return static_cast<T> (x);
            if (x < std::numeric_limits<T>::min() || x > std::numeric_limits<T>::max())
                CXU_THROWDATAEXCEPTION(vtl);
            return static_cast<T> (x);
        };
            break;

        default: CXU_ASSERT(false);
        }
        return 0;

    }


};

class Struct : public ConstStruct
{
public:
    Struct(Data &, const DataModel &);

    void setU8(uint8_t);
    void setU16(uint16_t);
    void setU32(uint32_t);
    void setU64(uint64_t);

    void setI8(int8_t);
    void setI16(int16_t);
    void setI32(int32_t);
    void setI64(int64_t);

    void setINT(int);
    void setUINT(unsigned int);
    void setLONG(long);
    void setULONG(unsigned long);

    void setSIZE(size_t);

    void setDOUBLE(double);

    void setBlob(const ConstSegment &);

protected:
    Data & _data;

    template <typename T> void setPOD(T x)
    {

        struct t_POD
        {
            T _;
        } __attribute__ ((packed));
        t_POD * pp = reinterpret_cast<t_POD *> (_data.ptr() + _ofs);
        pp->_ = x;
        _ofs += sizeof (T);
    }

    template<typename T> void setUZAD(T x, size_t z)
    {
        if (sizeof (T) == z)
            return setPOD<T>(x);

        const char * vtl = "value too large";

        switch (z)
        {
        case 1:
        {
            if (sizeof (T) < z)
                if (x > std::numeric_limits<uint8_t>::max())
                    CXU_THROWDATAEXCEPTION(vtl);
            setPOD<uint8_t>(static_cast<uint8_t> (x));
        };
            break;

        case 2:
        {
            if (sizeof (T) < z)
                if (x > std::numeric_limits<uint16_t>::max())
                    CXU_THROWDATAEXCEPTION(vtl);
            setPOD<uint16_t>(static_cast<uint16_t> (x));
        };
            break;

        case 4:
        {
            if (sizeof (T) < z)
                if (x > std::numeric_limits<uint32_t>::max())
                    CXU_THROWDATAEXCEPTION(vtl);
            setPOD<uint32_t>(static_cast<uint32_t> (x));

        };
            break;

        case 8:
        {
            if (sizeof (T) < z)
                if (x > std::numeric_limits<uint64_t>::max())
                    CXU_THROWDATAEXCEPTION(vtl);
            setPOD<uint64_t>(static_cast<uint64_t> (x));

        };
            break;

        default: CXU_ASSERT(false);
        }

    }

    template<typename T> void setSZAD(T x, size_t z)
    {
        if (sizeof (T) == z)
            return setPOD<T>(x);

        const char * vtl = "value too large";

        switch (z)
        {
        case 1:
        {
            if (sizeof (T) < z)
                if (x < std::numeric_limits<int8_t>::min() || x > std::numeric_limits<int8_t>::max())
                    CXU_THROWDATAEXCEPTION(vtl);
            setPOD<int8_t>(static_cast<int8_t> (x));
        };
            break;

        case 2:
        {
            if (sizeof (T) < z)
                if (x < std::numeric_limits<int16_t>::min() || x > std::numeric_limits<int16_t>::max())
                    CXU_THROWDATAEXCEPTION(vtl);
            setPOD<int16_t>(static_cast<int16_t> (x));
        };
            break;

        case 4:
        {
            if (sizeof (T) < z)
                if (x < std::numeric_limits<int32_t>::min() || x > std::numeric_limits<int32_t>::max())
                    CXU_THROWDATAEXCEPTION(vtl);
            setPOD<int32_t>(static_cast<int32_t> (x));
        };
            break;

        case 8:
        {
            if (sizeof (T) < z)
                if (x < std::numeric_limits<int64_t>::min() || x > std::numeric_limits<int64_t>::max())
                    CXU_THROWDATAEXCEPTION(vtl);
            setPOD<int64_t>(static_cast<int64_t> (x));
        };
            break;

        default: CXU_ASSERT(false);
        }

    }

};

class DynamicStruct : public Struct
{
public:
    DynamicStruct(Buffer &, const DataModel &);

    void addU8(uint8_t);
    void addU16(uint16_t);
    void addU32(uint32_t);

    void addI8(int8_t);
    void addI16(int16_t);
    void addI32(int32_t);

    void addSIZE(size_t);

    void addBlob(const ConstSegment &);

protected:
    Buffer & _buffer;
};






CXU_NS_END

#endif

//.
