// YAL zeldan

#include <signal.h>

#include <string>
#include <map>

#include <sys/param.h>


#include "memory.hxx"
#include "string.hxx"
#include "text.hxx"
#include "mymacros.hxx"
#include "logger.hxx"
#include "exception.hxx"
#include "mystd.hxx"
#include "base64.hxx"




using namespace std;

CXU_NS_BEGIN

size_t g_totalCopyCount = 0;

DataModel::DataModel(t_size z)
: _size(z)
{
    fillsizes();
}

DataModel::DataModel()
: _size(e_SYS)
{
    fillsizes();
}

void DataModel::fillsizes()
{
    switch (_size)
    {
    case e_SYS:
        _sizeof_char = sizeof (char);
        _sizeof_short = sizeof (short);
        _sizeof_int = sizeof (int);
        _sizeof_long = sizeof (long);
        _sizeof_double = sizeof (double);
        _sizeof_size = sizeof (size_t);
        break;

    case e_ILP32:
        _sizeof_char = 1;
        _sizeof_short = 2;
        _sizeof_int = 4;
        _sizeof_long = 4;
        _sizeof_double = 8;
        _sizeof_size = 4;
        break;

    case e_LP64:
        _sizeof_char = 1;
        _sizeof_short = 2;
        _sizeof_int = 4;
        _sizeof_long = 8;
        _sizeof_double = 8;
        _sizeof_size = 8;
        break;

    default: ASSERT(false);
    }
}


//** ConstStruct

ConstStruct::ConstStruct(const Data & d, const DataModel & m)
: _cdata(d)
, _model(m)
, _ofs(0)
{
}

ConstStruct::~ConstStruct()
{
}

void ConstStruct::seek(size_t z)
{
    if (z > _cdata.size())
        THROWINTERFACEEXCEPTION("out of size");
    _ofs = z;
}

int8_t ConstStruct::getI8()
{
    return getSZAD<int8_t>(sizeof (int8_t));
}

int16_t ConstStruct::getI16()
{
    return getSZAD<int16_t>(sizeof (int16_t));
}

int32_t ConstStruct::getI32()
{
    return getSZAD<int32_t>(sizeof (int32_t));
}

int64_t ConstStruct::getI64()
{
    return getSZAD<int64_t>(sizeof (int64_t));
}

uint8_t ConstStruct::getU8()
{
    return getUZAD<uint8_t>(sizeof (uint8_t));
}

uint16_t ConstStruct::getU16()
{
    return getUZAD<uint16_t>(sizeof (uint16_t));
}

uint32_t ConstStruct::getU32()
{
    return getUZAD<uint32_t>(sizeof (uint32_t));
}

uint64_t ConstStruct::getU64()
{
    return getUZAD<uint64_t>(sizeof (uint64_t));
}

int ConstStruct::getINT()
{
    return getSZAD<int>(_model.sizeof_int());
}

unsigned int ConstStruct::getUINT()
{
    return getUZAD<unsigned int>(_model.sizeof_int());
}

long ConstStruct::getLONG()
{
    return getSZAD<long>(_model.sizeof_long());
}

unsigned long ConstStruct::getULONG()
{
    return getUZAD<unsigned long>(_model.sizeof_long());
}

size_t ConstStruct::getSIZE()
{
    return getUZAD<size_t>(_model.sizeof_size());
}

double ConstStruct::getDOUBLE()
{
    const size_t z = _model.sizeof_double();

    if (z == sizeof (double))
        return getPOD<double>();
    else
        THROWINTERFACEEXCEPTION("unknown double type");
}





//** Struct

Struct::Struct(Data & d, const DataModel & m)
: ConstStruct(d, m)
, _data(d)
{
}

void Struct::setU8(uint8_t x)
{
    setUZAD<uint8_t>(x, sizeof (x));
}

void Struct::setU16(uint16_t x)
{
    setUZAD<uint16_t>(x, sizeof (x));
}

void Struct::setU32(uint32_t x)
{
    setUZAD<uint32_t>(x, sizeof (x));
}

void Struct::setU64(uint64_t x)
{
    setUZAD<uint64_t>(x, sizeof (x));
}

void Struct::setI8(int8_t x)
{
    setSZAD<int8_t>(x, sizeof (x));
}

void Struct::setI16(int16_t x)
{
    setSZAD<int16_t>(x, sizeof (x));
}

void Struct::setI32(int32_t x)
{
    setSZAD<int32_t>(x, sizeof (x));
}

void Struct::setI64(int64_t x)
{
    setSZAD<int64_t>(x, sizeof (x));
}

void Struct::setINT(int x)
{
    setSZAD<int>(x, _model.sizeof_int());
}

void Struct::setUINT(unsigned int x)
{
    setUZAD<unsigned int>(x, _model.sizeof_int());
}

void Struct::setLONG(long x)
{
    setSZAD<long>(x, _model.sizeof_long());
}

void Struct::setULONG(unsigned long x)
{
    setUZAD<unsigned long>(x, _model.sizeof_long());
}

void Struct::setSIZE(size_t x)
{
    setUZAD<size_t>(x, _model.sizeof_size());
}

void Struct::setDOUBLE(double x)
{
    const size_t z = _model.sizeof_double();
    if (z == sizeof (double))
        setPOD<double>(x);
    else
        THROWDATAEXCEPTION("unknown double format");
}


//** DynamicStruct

DynamicStruct::DynamicStruct(Buffer & b, const DataModel & m)
: Struct(b, m)
, _buffer(b)
{
}

void DynamicStruct::addU8(uint8_t x)
{
    const size_t z = _buffer.size();
    _buffer.resize(z + sizeof (x));
    *reinterpret_cast<uint8_t*> (_buffer.ptr() + z) = x;
}

void DynamicStruct::addSIZE(size_t x)
{
    const size_t z = _model.sizeof_size();
    const size_t zb = _buffer.size();
    _buffer.resize(zb + z);
    seek(zb);
    setSIZE(x);
}

void DynamicStruct::addBlob(const ConstSegment & s)
{
    _buffer |= s;
}


//** DataModel

DataModel::t_size DataModel::getRealSize() const
{
    if (_size == e_SYS) return getSysSize();
    return _size;
}

DataModel::t_size DataModel::getSysSize()
{
#ifdef _ILP32
    return e_ILP32;
#endif

#ifdef _LP64
    return e_LP64;
#endif
}

DataModel::t_size DataModel::getSimpleSize() const
{
    if (_size == getSysSize()) return e_SYS;
    return _size;
}


CXU_NS_END


//.
