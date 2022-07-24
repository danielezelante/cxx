// YAL zeldan

#include <stdio.h>

#include "text.hxx"
#include "mystd.hxx"
#include "mymacros.hxx"

using namespace std;

CXU_NS_BEGIN

static bool s_valid(char k)
{
    return inco(static_cast<int> (k), 32, 127) || k == '\t';
}

void TextOutput::dump(const ConstSegment & s, const std::string & prefix)
{
    const int dw = 68 + sizeof (void*)*2;

    if (!s.ptr())
    {
        write(":--dump--: null segment");
        return;
    }


    const std::string s1 = Format(":--dump--: ptr = %$ : size = %$ ")
            % ptrtoa(s.ptr()) % utox(s.size());

    const std::string s2 = ljus(s1, dw, '-');
    write(Format("%$%$") % prefix % s2);

    for (size_t j = 0; j < s.size(); j += 16)
    {
        writeln();
        write(Format("%$%$:") % prefix % utox(j));
        for (size_t k = 0; k < 16; ++k)
        {
            std::string bb = (j + k < s.size()) ? utox(s.ptr()[j + k]) : "$  ";
            bb[0] = ' ';
            write(bb);
        }

        write("  "); // write 2 blank chars
        for (size_t k = 0; k < 16; ++k)
        {
            if (j + k < s.size())
            {
                const char c = s.ptr()[j + k];
                if (inco(static_cast<int> (c), 32, 127))
                    write(c);
                else
                    write('.');
            }
            else
                write(' ');
        }
    }

    writeln();
    const std::string s3("--end---");
    const std::string s4 = ljus(s3, dw, '-');
    writeln(Format("%$%$") % prefix % s4);

}

void TextOutputFILE::write(char k)
{
    putc(k, _f);
}

void TextOutputFILE::write(const char * s)
{
    fputs(s, _f);
}

void TextOutputFILE::writeln()
{
    fputs("\n", _f);
}

void TextOutputFILE::writeln(const char * s)
{
    fputs(s, _f);
    writeln();
}

void TextOutputFILE::flush()
{
    fflush(_f);
}


//** TextInputFILE

bool TextInputFILE::read(std::string & s)
{
    s.clear();
    for (;;)
    {
        const int k = ::fgetc(_f);
        if (k == EOF) return !s.empty();
        if (k == '\r') continue; // TODO gestire X\rY
        if (k == '\n') break;
        s += validate(k);
    }
    return true;



}

std::string TextInputFILE::read()
{
    string s;
    read(s);
    return s;
}



#if 0
//** TextIO

char TextIO::validate(char k)
{
    return s_valid(k) ? k : _q;
}
#endif



//** TextOutputFile

TextOutputFile::TextOutputFile(WritableFile & s)
: _s(s)
{
}

void TextOutputFile::write(char x)
{
    const ConstNativeData cnd(x);
    _s.writeAll(ConstSegment(cnd));
}

void TextOutputFile::write(const char * str)
{
    const ConstStrData cnd(str);
    _s.writeAll(cnd);
}

void TextOutputFile::writeln()
{
    const char * str = "\n";
    const ConstStrData cnd(str);
    _s.writeAll(cnd);
}

void TextOutputFile::writeln(const char * str)
{
    write(str);
    writeln();
}

//** TextInputFile

TextInputFile::TextInputFile(ReadableFile & s)
: _s(s)
{
}

bool TextInputFile::read(std::string & str)
{
    str.clear();
    for (;;)
    {
        char k;
        NativeData nd(k);
        if (!_s.read(Segment(nd))) return !str.empty();
        if (k == '\r') continue; // per ora ignora i \r
        if (k == '\n') break;
        str += validate(k);
    }
    return true;
}

std::string TextInputFile::read()
{
    std::string x;
    read(x);
    return x;
}







//** TextInput

char TextInput::validate(char k)
{
    return s_valid(k) ? k : _q;
}


//** TextInputConstSegment

