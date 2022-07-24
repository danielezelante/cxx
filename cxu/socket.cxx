// YAL zeldan

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/ioctl.h>

#include <net/if.h>

#include <sys/socket.h>
#include <sys/un.h>

#if __linux__
#include <net/if_arp.h>
#endif

#include "socket.hxx"

#include "exception.hxx"
#include "defs.hxx"
#include "globals.hxx"
#include "logger.hxx"
#include "string.hxx"
#include "thread.hxx"
#include "mystd.hxx"
#include "mymacros.hxx"

using namespace std;

CXU_NS_BEGIN


std::string getHostname()
{
    const size_t n = 1024;
    char a[n];
    ERRCALLS(::gethostname(a, n), "gethostname");
    return std::string(a);
}




// Inet4Address

Inet4Address::Inet4Address()
{
    ZERO(_address);
}

const string Inet4Address::octets() const
{
    char buf[64];
    if (!inet_ntop(AF_INET, &_address, buf, sizeof (buf)))
        THROWERREXCEPTIONHERES(
                               Format("inet_ntop(AF_INET, ?, ?, %$)") % sizeof (buf)
                               );
    return string(buf);
}

std::string Inet4Address::str() const
{
    return octets();
}

Inet4Address::Inet4Address(const char * a)
{
    if (!inet_aton(a, &_address)) THROWDATAEXCEPTION(Format("invalid IPv4 address: %$") % a);
}

Inet4Address::Inet4Address(const std::string & a)
{
    if (!inet_aton(a.c_str(), &_address)) THROWDATAEXCEPTION(Format("invalid IPv4 address: %$") % a);
}

Inet4Address Inet4Address::any()
{
    in_addr x;
    ZERO(x);
    x.s_addr = htonl(INADDR_ANY);
    return Inet4Address(x);
}

Inet4Address Inet4Address::broadcast()
{
    in_addr x;
    ZERO(x);
    x.s_addr = htonl(INADDR_BROADCAST);
    return Inet4Address(x);
}

Inet4Address Inet4Address::loopback()
{
    in_addr x;
    ZERO(x);
    x.s_addr = htonl(INADDR_LOOPBACK);
    return Inet4Address(x);
}

const in_addr * Inet4Address::address() const
{
    return &_address;
}

int Inet4Address::sdo_TotalOrder_compare(const Inet4Address & x, const Inet4Address & y)
{
    if (x.address()->s_addr < y.address()->s_addr) return -1;
    if (x.address()->s_addr > y.address()->s_addr) return +1;
    return 0;
}





// Inet6Address

Inet6Address::Inet6Address()
{
    ZERO(_address);
}

const string Inet6Address::octets() const
{
    char buf[64]; //dvorebbe bastare anche per IPV6
    if (!inet_ntop(AF_INET6, &_address, buf, sizeof (buf)))
        THROWERREXCEPTIONHERES(
                               Format("inet_ntop(AF_INET6, ?, ?, %$)") % sizeof (buf)
                               );
    return string(buf);
}

std::string Inet6Address::str() const
{
    return octets();
}

Inet6Address::Inet6Address(const char * a)
{
    const int w = inet_pton(AF_INET6, a, &_address);
    if (w < 0)
        THROWERREXCEPTIONHERES(
                               Format("inet_pton(AF_INET6, %$, ?)") % quote(a)
                               );
    if (!w) THROWDATAEXCEPTION(Format("invalid IPv6 address: %$") % quote(a));
}

Inet6Address::Inet6Address(const string & s)
{
    const int w = inet_pton(AF_INET6, s.c_str(), &_address);
    if (w < 0)
        THROWERREXCEPTIONHERES(
                               Format("inet_pton(AF_INET6, %$, ?)") % quote(s)
                               );
    if (!w) THROWDATAEXCEPTION(Format("invalid IPv6 address: %$") % quote(s));
}

Inet6Address Inet6Address::any()
{
    const in6_addr x = IN6ADDR_ANY_INIT;
    return Inet6Address(x);
}

Inet6Address Inet6Address::loopback()
{
    const in6_addr x = IN6ADDR_LOOPBACK_INIT;
    return Inet6Address(x);
}

const in6_addr * Inet6Address::address() const
{
    return &_address;
}

int Inet6Address::sdo_TotalOrder_compare(const Inet6Address & x, const Inet6Address & y)
{
    return memcmp(x.address()->s6_addr, y.address()->s6_addr, 16);
    // 16 da documentazione
}





//** SockAddress

SockAddress::SockAddress()
{
    clear();
}

void SockAddress::clear()
{
    ZERO(_sockaddr);
    ptr_sockaddr()->sa_family = AF_UNSPEC;
#ifdef __FreeBSD__
    ptr_sockaddr()->sa_len = 0;
#endif
}

size_t SockAddress::length() const
{
    switch (domain())
    {
    case AF_UNSPEC: return 0;
    case AF_INET: return sizeof (sockaddr_in);
    case AF_INET6: return sizeof (sockaddr_in6);
    default: ASSERT(false);
    }

    return 0;
}

