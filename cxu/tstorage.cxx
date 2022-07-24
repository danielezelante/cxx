// YAL zeldan


#include "tstorage.hxx"

#include "memory.hxx"
#include "exception.hxx"
#include "string.hxx"
#include "mymacros.hxx"
#include "logger.hxx"

CXU_NS_BEGIN


//**** TIStorage

TIStorage::TIStorage(const Data & data)
: _data(data)
, _ptr(0)
{
}

TIStorage::~TIStorage()
{
    for (; _ptr < _data.size(); ++_ptr)
        if (!strchr(" \t\r\n", getCurrent())) break;

    if (_ptr != _data.size())
        LOGGER.error(
                     Format("NOT EOF (%$<%$)")
                     % utod(_ptr)
                     % utod(_data.size())
                     );
}

char TIStorage::read()
{
    if (_ptr >= _data.size()) THROWDATAEXCEPTION("EOF");
    char k = qread();
    static const char * s1 = "([{<";
    static const char * s2 = ")]}>";
    const char * p1 = strchr(s1, k);
    const char * p2 = strchr(s2, k);
    ASSERT(!(p1 && p2));
    if (p1)
        _nesting.push_back(k);

    if (p2)
    {
        ASSERT(!_nesting.empty());
        ASSERT(_nesting.back() == s1[p2 - s2]);
        _nesting.pop_back();
    }
    return k;
}

char TIStorage::qread()
{
    if (_ptr >= _data.size()) THROWDATAEXCEPTION("EOF");
    const char k = getCurrent();
    ++_ptr;
    return k;
}

char TIStorage::qpeek()
{
    if (_ptr >= _data.size()) THROWDATAEXCEPTION("EOF");
    return getCurrent();
}

void TIStorage::qskip(size_t n)
{
    _ptr += n;
    if (_ptr > _data.size()) THROWDATAEXCEPTION("EOF");
}

void TIStorage::qskip()
{
    ++_ptr;
    if (_ptr > _data.size()) THROWDATAEXCEPTION("EOF");
}

const char * TIStorage::ptr() const
{
    return _data.cptr() + _ptr;
}

TIStorage & TIStorage::operator>>(Cmd op)
{
    switch (op)
    {
    case BEGIN: (*this) >> "{";
        break;
    case END: (*this) >> "}";
        break;
    case ABORT:
    {
        const size_t s = _nesting.size();
        size_t j;
        for (j = s; j > 0; --j) if (_nesting[j - 1] == '{') break;
        while (_nesting.size() > j - 1) read();
    };
        break;
    }
    return *this;
}

TIStorage & TIStorage::operator>>(bool & x)
{
    eatWhite();
    const char k = ::toupper(read());

    switch (k)
    {
    case 'T':
    case 'Y':
    case 'J':
    case 'S':
    case '1':
        x = true;
        break;

    case 'F':
    case 'N':
    case '0':
        x = false;
        break;

    default:
        THROWDATAEXCEPTION(Format("invalid bool value: %$") % std::string(1, k));
    }

    return *this;
}

TIStorage & TIStorage::operator>>(unsigned int & x)
{
    eatWhite();
    char * ep;
    const char * p = ptr();
    x = stou(p, &ep);
    if (ep == p) THROWDATAEXCEPTION(quote(p));
    qskip(ep - p);

    return *this;
}

TIStorage & TIStorage::operator>>(int & x)
{
    eatWhite();
    char * ep;
    const char * p = ptr();
    x = strtol(p, &ep, 10);
    if (ep == p) THROWDATAEXCEPTION(quote(p));
    qskip(ep - p);

    return *this;
}

TIStorage & TIStorage::operator>>(unsigned long & x)
{
    eatWhite();
    char * ep;
    const char * p = ptr();
    x = stou(p, &ep);
    if (ep == p) THROWDATAEXCEPTION(quote(p));
    qskip(ep - p);

    return *this;
}

TIStorage & TIStorage::operator>>(long & x)
{
    eatWhite();
    char * ep;
    const char * p = ptr();
    x = strtol(p, &ep, 10);
    if (ep == p) THROWDATAEXCEPTION(quote(p));
    qskip(ep - p);

    return *this;
}

#ifndef _LP64

TIStorage & TIStorage::operator>>(uint64_t & x)
{
    eatWhite();
    char * ep;
    const char * p = ptr();
    x = stou64(p, &ep);
    if (ep == p) THROWDATAEXCEPTION(quote(p));
    qskip(ep - p);

    return *this;
}

