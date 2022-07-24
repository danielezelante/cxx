// YAL zeldan

#ifndef CXU_SOCKET_HXX_
#define CXU_SOCKET_HXX_

#include <sys/types.h>
#include <sys/socket.h>
//#include <netinet/in_systm.h>
#include <netinet/in.h>
//#include <netinet/ip.h>
#include <netinet/tcp.h>

#if __linux__
#include <netpacket/packet.h>
#include <net/ethernet.h>
#endif

#include <netdb.h>
#include <assert.h>

#include <set>
#include <vector>

#include "object.hxx"
#include "memory.hxx"
#include "sync.hxx"
#include "exception.hxx"
#include "types.hxx"
#include "file.hxx"

CXU_NS_BEGIN


std::string getHostname(); // TODO sta qua ma potrebbe stare la

int netdevice_name2index(const char *);
std::string netdevice_name2index(int);

// return AF_INET / AF_INET6 / AF_UNSPEC
int split_hostport(const std::string &, std::string & host, std::string & port);

class Inet4Address
: public Stringable
, public Set<Inet4Address, true>
, public TotalOrder<Inet4Address>
//, public Serialization // TODO
{
public:

    Inet4Address(const in_addr & x) : _address(x)
    {
    }


    //! null address i.e 0.0.0.0
    Inet4Address();
    explicit Inet4Address(const char *); // form dotted decimal
    explicit Inet4Address(const std::string &); // form dotted decimal

    static Inet4Address any();
    static Inet4Address loopback();
    static Inet4Address broadcast();

    //! returns numeric dotted form
    const std::string octets() const;


    std::string str() const override;

    static int sdo_TotalOrder_compare(const Inet4Address &, const Inet4Address &);

    const in_addr * address() const;


    //! Serialize to Binary Output Storage
    //void serialize(BOStorage &) const;

    //! Serialize from Binary Input Storage
    //void serialize(BIStorage &);

    //! Serialize to Text Output Storage
    //void serialize(TOStorage &) const;

    //! Serialize from Text Input Storage
    //void serialize(TIStorage &);

    //! Get pretty class name
    /*! Should be used to implement text serialization */
    //const char * classname() const { return "Inet4Address"; }


protected:

    in_addr _address;


};

class Inet6Address
: public Stringable
, public Set<Inet6Address, true>
, public TotalOrder<Inet6Address>
//, public Serialization // TODO
{
public:

    Inet6Address(const in6_addr & x) : _address(x)
    {
    }


    //! null address
    Inet6Address();

    explicit Inet6Address(const char * x);
    explicit Inet6Address(const std::string &);

    static Inet6Address any();
    static Inet6Address loopback();

    //Inet4Address(const ConstSegment &);

    //! returns numeric coloned form
    const std::string octets() const;

    std::string str() const override;

    static int sdo_TotalOrder_compare(const Inet6Address &, const Inet6Address &);


    const in6_addr * address() const;


    //! Serialize to Binary Output Storage
    //void serialize(BOStorage &) const;

    //! Serialize from Binary Input Storage
    //void serialize(BIStorage &);


protected:

    in6_addr _address;

};

class SockAddress
: public Set<SockAddress, true>
, public TotalOrder<SockAddress>
, public Stringable
{
public:
    SockAddress();
    SockAddress(const sockaddr *, size_t len);

    int domain() const
    {
        return ptr_sockaddr()->sa_family;
    }
    const std::string domain_label() const;
    static std::string s_domain_label(int);

    size_t length() const;

    size_t storage_length() const
    {
        return sizeof (_sockaddr);
    }
    std::string str() const; // dispatch polimorfico
    void clear();
    bool is() const;

    const sockaddr * ptr_sockaddr() const
    {
        return reinterpret_cast<const sockaddr *> (&_sockaddr);
    }

    sockaddr * ptr_sockaddr()
    {
        return reinterpret_cast<sockaddr *> (&_sockaddr);
    }


    SockAddress withPort(unsigned short) const; //dispatch polimorfico
    static bool equalsIgnorePort(const SockAddress &, const SockAddress &); //dispatch polimorfico

    // algebra interface
    static int sdo_TotalOrder_compare(const SockAddress &, const SockAddress &);

protected:
    sockaddr_storage _sockaddr;

};

