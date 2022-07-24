// YAL zeldan

#ifndef CXU_CRC_HXX
#define CXU_CRC_HXX

#include <stdint.h>
#include <stdlib.h>

#include "memory.hxx"

CXU_NS_BEGIN


class Crc16poly : Entity
{
protected:
    uint16_t _table[256];

public:
    explicit Crc16poly(uint16_t poly);

    static const uint16_t CAN = 0x4599;
    static const uint16_t ANSI = 0x8005;
    static const uint16_t CCITT = 0x1021;
    static const uint16_t T10DIF = 0x8BB7;
    static const uint16_t DNP = 0x3D65;
    static const uint16_t DECT = 0x0589;

    const uint16_t * getTable() const
    {
        return _table;
    }

};

class Crc16 : Entity
{
public:

    Crc16(const Crc16poly & poly, uint16_t start, bool refin, bool refout)
    : _poly(poly)
    , _refin(refin)
    , _refout(refout)
    , _value(start)
    {
    }


    void operator^=(const ConstSegment &);
    void operator^=(uint8_t);
    void operator^=(char);

    uint16_t operator()() const;

protected:
    const Crc16poly & _poly;
    const bool _refin;
    const bool _refout;
    uint16_t _value;

};


CXU_NS_END

#endif

//.