std::string SockAddress::str() const
{
    switch (domain())
    {
    case AF_UNSPEC: return "UNSPEC";
    case AF_INET: return Inet4SockAddress(*this).octets();
        break;
    case AF_INET6: return Inet6SockAddress(*this).octets();
        break;
    case AF_UNIX: return LocalSockAddress(*this).name();
        break;
    }
    return domain_label() + ":address";
}

SockAddress SockAddress::withPort(unsigned short port) const
{
    switch (domain())
    {
    case AF_INET:
        return Inet4SockAddress(Inet4SockAddress(*this).address(), port);
        break;

    case AF_INET6:
        return Inet6SockAddress(Inet6SockAddress(*this).address(), port);
        break;

    default: return *this;
    }
}

bool SockAddress::equalsIgnorePort(const SockAddress & x, const SockAddress & y)
{
    const auto xd = x.domain();
    const auto yd = y.domain();
    if (xd != yd) return false;

    switch (xd)
    {
    case AF_INET:
        return Inet4SockAddress(x).address() == Inet4SockAddress(y).address();
        break;

    case AF_INET6:
        return Inet6SockAddress(x).address() == Inet6SockAddress(y).address();
        break;

    default: return x == y;
    }


}

Inet4SockAddress::Inet4SockAddress(const SockAddress & s)
: SockAddress(s)
{
    if (s.domain() != AF_INET)
        THROWINTERFACEEXCEPTION(Format("wrong domain, exp=%$, got=%$")
                                % s_domain_label(AF_INET)
                                % s.domain_label()
                                );
}

Inet6SockAddress::Inet6SockAddress(const SockAddress & s)
: SockAddress(s)
{
    if (s.domain() != AF_INET6)
        THROWINTERFACEEXCEPTION(Format("wrong domain, exp=%$, got=%$")
                                % s_domain_label(AF_INET6)
                                % s.domain_label()
                                );
}

LocalSockAddress::LocalSockAddress(const SockAddress & s)
: SockAddress(s)
{
    if (s.domain() != AF_UNIX)
        THROWINTERFACEEXCEPTION(Format("wrong domain, exp=%$, got=%$")
                                % s_domain_label(AF_UNIX)
                                % s.domain_label()
                                );
}

std::string LocalSockAddress::name() const
{
    ASSERT(false);
    return "TODO";

}

int LocalSockAddress::sdo_TotalOrder_compare_impl(const LocalSockAddress &, const LocalSockAddress &)
{
    ASSERT(false);
    return 0;
}

const std::string Inet4SockAddress::octets() const
{
    char host[256];
    char serv[256];
    const int x = ::getnameinfo(ptr_sockaddr(), length(), host, DIM(host), serv, DIM(serv), NI_NUMERICHOST | NI_NUMERICSERV);
    if (x) THROWDATAEXCEPTION(Format("getnameinfo => %$") % gai_strerror(x));
    return Format("%$:%$") % host % serv;
}

const std::string Inet6SockAddress::octets() const
{
    char host[256];
    char serv[256];
    const int x = ::getnameinfo(ptr_sockaddr(), length(), host, DIM(host), serv, DIM(serv), NI_NUMERICHOST | NI_NUMERICSERV);
    if (x) THROWDATAEXCEPTION(Format("getnameinfo => %$") % gai_strerror(x));
    return Format("[%$]:%$") % host % serv;
}

unsigned short Inet4SockAddress::port() const
{
    return htons(sax()->sin_port);
}

unsigned short Inet6SockAddress::port() const
{
    return htons(sax()->sin6_port);
}

Inet4Address Inet4SockAddress::address() const
{
    return Inet4Address(sax()->sin_addr);
}

Inet6Address Inet6SockAddress::address() const
{
    return Inet6Address(sax()->sin6_addr);
}

int Inet4SockAddress::sdo_TotalOrder_compare_impl(const Inet4SockAddress & x, const Inet4SockAddress & y)
{
    const int w = Inet4Address::compare(x.address(), y.address());
    if (w) return w;

    const auto px = x.port();
    const auto py = y.port();

    if (px < py) return -1;
    if (px > py) return +1;

    return 0;

}

int Inet6SockAddress::sdo_TotalOrder_compare_impl(const Inet6SockAddress & x, const Inet6SockAddress & y)
{
    const int w = Inet6Address::compare(x.address(), y.address());
    if (w) return w;

    const auto px = x.port();
    const auto py = y.port();

    if (px < py) return -1;
    if (px > py) return +1;

    return 0;

}

