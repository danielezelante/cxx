// YAL zeldan

#include "qazy.hxx"

#include "mymacros.hxx"


using namespace std;
using namespace cxu;

CXU_NS_BEGIN

QAZY1Client::~QAZY1Client()
{
    if (_waitingBanner) LOGGER.warning("waitingBanner");
    if (_pendingQuestion) LOGGER.warning("pendingQuestion");
    if (_zynced) LOGGER.warning("zynced");
}

QAZY1Client::QAZY1Client(const DeltaTime & to)
: _timeout(to)
, _waitingBanner(true)
, _pendingQuestion(false)
, _zynced(false)
, _sentKOD(false)
{
}

bool QAZY1Client::wantOutput() const
{
    if (!_koderror.empty()) return true;
    if (!_error.empty()) return true;

    if (_waitingBanner) return false;
    if (!_zynced && _pendingQuestion) return false;

    return wantQ() || wantY();
}

bool QAZY1Client::wantInput() const
{
    return true; // to receive '?'

#if 0
    if (_waitingBanner) return true;
    if (_pendingQuestion) return true;
    if (!_zynced) return true;

    return false;
#endif

}

void QAZY1Client::onInput(const std::string & ss)
{
    const std::string s = rtrim(ss);
    if (s.empty()) return;
    if (s[0] == '#') return;

    if (_waitingBanner)
    {
        const string ib = Format("invalid banner: %$") % quote(s);
        if (s.size() < 2)
        {
            _koderror = ib;
            return;
        }

        if (s[0] != '!' || s[1] != ' ')
        {
            _koderror = ib;
            return;
        }

        _banner = s.substr(2);
        LOGGER.debug(Format("banner: %$") % quote(_banner));
        _waitingBanner = false;
        return;
    }

    const string im = Format("invalid message type: %$") % quote(s);

    if (s.size() >= 2 && s[1] != ' ')
    {
        _koderror = im;
        return;
    }

    const string msg = (s.size() >= 2 ? s.substr(2) : string());

    switch (s[0])
    {
    case 'A':
    {
        if (_pendingQuestion)
        {
            onA(msg);
            _pendingQuestion = false;
        }
        else
        {
            _error = Format("unexpected Answer: %$") % quote(s);
        }
    };
        break;

    case 'Z':
    {
        if (!_zynced)
        {
            _zynced = true;
            onZ(msg);
        }
        else
        {
            _error = Format("ignoring double Zync: %$") % quote(s);
        }
    };
        break;

    case '?':
    {
        LOGGER.error(Format("notified error: %$") % quote(msg));
        _pendingQuestion = false;
        _zynced = false;
    };
        break;

    default:
    {
        _koderror = im;
    }
    }
}

std::string QAZY1Client::onOutput()
{
    if (_zynced)
    {
        const string y = anyY();
        if (!y.empty())
        {
            _zynced = false;
            return Format("Y %$") % y;
        }
    }

    if (!_pendingQuestion)
    {
        const string q = anyQ();
        if (!q.empty())
        {
            LOGGER.debug(Format("anyQ => %$") % quote(q));
            _pendingQuestion = true;
            _sentQtime = Time::now();
            return Format("Q %$") % q;
        }
    }

    if (!_error.empty())
    {
        const string x = Format("? %$") % _error;
        _error.clear();
        return x;
    }

    if (!_koderror.empty())
    {
        const string x = Format("? KOD %$") % _koderror;
        _koderror.clear();
        _sentKOD = true;
        return x;
    }

    return string();

}

TextPackerOutput::t_eol QAZY1Client::getEOL() const
{
    return TextPackerOutput::t_eol::e_crlf;
}

void QAZY1Client::onPTO()
{
    // do nothing, timeouts are check in onMux
}

bool QAZY1Client::onMux()
{
    if (_sentKOD) return false;

    if (_waitingBanner)
    {
        if (!_beginTime.is())
            _beginTime = Time::now();

        if (_beginTime + _timeout < Time::now())
            _koderror = "banner timeout";
    }

    if (_pendingQuestion)
    {
        if (_sentQtime + _timeout < Time::now())
        {
            _pendingQuestion = false;
            _error = "Q timeout";
        }
    }

    return onMuxQAZY();
}

CXU_NS_END

//.
