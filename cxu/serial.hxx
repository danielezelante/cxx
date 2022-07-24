// YAL zeldan

#ifndef CXU_SERIAL_H
#define CXU_SERIAL_H


#if __linux__ || __FreeBSD__

// TODO Sun, NetBSD, cygwin

#include <termios.h>
#include <unistd.h>

#include "object.hxx"
#include "memory.hxx"
#include "file.hxx"

CXU_NS_BEGIN

class SerialPort
: public RealFile
, public ZableFile
{
public:

    SerialPort(const char * device, speed_t baud, bool hwhs);
    virtual ~SerialPort();
    void writeString(const char * s);

    bool readChar(char & k, size_t readahead, const DeltaTime & timeout);

    void drain(); // force output
    void flush(int queue); // discard input/output

    static speed_t s_baudrate(unsigned);

    void forceDTR(bool);
    void forceRTS(bool);

    void force_DTR_RTS(bool); // atomically equals

    bool infoCTS() const;
    bool infoDSR() const;
    bool infoCAR() const;
    bool infoRNG() const;


protected:

    size_t _bufndx;
    size_t _buftot;
    struct termios _orgtio;
    Memory _buffer;


};




CXU_NS_END

#endif

#endif

//.