SockAddress::SockAddress(const sockaddr * a, size_t len)
{
    //fprintf(stderr, "SockAddress(%p, %zu) \n", a, len);
    const int d = a->sa_family;
    size_t wlen = 0;

    switch (d)
    {
    case AF_INET: wlen = sizeof (sockaddr_in);
        break;
    case AF_INET6: wlen = sizeof (sockaddr_in6);
        break;
    case AF_UNIX: wlen = sizeof (sockaddr_un);
        break;
    default: THROWINTERFACEEXCEPTION(Format("invalid sin_family = %$") % d);
    }

    if (wlen != len)
        THROWINTERFACEEXCEPTION(Format("invalid sockaddr len, got=%$, exp=%$") % len % wlen);

    memcpy(&_sockaddr, a, len);
}

Inet4SockAddress::Inet4SockAddress(const Inet4Address & addr, unsigned short p)
{
    ZERO(_sockaddr);
    sockaddr_in * sa = sax();
    sa->sin_family = AF_INET;
    sa->sin_port = htons(p);
    sa->sin_addr = *addr.address();
}

Inet6SockAddress::Inet6SockAddress(const Inet6Address & addr, unsigned short p, int scopeid)
{
    ZERO(_sockaddr);
    sockaddr_in6 * sa = sax();
    sa->sin6_family = AF_INET6;
    sa->sin6_port = htons(p);
    sa->sin6_addr = *addr.address();
    sa->sin6_flowinfo = 0; // cablato per ora
    sa->sin6_scope_id = scopeid;
}

int SockAddress::sdo_TotalOrder_compare(const SockAddress & x, const SockAddress & y)
{
    const int dx = x.domain();
    const int dy = y.domain();
    if (dx < dy) return -1;
    if (dx > dy) return +1;


    switch (dx)
    {
    case AF_UNSPEC: return 0;
    case AF_UNIX:
        return LocalSockAddress::sdo_TotalOrder_compare_impl
                (LocalSockAddress(x), LocalSockAddress(y));
    case AF_INET:
        return Inet4SockAddress::sdo_TotalOrder_compare_impl
                (Inet4SockAddress(x), Inet4SockAddress(y));
    case AF_INET6:
        return Inet6SockAddress::sdo_TotalOrder_compare_impl
                (Inet6SockAddress(x), Inet6SockAddress(y));

    default:
        THROWINTERFACEEXCEPTION(Format("uncomparable domain: %$")
                                % x.domain_label());
    }
    return 0;
}

bool SockAddress::is() const
{
    return domain() != AF_UNSPEC;
}

const std::string SockAddress::domain_label() const
{
    return s_domain_label(domain());
}


//** Socket

Socket::~Socket()
{
}

void Socket::setsockopt_int(int level, int optname, int value)
{
    ERRCALLS(::setsockopt(+fd(), level, optname, &value, sizeof (value)),
             Format("setsockopt(%$, ...)") % fd()
             );
}

void Socket::setsockopt_sock_int(int optname, int value)
{
    setsockopt_int(SOL_SOCKET, optname, value);
}

void Socket::setsockopt_keepalive(bool x)
{
    setsockopt_sock_int(SO_KEEPALIVE, x ? 1 : 0);
}



//** TcpSocket

#if 0

// implement in hier

void TcpSocket::setsockopt_tcp_int(int optname, int value)
{
    setsockopt_int(IPPROTO_TCP, optname, value);
}

void TcpSocket::setsockopt_nodelay(bool x)
{
    setsockopt_tcp_int(TCP_NODELAY, x ? 1 : 0);
}


#if __linux__ || __FreeBSD__ || __SunOS__

void TcpSocket::setsockopt_keepidle(const DeltaTime & x)
{
    setsockopt_tcp_int(TCP_KEEPIDLE, x.tot_s());
}

void TcpSocket::setsockopt_keepintvl(const DeltaTime & x)
{
    setsockopt_tcp_int(TCP_KEEPINTVL, x.tot_s());
}

void TcpSocket::setsockopt_keepcnt(unsigned x)
{
    setsockopt_tcp_int(TCP_KEEPCNT, x);
}
#endif

#endif

//** SockStream

SockStream::SockStream(const SockAddress & address)
: _slave(false)
{
    const int fdx =
#if __linux__ 
            ::socket(address.domain(), SOCK_STREAM | SOCK_CLOEXEC, 0);
#else
            ::socket(address.domain(), SOCK_STREAM, 0);
#endif
    if (fdx < 0)
        THROWERREXCEPTIONHERES(
                               Format("socket(%$, SOCK_STREAM, 0")
                               % address.domain_label()
                               );
#if ! __linux__
    ::fcntl(fdx, F_SETFD, FD_CLOEXEC);
#endif
    setup(FileDescriptor(fdx));
}

SockStream::SockStream(const FileDescriptor & x)
: _slave(true)
{
    setup(x);
}

SockStream::~SockStream()
{
    try
    {
        if (!_slave) close();
        else
        {
            if (!!fd())
                LOGGER.error("_slave not closed");
        }
    }
    catch (const Exception & ex)
    {
        ex.log(Logger::e_warning);
    }
}

