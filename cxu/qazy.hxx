// YAL zeldan


#ifndef CXU_QAZY_HXX
#define CXU_QAZY_HXX


#include "defs.hxx"
#include "types.hxx"
#include "object.hxx"
#include "socket.hxx"
#include "thread.hxx"
#include "text.hxx"

#include "servers.hxx"

CXU_NS_BEGIN


class QAZY1Client : public TextProtocol
{
public:
    virtual ~QAZY1Client();
    explicit QAZY1Client(const DeltaTime & timeout);


protected:

    virtual std::string anyQ() = 0;
    virtual std::string anyY() = 0;

    virtual bool wantQ() const = 0;
    virtual bool wantY() const = 0;

    virtual void onA(const std::string & s) = 0;
    virtual void onZ(const std::string & s) = 0;

    virtual TextPackerOutput::t_eol getEOL() const override;

    const SockAddress _server;
    virtual void onClose() override = 0;
    virtual bool onMuxQAZY() = 0;


private:
    virtual std::string onOutput() override;
    virtual void onInput(const std::string &) override;

    virtual void onPTO() override;
    virtual bool onMux() override;

    virtual bool wantInput() const override;
    virtual bool wantOutput() const override;


    const DeltaTime _timeout;
    bool _waitingBanner;
    bool _pendingQuestion;
    bool _zynced;
    bool _sentKOD;
    std::string _banner;
    std::string _error;
    std::string _koderror;

    Time _beginTime;
    Time _sentQtime;
};

class QAZYnClient : public WorkerThread
{
public:
    virtual ~QAZYnClient();
    QAZYnClient();


protected:

    virtual std::string anyQ(const std::string & tag) = 0;
    virtual std::string anyY(const std::string & tag) = 0;

    virtual void onA(const std::string & tag, const std::string & s) = 0;
    virtual void onZ(const std::string & tag, const std::string & s) = 0;

    virtual t_retval run() override;

};


CXU_NS_END

#endif

//.
