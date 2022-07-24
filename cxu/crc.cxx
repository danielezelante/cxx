// YAL zeldan

#include "crc.hxx"

#include "logger.hxx"

#include "mymacros.hxx"

CXU_NS_BEGIN


static uint8_t s_reflect8(uint8_t x)
{
    uint8_t j = 1;
    uint8_t y = 0;

    for (uint8_t i = 0x80; i; i >>= 1)
    {
        if (x & i) y |= j;
        j <<= 1;
    }
    return y;
}

static uint16_t s_reflect16(uint16_t x)
{
    uint16_t j = 1;
    uint16_t y = 0;

    for (uint16_t i = 0x8000; i; i >>= 1)
    {
        if (x & i) y |= j;
        j <<= 1;
    }
    return y;
}

Crc16poly::Crc16poly(uint16_t poly)
{
    // Compute the remainder of each possible dividend.
    for (int dividend = 0; dividend < 256; ++dividend)
    {
        // Start with the dividend followed by zeros.
        uint16_t remainder = dividend << 8;

        // Perform modulo-2 division, a bit at a time.
        for (uint8_t bit = 8; bit > 0; --bit)
        {
            // Try to divide the current data bit.
            if (remainder & 0x8000)
                remainder = (remainder << 1) ^ poly;
            else
                remainder = (remainder << 1);
        }

        // Store the result into the table.
        _table[dividend] = remainder;
    }

}

void Crc16::operator^=(const ConstSegment & s)
{
    for (size_t j = 0; j < s.size(); ++j)
        * this ^= s[j];
}

void Crc16::operator^=(uint8_t x1)
{
    //LOGGER.debug(Format("Crc16{%$}::operator ^= (%$)") %ptrtoa(this) %utox(x1));
    const uint16_t * table = _poly.getTable();
    const uint8_t x2 = _refin ? s_reflect8(x1) : x1;
    const uint8_t data = x2 ^ (_value >> 8);
    _value = table[data] ^ (_value << 8);
}

void Crc16::operator^=(char x)
{
    return *this ^= static_cast<uint8_t> (x);
}

uint16_t Crc16::operator()() const
{
    return _refout ? s_reflect16(_value) : _value;
}

CXU_NS_END

//.