void SockStream::shutdown()
{
    ERRCALLS(::shutdown(+fd(), SHUT_RDWR), Format("shutdown(%$, SHUT_RDWR)") % fd());
}



//** SockStreamClient

SockStreamClient::~SockStreamClient()
{
    if (_connecting) LOGGER.warning("close when connecting");
}

SockStreamClient::SockStreamClient(const SockAddress & a)
: SockStream(a)
, _address(a)
, _connecting(false)
, _connected(false)
{
}

bool SockStreamClient::close()
{
    const bool x = SockStream::close();
    _connected = false;
    _connecting = false;
    return x;
}

bool SockStreamClient::close_nothrow()
{
    const bool x = SockStream::close_nothrow();
    _connected = false;
    _connecting = false;
    return x;
}

void SockStreamClient::connect()
{
    if (_connected) THROWINTERFACEEXCEPTION("already connected");
    if (_connecting) THROWINTERFACEEXCEPTION("already connecting");
    ERRCALLS(
             ::connect(+fd(), _address.ptr_sockaddr(), _address.length()),
             Format("connect(fd=%$, %$)") % fd() % _address
             );
    _connected = true;
}

void SockStreamClient::fcntl_block()
{
    const int a = ::fcntl(+fd(), F_GETFL, 0);
    if (a < 0)
        THROWERREXCEPTIONHERES(
                               Format("fcntl(fd=%$, F_GETFL, 0)") % fd()
                               );
    ERRCALLS(::fcntl(+fd(), F_SETFL, a & ~O_NONBLOCK), Format("fcntl(%$, ...~O_NONBLOCK)") % fd());
}

void SockStreamClient::fcntl_nonblock()
{
    const int a = ::fcntl(+fd(), F_GETFL, 0);
    if (a < 0)
        THROWERREXCEPTIONHERES(
                               Format("fcntl(fd=%$, F_GETFL, 0)") % fd()
                               );
    ERRCALLS(::fcntl(+fd(), F_SETFL, a | O_NONBLOCK), Format("fcntl(%$m ...O_NONBLOCK)") % fd());
}

bool SockStreamClient::connectAsync()
{
    if (_connected) THROWINTERFACEEXCEPTION("already connected");
    if (_connecting) THROWINTERFACEEXCEPTION("already connecting");

    fcntl_nonblock();

    const int x = ::connect(+fd(), _address.ptr_sockaddr(), _address.length());
    if (x < 0)
    {
        _connecting = true;
        if (errno == EINPROGRESS) return false;
        THROWERREXCEPTIONHERES(
                               Format("connect(fd=%$, <%$>)") % fd() % _address
                               );
    }

    fcntl_block();
    _connected = true;
    return true;
}

void SockStreamClient::connectComplete()
{
    if (_connected) THROWINTERFACEEXCEPTION("already connected");
    if (!_connecting) THROWINTERFACEEXCEPTION("not connecting");

    int se = 0;
    socklen_t sel = sizeof (se);
    ERRCALLS(::getsockopt(+fd(), SOL_SOCKET, SO_ERROR, &se, &sel), Format("getsockopt(%$, ...ERROR...)") % fd());
    if (sel != sizeof (se))
        THROWOSEXCEPTION(Format("getsockopt(%$, SOL_SOCKET, SO_ERROR, ...) optlen exp=%$ got=%$") % fd() % sizeof (se) % sel);

    fcntl_block();
    _connecting = false;

    if (se)
    {
        _connecting = false;
        THROWERREXCEPTIONS(se, "connect/SO_ERROR");
    }

    _connected = true;

}

int SockStreamClient::connectAborted()
{
    if (_connected) THROWINTERFACEEXCEPTION("already connected");
    if (!_connecting) THROWINTERFACEEXCEPTION("not connecting");

    int se = 0;
    socklen_t sel = sizeof (se);
    ERRCALLS(::getsockopt(+fd(), SOL_SOCKET, SO_ERROR, &se, &sel),
             Format("getsockopt(%$, SOL_SOCKET, SO_ERROR, ?, ?)") % fd()
             );
    if (sel != sizeof (se))
        THROWOSEXCEPTION(Format("getsockopt(%$, SOL_SOCKET, SO_ERROR, ...) optlen exp=%$ got=%$") % fd() % sizeof (se) % sel);

    LOGGER.debug(Format("connectAborted, SO_ERROR=%$") % ErrException::s_text(se));

    fcntl_block();
    _connecting = false;
    return se;
}


//** SockStreamServer

