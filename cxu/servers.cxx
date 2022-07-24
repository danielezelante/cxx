// YAL zeldan

#include "servers.hxx"

#include "mymacros.hxx"


using namespace std;
using namespace cxu;

CXU_NS_BEGIN


TextStreamServer::~TextStreamServer()
{
    ObjLock<t_map> cli(_clients);
    if (!cli->empty())
    {
        LOGGER.info(Format("destroying server with %$ clients connected") % cli->size());
    }

    for (auto & it : *cli)
    {
        _server.deleteClose(it.second->stream());
        delete it.second;
        it.second = 0;
    }

}

WorkerThread::t_retval TextStreamServer::run()
{
    //TRACE("TextStreamServer::run()");
    for (;;)
    {
        Poller pox;
        pox |= _server; // to accept new connections

        pox |= _remux.rdep(); // to receive remux requests
        {
            //TRACE("ObjLock _clients");
            ObjLock<t_map> cli(_clients);
            for (auto const & it : *cli)
            {
                it.second->prepareout();
                SockStream * t = it.second->stream();
                ASSERT(t);
                if (it.second->wantInput()) pox |= t->rf();
                if (it.second->wantOutput()) pox |= t->wf(); // if has to send
            }
        }

        if (_hbto > DeltaTime::O())
        {
            //LOGGER.debug(Format("pox.wait(%$)") %_hbto);
            pox.wait(_hbto); // punto di blocco, timeout per buttafuori
        }
        else
        {
            //LOGGER.debug("pox.wait()");
            pox.wait(); // punto di blocco senza timeout
        }


        if (!pox.is_ok(_server))
        {
            LOGGER.error(Format("server error: %$") % pox[_server]);
            return 0;
        }


        {
            ObjLock<t_map> cli(_clients);
            for (auto it = cli->begin(); it != cli->end(); ++it)
            {
                TextStreamPeer * ptp = it->second;
                SockStream * t = ptp->stream();
                ASSERT(t);

                bool closed = false;
                const bool hangup = pox & (*t) && !pox.is_ok(*t);

                if (pox / t->rf())
                    if (!ptp->doInput()) closed = true;

                if (!closed)
                {
                    if (pox / t->wf())
                    {
                        ptp->doOutput();
                        if (ptp->_kodding && !ptp->_kodded && ptp->_top.empty())
                            ptp->_kodded = true;
                    }

                    if (ptp->_kodded)
                    {
                        LOGGER.debug(Format("shutdown after KOD, fd=%$") % t->fd());
                        t->shutdown();
                    }

                    if (ptp->isInvalid() && !ptp->_kodding)
                    {
                        LOGGER.debug(Format("scheduling KOD_invalid, fd=%$") % t->fd());
                        ptp->_kodding = true;
                        const std::string kod = ptp->onKOD_invalid();
                        if (kod.empty()) ptp->_kodded = true;
                        else ptp->_top.force(kod);
                    }

                    if (ptp->isError() && !ptp->_kodding)
                    {
                        LOGGER.debug(Format("scheduling KOD_error, fd=%$") % t->fd());
                        ptp->_kodding = true;
                        const std::string kod = ptp->onKOD_error();
                        if (kod.empty()) ptp->_kodded = true;
                        else ptp->_top.force(kod);
                    }

                    if (ptp->isTimeout(_hbto) && !ptp->_kodding)
                    {
                        LOGGER.debug(Format("scheduling KOD_timeout, fd=%$") % t->fd());
                        ptp->_kodding = true;
                        const std::string kod = ptp->onKOD_timeout();
                        if (kod.empty()) ptp->_kodded = true;
                        else ptp->_top.force(kod);
                    }
                }
                else
                {
                    LOGGER.debug(Format("closed by client, fd=%$") % t->fd());
                }

                if (closed || hangup || ptp->_kodded)
                {
                    ptp->onClose();
                    delete ptp;
                    _server.deleteClose(t);
                    cli->erase(it);
                    // qui it non e` piu` valido
                    break; //esce dal ciclo e torna il multiplex
                }
            }
        }


        if (pox / _server)
        {
            SockAddress clia;
            SockStream * cs = _server.newAccept(clia);
            LOGGER.debug(Format("accepted client: %$") % clia);
            ObjLock<t_map> clis(_clients);
            TextStreamPeer * peer = vnew_peer(cs, clia);
            if (!clis->insert(make_pair(clia, peer)).second)
            {
                LOGGER.error("duplicated client stream pointer");
                _server.deleteClose(cs);
                delete peer;
            }
            else
                peer->onConnect();

            continue; // torna a multiplexare con il nuovo client

        }


        if (pox / _remux.rdep()) // per ultimo perche` ha priorita di azione piu` bassa
        {
            //LOGGER.debug("received remux");
            char x = 0;
            NativeData nd(x);
            if (!_remux.rdep().readAll(nd))
                LOGGER.error("failed to read from remux pipe");

            continue;
        }

    }

    return 0;
}