class LocalSockAddress : public SockAddress
{
public:
    LocalSockAddress();
    LocalSockAddress(const SockAddress &);
    LocalSockAddress(const char * name);
#ifdef __linux__
    LocalSockAddress(const char * name, bool abstract);
#endif
    std::string name() const;

    // algebra interface
    static int sdo_TotalOrder_compare_impl(const LocalSockAddress &, const LocalSockAddress &);

};

class Inet4SockAddress : public SockAddress
{
public:
    Inet4SockAddress();
    Inet4SockAddress(const SockAddress &);
    Inet4SockAddress(const Inet4Address & host, unsigned short port);

    //! return in numeric format
    const std::string octets() const;

    //! resolve address and service
    const std::string resolv() const;

    unsigned short port() const; //! returns port (in host order)
    Inet4Address address() const;

    sockaddr_in * sax()
    {
        return reinterpret_cast<sockaddr_in*> (&_sockaddr);
    }

    const sockaddr_in * sax() const
    {
        return reinterpret_cast<const sockaddr_in*> (&_sockaddr);
    }

    // algebra interface
    static int sdo_TotalOrder_compare_impl(const Inet4SockAddress &, const Inet4SockAddress &);

};

class Inet6SockAddress : public SockAddress
{
public:
    Inet6SockAddress();
    Inet6SockAddress(const SockAddress &);
    Inet6SockAddress(const Inet6Address & host, unsigned short port, int scopeid = 0);

    //! return in numeric format
    const std::string octets() const;

    //! resolve address and service
    const std::string resolv() const;

    unsigned short port() const; //! returns port (in host order)
    Inet6Address address() const;

    sockaddr_in6 * sax()
    {
        return reinterpret_cast<sockaddr_in6*> (&_sockaddr);
    }

    const sockaddr_in6 * sax() const
    {
        return reinterpret_cast<const sockaddr_in6*> (&_sockaddr);
    }

    // algebra interface
    static int sdo_TotalOrder_compare_impl(const Inet6SockAddress &, const Inet6SockAddress &);
};


#if __linux__

class EthernetAddress
: public XStringation<EthernetAddress, char>
, public Set<EthernetAddress, true>
, public TotalOrder<EthernetAddress>
{
public:

    EthernetAddress(const sockaddr_ll & x) : _address(x)
    {
    }


    EthernetAddress();
    EthernetAddress(unsigned short proto, const char * eth, const char * dest); // to send

    static EthernetAddress any();
    static EthernetAddress broadcast();

    //! returns numeric dotted form
    const std::string octets() const;
    const std::string iface() const;


    std::string _XStringation_char_str() const;

    static int sdo_TotalOrder_compare(const EthernetAddress &, const EthernetAddress &);

    const sockaddr_ll * address() const
    {
        return &_address;
    }

    const sockaddr * ptr_sockaddr() const
    {
        return reinterpret_cast<const sockaddr *> (&_address);
    }

    unsigned short protocol() const
    {
        return ntohs(_address.sll_protocol);
    }

    size_t length() const
    {
        return sizeof (_address);
    }

    size_t halen() const
    {
        return _address.sll_halen;
    }

protected:

    sockaddr_ll _address;

};

#endif

#define CXU_THROWGAIEXCEPTION(E, N, S) CXU_THROWEXCEPTION(CXU_NS::GaiException, E, N, S)

class GaiException : public Exception
{
public:
    GaiException(int err, const std::string & node, const std::string & service);
    GaiException(int err, const std::string & node, const std::string & service, const char * file, int line, const t_stack & st);


protected:
    const int _err;
    const std::string _node;
    const std::string _service;

    virtual std::string vtext() const override;


};

class GetAddrInfo : Entity
{
public:

    virtual ~GetAddrInfo();
    GetAddrInfo(const std::string & node, const std::string & service);

    void hintFamily(int);
    void hintSocktype(int);
    void hintProtocol(int);
    void hintFlags(int);

    void doit();
    bool next();

    int getFlags() const;
    int getFamily() const;
    int getSocktype() const;
    int getProtocol() const;
    SockAddress getAddress() const;
    const std::string getCanonname() const;


protected:
    const std::string _node;
    const std::string _service;
    struct addrinfo _hints;
    struct addrinfo * _result;
    struct addrinfo * _ptr;