SockStreamServer::SockStreamServer(const SockAddress & address, bool reuse, int backlog)
: _address(address)
{
    const int fdx =
#if __linux__
            ::socket(address.domain(), SOCK_STREAM | SOCK_CLOEXEC, 0);
#else
            ::socket(address.domain(), SOCK_STREAM, 0);
#endif
    if (fdx < 0)
        THROWERREXCEPTIONHERES(
                               Format("socket(%$, SOCK_STREAM, 0)") % address.domain_label()
                               );

#if ! __linux__
    ::fcntl(fdx, F_SETFD, FD_CLOEXEC);
#endif
    setup(FileDescriptor(fdx));

    try
    {
        if (reuse)
        {
            int val = 1;
            ERRCALLS(
                     ::setsockopt(+fd(), SOL_SOCKET, SO_REUSEADDR, &val, sizeof (val)),
                     Format("setsockopt(%$, SOL_SOCKET, SO_REUSEADDR, <true>)")
                     );
        }
        bind_listen(backlog);
    }
    catch (const Exception & ex)
    {
        close_nothrow();
        throw;
    }

}

void SockStreamServer::bind_listen(int backlog)
{
    ERRCALLS(
             ::bind(+fd(), _address.ptr_sockaddr(), _address.length()),
             Format("bind(%$, <%$>") % fd() % _address
             );
    ERRCALLS(
             ::listen(+fd(), backlog),
             Format("listen(%$, %$)") % fd() % backlog
             );
}

SockStreamServer::~SockStreamServer()
{
    try
    {
        for (auto it : _slaves) it->close();
        for (auto it : _slaves) delete it;
        close_nothrow();
    }
    catch (const Exception & ex)
    {
        ex.log(Logger::e_warning);
    }
    catch (...)
    {
        LOGGER.error("unknown exception");
    }
}

SockStream * SockStreamServer::newAccept(SockAddress & addr)
{
    socklen_t addrlen = addr.storage_length();
    int s;
    ERRCALLS(
             s = ::accept(+fd(), addr.ptr_sockaddr(), &addrlen),
             Format("accept(%$, ...)") % fd()
             );

    // TODO in Unix Socket come usare addrlen

    if (addrlen != static_cast<socklen_t> (addr.length()))
        THROWOSEXCEPTION(Format("accept addrlen=%$ != addr.length()=%$") % addrlen % addr.length());

    SockStream * client = new SockStream(FileDescriptor(s));
    _slaves.insert(client);
    return client;

}

void SockStreamServer::deleteClose(SockStream * s)
{
    if (!s->_slave)
        THROWINTERFACEEXCEPTION("!_slave)");
    if (!_slaves.erase(s))
        THROWINTERFACEEXCEPTION("mismatched socket");
    s->close();
    delete s;
}

short SockStreamServer::events() const
{
    return POLLIN;
}

bool SockStreamServer::match(short x) const
{
    return x & POLLIN;
}





//** SockDgram

SockDgram::SockDgram(int domain)
{
    const int fdx =
#if __linux__
            ::socket(domain, SOCK_DGRAM | SOCK_CLOEXEC, 0);
#else
            ::socket(domain, SOCK_DGRAM, 0);
#endif
    if (fdx < 0)
        THROWERREXCEPTIONHERES(
                               Format("socket(%$, SOCK_DGRAM, 0)") % SockAddress::s_domain_label(domain)
                               );

#if ! __linux__
    ::fcntl(fdx, F_SETFD, FD_CLOEXEC);
#endif
    setup(FileDescriptor(fdx));
    try
    {
        int x = 0;
        socklen_t len = sizeof (x);
        ERRCALLS(
                 ::getsockopt(+fd(), SOL_SOCKET, SO_RCVBUF, &x, &len),
                 Format("getsocktop(%$, SOL_SOCKET, SO_RCVBUF, ...)") % fd()
                 );
        if (len != sizeof (x)) THROWOSEXCEPTION("getsockopt");
        _rcvsize = x;
        LOGGER.debug(Format("SockDgram() rcvsize=%$ len=%$") % _rcvsize % len);
    }
    catch (const Exception &)
    {
        close_nothrow();
        throw;
    }

}

SockDgram::~SockDgram()
{
    try
    {
        close_nothrow();
    }
    catch (const Exception & ex)
    {
        ex.log(Logger::e_warning);
    }
    catch (...)
    {
        LOGGER.error("unknown exception");
    }
}

size_t SockDgram::recvfrom(const Segment & seg, SockAddress * sak)
{
    //if(!_bound) THROWINTERFACEEXCEPTION("!_bound");
    //seg.clear(_rcvsize);

    struct sockaddr_storage sat;
    ZERO(sat);
    socklen_t salen = sizeof (sat);
    struct sockaddr * sa = reinterpret_cast<struct sockaddr *> (&sat);

    ssize_t readen;
    ERRCALLS(
             readen = ::recvfrom(
                                 +fd(),
                                 seg.ptr(),
                                 seg.size(),
                                 0,
                                 sa,
                                 &salen
                                 ),
             Format("recvfrom(fd=%$, %$, %$, 0, ?)")
             % fd() % ptrtoa(seg.ptr()) % seg.size()
             );

    if (sak) *sak = SockAddress(sa, salen);
    return readen;
}