void TextStreamServer::remux()
{
    char x = 0;
    ConstNativeData cnd(x);
    //LOGGER.debug("sending remux");
    if (!_remux.wrep().writeAll(cnd))
        LOGGER.error("failed to write to pipe");
    //LOGGER.debug("sent remux");
}



//** TextStreamPeer

TextStreamPeer::~TextStreamPeer()
{
    //if (!_inputstring.empty())
    //	LOGGER.warning(Format("closing with input string: %$") %quote(_inputstring));
    //if (!_outputstring.empty())
    //	LOGGER.warning(Format("closing with output string: %$") %quote(_outputstring));
}

TextStreamPeer::TextStreamPeer(cxu::SockStream * s, TextPackerOutput::t_eol eol, char inval)
: _top(*s, eol) // massima dimensione per IP
, _tip(*s, inval)
, _ts(s)
, _lastknown(cxu::Time::now())
, _kodding(false)
, _kodded(false)
, _error(false)
, _invalid(false)
{
}

bool TextStreamPeer::doInput()
{
    try
    {
        if (!_tip.read()) return false;
        _lastknown = Time::now();
        string si;
        while (_tip.pull(si))
        {
            if (!_tip.isvalid(si))
            {
                _invalid = true;
                break;
            }
            if (!onInput(si))
            {
                _error = true;
                break;
            }
        }
    }
    catch (const Exception & ex)
    { // per trappare errori del tipo "Connection reset by peer"
        ex.log(Logger::e_warning);
        return false;
    }
    return true;
}

bool TextStreamPeer::isTimeout(const DeltaTime & dt) const
{
    return (dt > DeltaTime::O()) && _lastknown + dt < Time::now();
}

bool TextStreamPeer::doOutput()
{
    while (!_top.empty())
        if (!_top.write()) return false;
    return true;
}

void TextStreamPeer::prepareout()
{
    while (_top.avail())
    {
        const string s = onOutput();
        if (s.empty()) break;
        _top.push(s);
    }
}



//** TextSingleProcessor

TextSingleProcessor::~TextSingleProcessor()
{
}

TextSingleProcessor::TextSingleProcessor(
                                         TextProtocol & p,
                                         ZableFile & f,
                                         const DeltaTime & cyc,
                                         const std::set<Remuxer*> & remuxers
                                         )
: _protocol(p)
, _file(f)
, _cycle(cyc)
, _tip(f, 0xFF)
, _top(f, _protocol.getEOL())
, _remuxers(remuxers)
{
}

int TextSingleProcessor::go()
{

    for (;;)
    {
        if (!_protocol.onMux() && _top.empty()) break;
        Poller mpx;
        for (auto const & it : _remuxers)
            if (it) mpx |= *it;
        if (_protocol.wantOutput() || !_top.empty()) mpx |= file().wf();
        if (_protocol.wantInput()) mpx |= file().rf();
        //LOGGER.debug(Format("Poller::wait(%$)") %_cycle);
        if (!mpx.wait(_cycle))
        {
            _protocol.onPTO();
            continue;
        }

        {
            bool remuxed = false;
            for (auto const & it : _remuxers)
            {
                if (it && (mpx / *it))
                {
                    LOGGER.debug("received remux");
                    it->consume();
                    remuxed = true;
                }
            }
            if (remuxed)
            {
                LOGGER.debug("doing remux");
                continue;
            }
        }

        if (!(mpx & file()))
            THROWTEXTEXCEPTION(Format("internal error, %$ not found") % file().fd());

        if (!mpx.is_ok(file()))
            THROWDATAEXCEPTION(
                               Format("poll error %$ on %$")
                               % utox(mpx[file()])
                               % file().fd()
                               )
            ;

        if (mpx / file().rf())
        {
            if (!_tip.read())
            {
                _protocol.onClose();
                break;
            }
            for (;;)
            {
                if (!_protocol.wantInput()) break;
                string s;
                if (!_tip.pull(s)) break;
                _protocol.onInput(s);
            }
        }

        if (mpx / file().wf())
        {
            while (_top.avail() && _protocol.wantOutput())
                _top.push(_protocol.onOutput());

            if (!_top.write())
            {
                _protocol.onClose();
                break;
            }
        }

    }


    return 0;
}


CXU_NS_END

//.