TIStorage & TIStorage::operator>>(int64_t & x)
{
    eatWhite();
    char * ep;
    const char * p = ptr();
    x = stoi64(p, &ep);
    if (ep == p) THROWDATAEXCEPTION(quote(p));
    qskip(ep - p);

    return *this;
}
#endif

TIStorage & TIStorage::operator>>(double & x)
{
    eatWhite();
    char * ep;
    const char * p = ptr();

    x = strtod(p, &ep);
    if (ep == p) THROWDATAEXCEPTION(quote(p));
    qskip(ep - p);

    return *this;
}

TIStorage & TIStorage::operator>>(const char * s)
{
    eatWhite();
    while (*s)
    {
        if (read() != *s) THROWDATAEXCEPTION(quote(s));
        ++s;
    }

    return *this;
}

TIStorage & TIStorage::operator>>(std::string & a)
{
    eatWhite();
    std::string qa;

    const char v = qread();
    if (v != '\"') THROWDATAEXCEPTION("not a '\"'");
    qa += v;

    for (;;)
    {
        char k = qread();
        qa += k;
        if (k == '\"') break;
        if (k == '\\') qa += qread();
    }
    a = unquote(qa);

    return *this;
}

TIStorage & TIStorage::operator>>(std::wstring & a)
{
    eatWhite();
    std::string qa;

    const char v = qread();
    if (v != '\"') THROWDATAEXCEPTION("not a '\"'");
    qa += v;

    for (;;)
    {
        char k = qread();
        qa += k;
        if (k == '\"') break;
        if (k == '\\') qa += qread();
    }
    a = wunquote(qa);

    return *this;
}

void TIStorage::eatWhite()
{
    while (strchr(" \t\r\n", qpeek())) qskip();
}

//**** TOStorage

TOStorage::TOStorage(Buffer & data)
: _data(data)
, _ptr(0)
, _needspaceafter(false)
, _newline(true)
, _error(false)
{
    _data.clear();
}

TOStorage::~TOStorage()
{
    if (!_newline) qwrite('\n');
    _data.resize(_ptr);
    ASSERT(_radix.empty() || _error);
    ASSERT(_nesting.empty() || _error);
}

/*
void TOStorage::throws(const char * str, const char * file, int line)
{
        _error = true;
        throw CXUException(str,file,line,stacktrace());
}

void TOStorage::throws(const char * str)
{
        _error = true;
        throw CXUException(str);
}
 */


TOStorage & TOStorage::operator<<(const char * s)
{
    write(s);
    return *this;
}

TOStorage & TOStorage::operator<<(Cmd w)
{
    switch (w)
    {
    case POP: ASSERT(!_radix.empty());
        _radix.pop();
        break;
    case NL: if (!_newline) qwrite('\n');
        _newline = true;
        _needspaceafter = false;
        break;
    case BLANK: (*this) << NL;
        qwrite('\n');
        break;
    case BEGIN: (*this) << NL << "{" << NL;
        break;
    case END: (*this) << NL << "}" << NL;
        break;

    default: ASSERT(false);
    }

    return *this;
}

TOStorage & TOStorage::operator<<(Radix w)
{
    _radix.push(w);
    return *this;
}

TOStorage & TOStorage::operator<<(bool x)
{
    write(x ? "T" : "F");
    return *this;
}

TOStorage & TOStorage::operator<<(int n)
{
    indent();
    if (_needspaceafter) qwrite(' ');
    qreserve(sizeof (n)*3 + 2); //3 for base10, 2 for sign and null
    qskip(itod(n, cur()));
    _needspaceafter = true;
    _newline = false;
    return *this;
}

TOStorage & TOStorage::operator<<(unsigned int n)
{
    ASSERT(!_radix.empty());
    indent();
    if (_needspaceafter) qwrite(' ');
    qreserve(sizeof (n) * CHAR_BIT + 1);
    char * a = cur();
    size_t sz = 0;

    switch (_radix.top())
    {
    case DEC: sz = utod(n, a);
        break;
    case HEX: sz = utox(n, a, '$');
        break;
    case BIN: sz = utob(n, a, '%');
        break;
    default: ASSERT(false);
    }

    qskip(sz);
    _needspaceafter = true;
    _newline = false;

    return *this;
}

TOStorage & TOStorage::operator<<(long n)
{
    indent();
    if (_needspaceafter) qwrite(' ');
    qreserve(sizeof (n)*3 + 2); //3 for base10, 2 for sign and null
    qskip(itod(n, cur()));
    _needspaceafter = true;
    _newline = false;
    return *this;
}