SockDgramServer::SockDgramServer(const SockAddress & a, bool reuse)
: SockDgram(a.domain())
{
    if (reuse)
    {
        int val = 1;
        ERRCALLS(
                 ::setsockopt(+fd(), SOL_SOCKET, SO_REUSEADDR, &val, sizeof (val)),
                 Format("setsockopt(fd=%$, SOL_SOCKET, SO_REUSEADDR, <true>)")
                 % fd()
                 );
    }
    TRACE(Format("bind address size = %$") % a.length());
    ERRCALLS(
             ::bind(+fd(), a.ptr_sockaddr(), a.length()),
             Format("fd=%$, address=%$") % fd() % a
             );
}

void SockDgramServer::sendto(const ConstSegment & seg, const SockAddress & a)
{
    socklen_t len = a.length();
    ssize_t x;
    ERRCALLS(
             x = ::sendto(+fd(), seg.ptr(), seg.size(), 0, a.ptr_sockaddr(), len),
             Format("sendto(fd=%$, %$, %$, 0, <%$>")
             % fd() % ptrtoa(seg.ptr()) % seg.size() % a
             );
    if (len != static_cast<socklen_t> (a.length()))
        THROWOSEXCEPTION(Format("address length %$!=%$") % len % a.length());
    if (x != static_cast<ssize_t> (seg.size()))
        THROWOSEXCEPTION(Format("message size %$!=%$") % x % seg.size());

}

SockDgramClient::SockDgramClient(const SockAddress & a)
: SockDgram(a.domain())
{
    TRACE(Format("connect address size = %$") % a.length());
    ERRCALLS(
             ::connect(+fd(), a.ptr_sockaddr(), a.length()),
             Format("connect(fd=%$, <%$>)") % fd() % a
             );
}

void SockDgramClient::send(const ConstSegment & seg)
{
    ssize_t x;
    ERRCALLS(
             x = ::send(+fd(), seg.ptr(), seg.size(), 0),
             Format("send(fd=%$, %$, %$, 0)") % fd() % ptrtoa(seg.ptr()) % seg.size()
             );
    if (x != static_cast<ssize_t> (seg.size()))
        THROWOSEXCEPTION(Format("message size %$!=%$") % x % seg.size());
}

size_t SockDgramClient::recv(const Segment & seg)
{
    ssize_t x;
    ERRCALLS(
             x = ::recv(+fd(), seg.ptr(), seg.size(), 0),
             Format("recv(%$, %$, %$, 0)") % fd() % ptrtoa(seg.ptr()) % seg.size()
             );
    return x;
}

size_t SockDgramClient::recvnb(const Segment & seg, bool & wb)
{
    const ssize_t x = ::recv(+fd(), seg.ptr(), seg.size(), MSG_DONTWAIT);
    const int eee = errno;
    if (x < 0)
    {
        if (eee == EAGAIN || eee == EWOULDBLOCK)
        {
            wb = true;
            return 0;
        }
        THROWERREXCEPTIONS(
                           eee,
                           Format("recv(fd=%$, %$, %$, MSG_DONTWAIT)")
                           % fd() % ptrtoa(seg.ptr()) % seg.size()
                           );
    }
    wb = false;
    return x;
}


//** GetAddrInfo

GetAddrInfo::~GetAddrInfo()
{
    if (_result)
        ::freeaddrinfo(_result);
}

GetAddrInfo::GetAddrInfo(const std::string & node, const std::string & service)
: _node(node)
, _service(service)
, _result(0)
, _ptr(0)
{
    ZERO(_hints);

    // default as in ::getaddrinfo manpage
    _hints.ai_family = AF_UNSPEC;
    _hints.ai_flags |= AI_ADDRCONFIG;
#if __linux__
    _hints.ai_flags |= AI_V4MAPPED;
#endif
}

void GetAddrInfo::doit()
{
    checkAval();
    const int x = ::getaddrinfo(
                                _node.empty() ? 0 : _node.c_str(),
                                _service.empty() ? 0 : _service.c_str(),
                                &_hints,
                                &_result
                                );
    if (x) CXU_THROWGAIEXCEPTION(x, _node, _service);
}

void GetAddrInfo::hintFamily(int x)
{
    checkAval();
    _hints.ai_family = x;
}

void GetAddrInfo::hintSocktype(int x)
{
    checkAval();
    _hints.ai_socktype = x;
}

void GetAddrInfo::hintProtocol(int x)
{
    checkAval();
    _hints.ai_protocol = x;
}

void GetAddrInfo::hintFlags(int x)
{
    checkAval();
    _hints.ai_flags = x;
}

bool GetAddrInfo::next()
{
    checkDone();
    if (_ptr) _ptr = _ptr->ai_next;
    else _ptr = _result;
    return _ptr != 0;
}

int GetAddrInfo::getFlags() const
{
    checkPtr();
    return _ptr->ai_flags;
}

int GetAddrInfo::getFamily() const
{
    checkPtr();
    return _ptr->ai_family;
}

int GetAddrInfo::getSocktype() const
{
    checkPtr();
    return _ptr->ai_socktype;
}

