// YAL zeldan

#if __linux__ || __FreeBSD__
// TODO check FreeBSD
// TODO implement Sun, NetBSD, Cygwin

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <termios.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>

#include "serial.hxx"
#include "defs.hxx"
#include "time.hxx"
#include "text.hxx"

#include "mymacros.hxx"

CXU_NS_BEGIN

SerialPort::SerialPort(const char * name, speed_t baud, bool hwhs)
: _bufndx(0)
, _buftot(0)
{
    const int fdx = ::open(name, O_RDWR);
    if (fdx < 0)
        THROWERREXCEPTIONHERES(
                               Format("open(%$, O_RDWR)") % quote(name)
                               );
    setup(FileDescriptor(fdx));

    try
    {
        ERRCALLS(::tcgetattr(+fd(), &_orgtio), Format("tcgetattr(%$, ...)") % fd());

        struct termios tio;
        ZERO(tio);
        cfmakeraw(&tio);

        tio.c_cflag = CS8 | CREAD | CLOCAL;
        ERRCALLS(::cfsetspeed(&tio, baud), "cfsetspeed");
        if (hwhs)
            tio.c_cflag |= CRTSCTS;

        tio.c_cc[VMIN] = 1; // i/o bloccante standard
        tio.c_cc[VTIME] = 0;

        ERRCALLS(::tcflush(+fd(), TCIFLUSH), Format("tcflush(%$, TCIFLUSH)") % fd());
        ERRCALLS(::tcsetattr(+fd(), TCSANOW, &tio), Format("tcsetattr(%$, TCSANOW, ?)") % fd());
    }
    catch (...)
    {
        if (0 > ::close(+fd())) LOGERREXCEPTIONHERE(Logger::e_error);
        throw;
    }
}

SerialPort::~SerialPort()
{
    if (0 > ::tcdrain(+fd()))
        LOGERREXCEPTIONHERE(Logger::e_error);
    if (0 > ::tcsetattr(+fd(), TCSANOW, &_orgtio))
        LOGERREXCEPTIONHERE(Logger::e_error);
}

void SerialPort::writeString(const char * s)
{
    assert(s);
    ConstStrData csd(s);
    write(csd);
}

bool SerialPort::readChar(char & k, size_t readahead, const DeltaTime & timeout)
{

    if (_bufndx >= _buftot)
    {
        _bufndx = 0, _buftot = 0;
        if (readahead > _buffer.size()) _buffer.clear(readahead);

        struct pollfd fds[1];
        fds[0].fd = +fd();
        fds[0].events = POLLIN;
        fds[0].revents = 0;
        const int x = ::ppoll(fds, 1, timeout.timespec_cptr(), NULL);
        if (x < 0)
            THROWERREXCEPTIONHERES(
                                   Format("ppoll(?, 1, %$, NULL)") % timeout
                                   );
        //LOGGER.debug(Format("ppoll => %$") %x);
        if (!x) return false;
        Segment seg(_buffer);
        const int q = ::read(+fd(), seg.ptr(), seg.size());
        if (q < 0)
            THROWERREXCEPTIONHERES(
                                   Format("read(fd=%$, %$, %$)") % fd() % ptrtoa(seg.ptr()) % seg.size()
                                   );
        //LOGGER.debug(Format("read => %$") %q);
        if (!q) return false;

        _buftot = q;
    }

    Segment seg(_buffer);
    k = seg[_bufndx++];
    return true;
}

void SerialPort::drain()
{
    ERRCALL(::tcdrain(+fd()));
}

void SerialPort::flush(int queue)
{
    ERRCALL(::tcflush(+fd(), queue));
}

speed_t SerialPort::s_baudrate(unsigned b)
{
    switch (b)
    {
#define CASE(X) case X : return B##X;

        CASE(50)
        CASE(75)
        CASE(110)
        CASE(134)
        CASE(150)
        CASE(200)
        CASE(300)
        CASE(600)
        CASE(1200)
        CASE(1800)
        CASE(2400)
        CASE(4800)
        CASE(9600)
        CASE(19200)
        CASE(38400)
        CASE(57600)
        CASE(115200)

#if __linux__
                CASE(230400)
                CASE(460800)
                CASE(500000)
                CASE(576000)
                CASE(921600)
                CASE(1000000)
                CASE(1152000)
                CASE(1500000)
                CASE(2000000)
                CASE(2500000)
                CASE(3000000)
                CASE(3500000)
                CASE(4000000)
#endif

#undef CASE

    default: THROWDATAEXCEPTION(Format("invalid baudrate: %$") % b);

    }
    return B0;
}


#if __unix__

void SerialPort::forceDTR(bool x)
{
    const int a = TIOCM_DTR;
    ERRCALLS(::ioctl(+fd(), x ? TIOCMBIS : TIOCMBIC, &a),
             Format("ioctl(%$, %$, TIOCM_DTR)") % fd() % (x ? "TIOCMBIS" : "TIOCMBIC")
             );
}

void SerialPort::forceRTS(bool x)
{
    const int a = TIOCM_RTS;
    ERRCALLS(::ioctl(+fd(), x ? TIOCMBIS : TIOCMBIC, &a),
             Format("ioctl(%$, %$, TIOCM_RTS)") % fd() % (x ? "TIOCMBIS" : "TIOCMBIC")
             );
}

void SerialPort::force_DTR_RTS(bool x)
{
    const int a = TIOCM_DTR | TIOCM_RTS;
    ERRCALLS(::ioctl(+fd(), x ? TIOCMBIS : TIOCMBIC, &a),
             Format("ioctl(%$, %$, TIOCM_DTR|TIOCM_RTS)") % fd() % (x ? "TIOCMBIS" : "TIOCMBIC")
             );
}

bool SerialPort::infoCTS() const
{
    int a = 0;
    ERRCALLS(::ioctl(+fd(), TIOCMGET, &a),
             Format("ioctl(%$, TIOCMGET, ?)") % fd()
             );
    return a & TIOCM_CTS;
}

bool SerialPort::infoDSR() const
{
    int a = 0;
    ERRCALLS(::ioctl(+fd(), TIOCMGET, &a),
             Format("ioctl(%$, TIOCMGET, ?)") % fd()
             );
    return a & TIOCM_DSR;
}

bool SerialPort::infoCAR() const
{
    int a = 0;
    ERRCALLS(::ioctl(+fd(), TIOCMGET, &a),
             Format("ioctl(%$, TIOCMGET, ?)") % fd()
             );
    return a & TIOCM_CAR;
}

bool SerialPort::infoRNG() const
{
    int a = 0;
    ERRCALLS(::ioctl(+fd(), TIOCMGET, &a),
             Format("ioctl(%$, TIOCMGET, ?)") % fd()
             );
    return a & TIOCM_RNG;
}

#endif



CXU_NS_END

#endif

//.