TOStorage & TOStorage::operator<<(unsigned long n)
{
    ASSERT(!_radix.empty());
    indent();
    if (_needspaceafter) qwrite(' ');
    qreserve(sizeof (n) * CHAR_BIT + 1);
    char * a = cur();
    size_t sz = 0;

    switch (_radix.top())
    {
    case DEC: sz = utod(n, a);
        break;
    case HEX: sz = utox(n, a, '$');
        break;
    case BIN: sz = utob(n, a, '%');
        break;
    default: ASSERT(false);
    }

    qskip(sz);
    _needspaceafter = true;
    _newline = false;

    return *this;
}

#ifndef _LP64

TOStorage & TOStorage::operator<<(int64_t n)
{
    indent();
    if (_needspaceafter) qwrite(' ');
    qreserve(sizeof (n)*3 + 2); //3 for base10, 2 for sign and null
    qskip(itod(n, cur()));
    _needspaceafter = true;
    _newline = false;
    return *this;
}

TOStorage & TOStorage::operator<<(uint64_t n)
{
    ASSERT(!_radix.empty());
    indent();
    if (_needspaceafter) qwrite(' ');
    qreserve(sizeof (n) * CHAR_BIT + 1);
    char * a = cur();
    size_t sz = 0;

    switch (_radix.top())
    {
    case DEC: sz = utod(n, a);
        break;
    case HEX: sz = utox(n, a, '$');
        break;
    case BIN: sz = utob(n, a, '%');
        break;
    default: ASSERT(false);
    }

    qskip(sz);
    _needspaceafter = true;
    _newline = false;

    return *this;
}

#endif

void TOStorage::write(char k)
{
    ASSERT(k != 0x22); //eclipse does not like '"'
    ASSERT(k >= 32);
    //ASSERT(k<128);

    static const char * s1 = "([{<";
    static const char * s2 = ")]}>";
    static const char * s3 = "`~!@#$%^&*-=+;:,<.>/?|";
    const char * p1 = strchr(s1, k);
    const char * p2 = strchr(s2, k);
    const char * p3 = strchr(s3, k);
    ASSERT(!(p1 && p2));
    if (!(p1 || p2))
    {
        indent();
        if (_needspaceafter && !p3) qwrite(' ');
        qwrite(k);
    }
    else
    {
        if (p1)
        {
            indent();
            qwrite(k);
            _nesting.push(k);
        }

        if (p2)
        {
            ASSERT(!_nesting.empty());
            ASSERT(_nesting.top() == s1[p2 - s2]);
            _nesting.pop();
            indent();
            qwrite(k);
        }
    }
    _newline = false;
    _needspaceafter = false;
}

void TOStorage::write(const char * s)
{
    while (*s)
    {
        write(*s);
        ++s;
    }
}

void TOStorage::qwrite(char k)
{
    _data.resize(_ptr + 1);
    refCurrent() = k;
    ++_ptr;
}

void TOStorage::qwrite(const char * s)
{
    while (*s) qwrite(*(s++));
}

TOStorage & TOStorage::operator<<(const std::string & a)
{
    indent();
    std::string qa = quote(a);
    qwrite(qa.data());
    _newline = false;
    return *this;
}

TOStorage & TOStorage::operator<<(const std::wstring & a)
{
    indent();
    std::string qa = quote(a);
    qwrite(qa.data());
    _newline = false;
    return *this;
}

TOStorage & TOStorage::operator<<(double x)
{
    ASSERT(!_radix.empty());

    indent();
    if (_needspaceafter) qwrite(' ');

    const size_t z = 256;
    qreserve(z);
    size_t skip = 0;
    char * p = cur();
    switch (_radix.top())
    {
    case DEC: skip = snprintf(p, z, "%.15g", x);
        break;
    case BIN:
    case HEX: skip = snprintf(p, z, "%a", x);
        break;
    default: ASSERT(false);
    }

    qskip(skip);
    _needspaceafter = true;
    return *this;
}

char * TOStorage::cur() const
{
    return _data.ptr() + _ptr;
}

void TOStorage::qskip(size_t n)
{
    ASSERT(_ptr + n <= _data.size());
    _ptr += n;
}

void TOStorage::qreserve(size_t n)
{
    _data.resize(_ptr + n);
}

void TOStorage::indent()
{
    if (_newline) LOOP(j, _nesting.size()) qwrite(' ');
}

CXU_NS_END


//.