int GetAddrInfo::getProtocol() const
{
    checkPtr();
    return _ptr->ai_protocol;
}

SockAddress GetAddrInfo::getAddress() const
{
    checkPtr();
    return SockAddress(_ptr->ai_addr, _ptr->ai_addrlen);
}

const std::string GetAddrInfo::getCanonname() const
{
    checkPtr();
    if (!_ptr->ai_canonname) return "";
    return _ptr->ai_canonname;
}

void GetAddrInfo::checkPtr() const
{
    checkDone();
    if (!_ptr)
        THROWINTERFACEEXCEPTION("next() not called yet");
}

void GetAddrInfo::checkAval() const
{
    if (_result)
        THROWINTERFACEEXCEPTION("doit() already called");
}

void GetAddrInfo::checkDone() const
{
    if (!_result)
        THROWINTERFACEEXCEPTION("doit() not called yet");
}

GaiException::GaiException(int err, const std::string & node, const std::string & service)
: Exception()
, _err(err)
, _node(node)
, _service(service)
{
}

GaiException::GaiException(int err, const std::string & node, const std::string & service, const char * file, int line, const t_stack & st)
: Exception(file, line, st)
, _err(err)
, _node(node)
, _service(service)
{
}

std::string GaiException::vtext() const
{
    const char * s = gai_strerror(_err);
    const string errs = s ? quote(s) : string(Format("(%$ n/a)") % _err);
    return Format("GaiException for node=%$ service=%$, error=%$")
            % quote(_node)
            % quote(_service)
            % errs
            ;
}

#if __linux__

EthernetSocket::EthernetSocket(unsigned short protocol)
: _protocol(protocol)
{
    if (_protocol != 3)
    {
        if (_protocol < 0x600)
            THROWINTERFACEEXCEPTION(
                                    Format("protocol = %$ : should be >= 0x0600") % utox(protocol));

        if (_protocol == 0x8870)
            THROWINTERFACEEXCEPTION("protocol 0x8870 reserved for JumboFrames");
    }

    const int fdx = ::socket(AF_PACKET, SOCK_DGRAM, htons(_protocol));
    if (fdx < 0)
        THROWERREXCEPTIONHERES(
                               Format("socket(AF_PACKET, SOCK_DGRAM, htons(%$))") % utox(protocol)
                               );
    setup(FileDescriptor(fdx));
}

void EthernetSocket::bind(const char * iface)
{
    if (::strlen(iface) >= IFNAMSIZ)
        THROWINTERFACEEXCEPTION(
                                Format("interface name too long: %$ : IFNAMSIZ=%$")
                                % quote(iface)
                                % IFNAMSIZ
                                );

    ifreq q;
    ZERO(q);
    ::strncpy(q.ifr_name, iface, IFNAMSIZ);

    ERRCALLS(
             ::ioctl(+fd(), SIOCGIFINDEX, &q),
             Format("ioctl(fd=%$, SIOCGIFINDEX, ?)") % fd()
             );

    sockaddr_ll sa;
    ZERO(sa);
    sa.sll_family = AF_PACKET;
    sa.sll_hatype = ARPHRD_ETHER;
    sa.sll_protocol = htons(_protocol);
    sa.sll_ifindex = q.ifr_ifindex;
    sa.sll_pkttype = PACKET_HOST;
    sa.sll_halen = ETH_ALEN;

    ERRCALLS(
             ::bind(+fd(), reinterpret_cast<const sockaddr *> (&sa), sizeof (sa)),
             Format("bind(fd=%$, ?, %$)") % fd() % sizeof (sa)
             );
}

size_t EthernetSocket::sendto(const ConstSegment & s, const EthernetAddress & d)
{
    const int x = ::sendto(+fd(), s.ptr(), s.size(), 0, d.ptr_sockaddr(), d.length());
    if (x < 0)
        THROWERREXCEPTIONHERES(
                               Format("sendto(fd=%$, %$, %$, 0, <%$>)") % fd() % ptrtoa(s.ptr()) % s.size() % d
                               );
    return x;
}

size_t EthernetSocket::recvfrom(const Segment & s, EthernetAddress * k)
{
    sockaddr_ll q;
    socklen_t ql = sizeof (q);
    const int x = ::recvfrom(
                             +fd(),
                             s.ptr(), s.size(),
                             0,
                             reinterpret_cast<sockaddr*> (&q), &ql
                             );
    if (x < 0)
        THROWERREXCEPTIONHERES(
                               Format("recvfrom(fd=%$, %$, %$, 0, ?)") % fd() % ptrtoa(s.ptr()) % s.size()
                               );
    const size_t qz = sizeof (q) - sizeof (q.sll_addr) + ETH_ALEN;
    if (ql != qz)
        THROWOSEXCEPTION(
                         Format("socklen mismatch: exp=%$, got=%$") % qz % ql
                         );
    if (k) *k = EthernetAddress(q);
    return x;
}