bool TextInputConstSegment::read(std::string & s)
{
    s.clear();
    for (;;)
    {
        const int k = readChar();
        if (k == EOF) return !s.empty();
        if (k == '\n') break;
        if (k == '\r') continue;
        s += validate(k);
    }
    return true;
}

std::string TextInputConstSegment::read()
{
    std::string x;
    read(x);
    return x;
}

int TextInputConstSegment::readChar()
{
    if (_ptr >= _cseg.size()) return EOF;
    return _cseg.ptr()[_ptr++];
}



//** TextOutputBuffer

void TextOutputBuffer::write(char k)
{
    if (!s_valid(k)) THROWINTERFACEEXCEPTION(Format("Invalid text char: %$") % quote(std::string(1, k)));
    const size_t z = _buffer.size();
    _buffer.resize(z + 1);
    _buffer.ptr()[z] = k;
}

void TextOutputBuffer::write(const char * s)
{
    while (*s) write(*s++);
}

void TextOutputBuffer::writeln()
{
    const size_t z = _buffer.size();
    _buffer.resize(z + 1);
    _buffer.ptr()[z] = '\n';
}

void TextOutputBuffer::writeln(const char * str)
{
    write(str);
    writeln();
}


//** Properties

bool Properties::has(const std::string & key) const
{
    return _map.find(key) != _map.end();
}

void Properties::clear()
{
    _map.clear();
}

const string & Properties::get(const std::string & key) const
{
    const auto it = _map.find(key);
    if (it == _map.end())
        THROWDATAEXCEPTION(Format("property %$ not found") % quote(key));

    return it->second;
}

void Properties::set(const std::string & key, const string & val)
{
    for (auto it : key)
        if (it <= ' ' || it == 127 || it == '#')
            THROWDATAEXCEPTION(Format("invalid property key: %$") % quote(key));

    const auto ka = _map.insert(make_pair(key, val));
    if (!ka.second)
        ka.first->second = val;

}

bool Properties::mayset(const std::string & key, const string & val)
{
    for (auto it : key)
        if (it <= ' ' || it == 127 || it == '#')
            THROWDATAEXCEPTION(Format("invalid property key: %$") % quote(key));

    const auto ka = _map.insert(make_pair(key, val));
    return ka.second;
}

const string & Properties::operator()(const std::string & key) const
{
    return get(key);
}

string & Properties::operator[](const std::string & key)
{
    for (auto it : key)
        if (it <= ' ' || it == 127 || it == '#')
            THROWDATAEXCEPTION(Format("invalid property key: %$") % quote(key));

    return _map[key];
}

void Properties::del(const std::string & p)
{
    if (!trydel(p))
        THROWDATAEXCEPTION(Format("property %$ not found") % quote(p));
}

bool Properties::trydel(const std::string & p)
{
    const auto it = _map.find(p);
    if (it == _map.end()) return false;
    _map.erase(it);
    return true;
}

bool Properties::any() const
{
    return !_map.empty();
}

const string & Properties::getString(const std::string & key, const std::string & def) const
{
    const auto it = _map.find(key);
    return it == _map.end() ? def : it->second;
}

int Properties::getInt(const std::string & key, int def) const
{
    const auto it = _map.find(key);
    if (it == _map.end())
    {
        LOGGER.info(Format("returning default: %$ => %$") % key % def);
        return def;
    }
    int x = def;
    try
    {
        x = stoi(it->second);
    }
    catch (const Exception & ex)
    {
        ex.log(Logger::e_warning);
    }
    catch (const std::exception & ex)
    {
        LOGGER.warning(ex.what());
    }
    return x;
}

unsigned Properties::getUnsigned(const std::string & key, unsigned def) const
{
    const auto it = _map.find(key);
    if (it == _map.end())
    {
        LOGGER.info(Format("returning default: %$ => %$") % key % def);
        return def;
    }
    unsigned x = def;
    try
    {
        x = stou(it->second);
    }
    catch (const Exception & ex)
    {
        ex.log(Logger::e_warning);
    }
    catch (const std::exception & ex)
    {
        LOGGER.warning(ex.what());
    }
    return x;
}