    void checkDone() const;
    void checkAval() const;
    void checkPtr() const;
};

class Socket : public RealFile
{
public:
    virtual ~Socket();

    void setsockopt_sock_int(int optname, int value);
    void setsockopt_keepalive(bool);


protected:
    void setsockopt_int(int level, int optname, int value);

};

#if 0
// implement out of hier

class TcpSocket : public Socket
{
public:
    void setsockopt_tcp_int(int optname, int value);
    void setsockopt_nodelay(bool);
    void setsockopt_keepidle(const DeltaTime &);
    void setsockopt_keepintvl(const DeltaTime &);
    void setsockopt_keepcnt(unsigned);
};

class UdpSocket : public Socket
{
};
#endif


//! generic TCP stream used both in client and server side

class SockStream
: public Socket
, public ZableFile
{
    friend class SockStreamServer;

public:

    virtual ~SockStream();

    void shutdown();


protected:

    explicit SockStream(const SockAddress &); // per client

private:

    explicit SockStream(const FileDescriptor & fd); // perserver

    bool _slave;


};

class SockStreamClient : public SockStream
{
public:
    virtual ~SockStreamClient();
    explicit SockStreamClient(const SockAddress &);
    void connect();

    bool connectAsync(); // return true if connected now
    void connectComplete(); // should be called after POLLIN 
    int connectAborted(); // return error code, should be called afer POLLERR

    bool isConnected() const
    {
        return _connected;
    }

    bool isConnecting() const
    {
        return _connecting;
    }

    const SockAddress & getServer() const
    {
        return _address;
    }

    virtual bool close() override;
    bool close_nothrow();

private:
    const SockAddress & _address;
    bool _connecting;
    bool _connected;

    void fcntl_block();
    void fcntl_nonblock();

};




//! SockStreamServer, used to bind and listen connections

class SockStreamServer : public Socket, virtual public Pollable
{
public:
#if __unix__
    SockStreamServer(const SockAddress &, bool reuse = true, int backlog = SOMAXCONN);
#else
    SockStreamServer(const SockAddress &, bool reuse = true, int backlog = INT_MAX);
#endif
    virtual ~SockStreamServer();

    //! accept a connection
    /*! Accept a connection, and returns a new SockStream when connected.
     *! The client address is stored in \pa.
     *! At the end of use, you should call \mdeleteClose on returned
     *! SockStream to prevent leakage
     */
    SockStream* newAccept(SockAddress &);


    //! close and delete a SockStream returned by newAccept
    void deleteClose(SockStream *);

    // Pollable
    virtual short events() const override;
    virtual bool match(short) const override;


private:

    void bind_listen(int backlog);

    const SockAddress & _address;
    std::set<SockStream*> _slaves;
};



//! SockDgram class used both by server and client side

class SockDgram
: public Socket
, virtual public ZableFile
{
public:

    virtual ~SockDgram();

    size_t recvfrom(const Segment &, SockAddress *);
    //size_t recvfrom(const Segment &, SockAddress *, const DeltaTime & timeout);



protected:

    explicit SockDgram(int domain);
    size_t _rcvsize;

};


//! UDP server, i.e. UDP socket bound to a w.k.p.

class SockDgramServer : public SockDgram
{
public:
    SockDgramServer(const SockAddress &, bool reuse = false);

    void sendto(const ConstSegment &, const SockAddress &);

};

class SockDgramClient : public SockDgram
{
public:
    explicit SockDgramClient(const SockAddress &);

    void send(const ConstSegment &);
    size_t recv(const Segment &);
    size_t recvnb(const Segment &, bool & wb); // wb==true if would block

};

#if __linux__

class EthernetSocket
: public Socket
, virtual public ZableFile
{
public:
    //EthernetSocket();
    explicit EthernetSocket(unsigned short protocol);
    //EthernetSocket(const EthernetAddress & local);
    void bind(const char * iface);
    size_t sendto(const ConstSegment &, const EthernetAddress &);
    size_t recvfrom(const Segment &, EthernetAddress * sender);

protected:
    unsigned short _protocol;
};

#endif

CXU_NS_END


#endif

//.