EthernetAddress::EthernetAddress()
{
    ZERO(_address);
    _address.sll_family = AF_PACKET;
    _address.sll_hatype = ARPHRD_ETHER;
    _address.sll_pkttype = PACKET_HOST;
    _address.sll_protocol = htons(ETH_P_ALL);
    _address.sll_halen = ETH_ALEN;
}

EthernetAddress::EthernetAddress(unsigned short proto, const char * eth, const char * dest)
{

    ZERO(_address);
    _address.sll_family = AF_PACKET;
    _address.sll_hatype = ARPHRD_ETHER;
    _address.sll_pkttype = PACKET_OTHERHOST;
    _address.sll_protocol = htons(proto);
    _address.sll_ifindex = netdevice_name2index(eth);
    _address.sll_halen = ETH_ALEN;

    int j;
    unsigned char x = 0;
    for (j = 0; dest[j]; ++j)
    {
        switch (j % 3)
        {
        case 0:
        {
            const int y = sym2val(dest[j]);
            if (y < 0)
                THROWINTERFACEEXCEPTION(
                                        Format("invalid ethernet address: %$ at %$") % dest % j
                                        );
            x = y << 4;
        };
            break;

        case 1:
        {
            const int y = sym2val(dest[j]);
            if (y < 0)
                THROWINTERFACEEXCEPTION(
                                        Format("invalid ethernet address: %$ at %$") % dest % j
                                        );
            x |= y;
            _address.sll_addr[j / 3] = x;
        };
            break;

        case 2: if (dest[j] != ':')
                THROWINTERFACEEXCEPTION(
                                        Format("invalid ethernet address: %$ at %$")
                                        % quote(dest) % j
                                        );
            break;
        }

    }

    if (j != 3 * 6 - 1)
        THROWINTERFACEEXCEPTION(
                                Format("invalid ethernet address length %$ in %$") % j % quote(dest)
                                );

    _address.sll_addr[6] = 0;
    _address.sll_addr[7] = 0;
}

int netdevice_name2index(const char * name)
{
    if (strlen(name) >= IFNAMSIZ)
        THROWINTERFACEEXCEPTION(
                                Format("interface name too long: %$ : IFNAMSIZ=%$")
                                % quote(name)
                                % IFNAMSIZ
                                );

    ifreq q;
    ZERO(q);
    ::strncpy(q.ifr_name, name, IFNAMSIZ);

    EthernetSocket s(ETH_P_ALL);
    ERRCALLS(
             ::ioctl(+s.fd(), SIOCGIFINDEX, &q),
             Format("ioctl(fd=%$, SIOCGIFINDEX, ?)") % s.fd()
             );
    return q.ifr_ifindex;
}

string netdevice_index2name(int index)
{
    ifreq q;
    ZERO(q);
    q.ifr_ifindex = index;

    EthernetSocket s(ETH_P_ALL);
    ERRCALLS(
             ::ioctl(+s.fd(), SIOCGIFNAME, &q),
             Format("ioctl(fd=%$, SIOCGIFNAME, ?)")
             % s.fd()
             );
    return q.ifr_name;
}

string EthernetAddress::_XStringation_char_str() const
{
    return Format("%$:%$:%$:%$:%$:%$@%$(%$)!%$")
            % utox(_address.sll_addr[0], '\0')
            % utox(_address.sll_addr[1], '\0')
            % utox(_address.sll_addr[2], '\0')
            % utox(_address.sll_addr[3], '\0')
            % utox(_address.sll_addr[4], '\0')
            % utox(_address.sll_addr[5], '\0')
            % netdevice_index2name(_address.sll_ifindex)
            % _address.sll_ifindex
            % utox(ntohs(_address.sll_protocol))
            ;
}
#endif

int split_hostport(const string & k, string & host, string & port)
{
    const string s = trim(k);
    if (s.empty()) return AF_UNSPEC;

    size_t col = s.rfind(':');
    const size_t ket = s.rfind(']');
    if (ket != string::npos && col != string::npos && ket > col) col = string::npos;

    host = trim(col != string::npos ? s.substr(0, col) : s);
    port = trim(col != string::npos ? s.substr(col + 1) : string());

    if (host.empty()) return AF_UNSPEC;

    if (host[0] == '[')
    {
        if (host[host.size() - 1] != ']')
            THROWDATAEXCEPTION(Format("bracket mismatch in: %$") % quote(k));
        host = trim(host.substr(1, host.size() - 2));
        return AF_INET6;
    }

    return AF_INET;
}

string SockAddress::s_domain_label(int x)
{
    switch (x)
    {
    case AF_UNSPEC: return "AF_UNSPEC";
    case AF_UNIX: return "AF_UNIX";
    case AF_INET: return "AF_INET";
    case AF_INET6: return "AF_INET6";
    default: return Format("AF_%$") % x;
    }
    return "?";

}

CXU_NS_END


//.
