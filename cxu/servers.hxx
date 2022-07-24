// YAL zeldan

#ifndef CXU_SERVERS_HXX
#define CXU_SERVERS_HXX


#include "defs.hxx"
#include "types.hxx"
#include "object.hxx"
#include "socket.hxx"
#include "thread.hxx"
#include "text.hxx"

CXU_NS_BEGIN

// lato server
class TextStreamPeer : Entity
{
    friend class TextStreamServer;

public:
    //enum class t_eol {e_CR, e_LF, e_CRLF};


protected:

    virtual ~TextStreamPeer();

    TextStreamPeer(SockStream *, cxu::TextPackerOutput::t_eol eol, char inval);

    SockStream * stream() const
    {
        return _ts;
    }

    // queste per il Poller

    bool wantOutput()
    {
        return !_top.empty();
    }

    bool wantInput()
    {
        return !_kodding;
    }
    bool doInput(); // torna false se closed
    bool doOutput(); // torna false se closed

    // fa in modo che wantOutput torni dati coerenti,
    // eventualmente chiamando onOutput
    void prepareout();

    bool isTimeout(const DeltaTime &) const;

    bool isError() const
    {
        return _error;
    }

    bool isInvalid() const
    {
        return _invalid;
    }

    virtual bool onInput(const std::string &) = 0; // torna falso KOD_error
    virtual std::string onOutput() = 0; // torna vuoto se non ha output da fare
    virtual void onConnect() = 0;
    virtual void onClose() = 0;

    virtual const std::string onKOD_timeout() = 0;
    virtual const std::string onKOD_error() = 0;
    virtual const std::string onKOD_invalid() = 0;



private:
    TextPackerOutput _top;
    TextPackerInput _tip;
    SockStream * _ts;
    Time _lastknown;
    bool _kodding;
    bool _kodded;
    bool _error;
    bool _invalid;

};

// lato server

class TextStreamServer : public WorkerThread
{
public:

    typedef std::map<SockAddress, TextStreamPeer*> t_map;

    virtual ~TextStreamServer();

    DeltaTime get_hbto() const
    {
        return _hbto;
    }

    ObjSync<t_map> & getClients()
    {
        return _clients;
    }

    const ObjSync<t_map> & getClients() const
    {
        return _clients;
    }

    void remux();

protected:

    TextStreamServer(
                     const char * thn,
                     const SockAddress & b,
                     const DeltaTime & hbto = DeltaTime::O(),
                     bool reuse = true
                     )
    : WorkerThread(thn)
    , _server(b, reuse)
    , _hbto(hbto)
    {
    }

    virtual TextStreamPeer * vnew_peer(SockStream *, const SockAddress &) = 0;

    virtual WorkerThread::t_retval run() override;

    SockStreamServer & server()
    {
        return _server;
    }

    const SockStreamServer & server() const
    {
        return _server;
    }

private:

    SockStreamServer _server;
    VarObjSync<t_map> _clients;
    DeltaTime _hbto;
    Pipe _remux;

};


// lato client

class TextProtocol : public Entity
{
public:
    virtual ~TextProtocol() = default;

    virtual bool onMux() = 0;

    virtual std::string onOutput() = 0;
    virtual void onInput(const std::string &) = 0;

    virtual bool wantInput() const = 0;
    virtual bool wantOutput() const = 0;

    virtual void onClose() = 0;
    virtual void onPTO() = 0;

    virtual TextPackerOutput::t_eol getEOL() const = 0;

};

class Remuxer : public Pollable
{
public:

    virtual void consume() = 0;
    virtual void produce() = 0;

};

// lato client

class TextSingleProcessor : public Entity
{
public:
    virtual ~TextSingleProcessor();
    TextSingleProcessor(
                        TextProtocol & proto,
                        ZableFile & data,
                        const DeltaTime & cycle,
                        const std::set<Remuxer*> & remuxers
                        );


    int go();


protected:

    ZableFile & file()
    {
        return _file;
    }

    const ZableFile & file() const
    {
        return _file;
    }


private:

    TextProtocol & _protocol;
    ZableFile & _file;
    const DeltaTime _cycle;
    TextPackerInput _tip;
    TextPackerOutput _top;
    const std::set<Remuxer*> & _remuxers;

};




CXU_NS_END

#endif

//.