void loadProperties_bl(Properties & p, TextInput & ti)
{
    std::string a;
    while (ti.read(a))
    {
        const vector<string> v = dqparse(a, '#');
        const size_t z = v.size();
        if (z == 0) continue;
        if (z == 1) p.set(v[0], string());
        if (z >= 2) p.set(v[0], tryunquote(v[1]));

        if (z >= 3)
            LOGGER.warning(Format("extra fields on property line: %$") % quote(a));
    }
}

void saveProperties_bl(const Properties & p, TextOutput & to)
{
    for (auto const & it : +p)
        to.writeln(
                   Format("%$ %$")
                   % it.first
                   % quote(it.second)
                   );
}

void loadProperties_eq(Properties & p, TextInput & ti)
{
    std::string a;
    while (ti.read(a))
    {
        const vector<string> v = parse(a, true, "=", true, true, "#");
        const size_t z = v.size();
        if (z == 0) continue;
        if (z >= 3)
        {
            if (v[1] != "=")
            {
                LOGGER.error(Format("missing '=' symbol in eqproperty line: %$") % a);
                continue;
            }

            if (z > 3)
                LOGGER.warning(Format("extra fields on eqproperty line: %$") % quote(a));

            p.set(v[0], tryunquote(v[2]));

        }
        else
            THROWDATAEXCEPTION(Format("invalid eqproperty line: %$") % quote(a));
    }
}

void saveProperties_eq(const Properties & p, TextOutput & to)
{
    for (auto const & it : +p)
    {
        to.writeln(
                   Format("%$=%$")
                   % it.first
                   % quoteifneeded(it.second)
                   );
    }
}




//** TextPackerOutput

TextPackerOutput::~TextPackerOutput()
{
}

TextPackerOutput::TextPackerOutput(WritableFile & f, t_eol e)
: _file(f)
, _eol(s_eol(e))
, _buffer(_file.write_bufsize())
{
}

void TextPackerOutput::push(const std::string & s)
{
    if (!avail()) THROWINTERFACEEXCEPTION("not available");
    _outstr = s + _eol;
    str2buf();
}

void TextPackerOutput::push(const char * s)
{
    push(string(s));
}

void TextPackerOutput::force(const std::string & s)
{
    if (!avail()) LOGGER.info(Format("forcing: %$") % quote(s));
    _outstr = _eol + s + _eol;
    str2buf();
}

void TextPackerOutput::force(const char * s)
{
    force(string(s));
}

const char * TextPackerOutput::s_eol(t_eol x)
{
    switch (x)
    {
    case t_eol::e_cr: return "\r";
    case t_eol::e_lf: return "\n";
    case t_eol::e_crlf: return "\r\n";
    }

    return "";
}

bool TextPackerOutput::write()
{
    const bool x = _buffer.write(_file);
    str2buf();
    return x;
}

void TextPackerOutput::str2buf()
{
    auto it = _outstr.begin();
    for (; it != _outstr.end() && !_buffer.full(); ++it)
        _buffer.put(*it);
    _outstr.erase(_outstr.begin(), it);
}

//** TextPackerInput

TextPackerInput::~TextPackerInput()
{
    if (!_build.empty())
        LOGGER.warning(Format("destroying with data: %$") % quote(_build));
    if (!_buffer.empty())
        LOGGER.warning(Format("destroying with buffer size: %$") % _buffer.fill());
}

void TextPackerInput::clear()
{
    _build.clear();
    _buffer.clear();
}

TextPackerInput::TextPackerInput(ReadableFile & f, char inval)
: _file(f)
, _inval(inval)
, _buffer(_file.read_bufsize())
{
}

bool TextPackerInput::read()
{
    return _buffer.read(_file);
}

bool TextPackerInput::pull(string & s)
{
    while (!_buffer.empty())
    {
        const char k = _buffer.get();
        switch (k)
        {
        case ' ' ... '~': _build += k;
            break;
        case '\r': break; // semplicemente ignora i '\r'
        case '\n':
        {
            s = _build;
            _build.clear();
            return true;
        };
            break;
        default: _build += _inval;
        }
    }
    return false;
}





CXU_NS_END

//.
