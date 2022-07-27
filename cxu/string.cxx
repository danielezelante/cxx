// YAL zeldan


#include <set>
#include <limits>
#include <wctype.h>
#include <wchar.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "string.hxx"

#include "defs.hxx"
#include "globals.hxx"
#include "exception.hxx"
#include "integer.hxx"
#include "mymacros.hxx"
#include "mystd.hxx"

CXU_NS_BEGIN


char val2sym(int n)
{
    if (n < 0) return '!';
    if (n < 10) return '0' + n;
    if (n < 36) return 'A' + n - 10;
    return '?';
}

int sym2val(char k)
{
    if (incc(k, '0', '9')) return k - '0';
    if (incc(k, 'A', 'Z')) return k - 'A' + 10;
    if (incc(k, 'a', 'z')) return k - 'a' + 10;
    return -1;
}

size_t itod(int n, char * s, bool plus)
{
    if (n > 0)

    {
        if (plus) *(s++) = '+';
        return utod(unsign(n), s);
    }

    if (n < 0)
    {
        *(s++) = '-';
        return utod(unsign(-n), s);
    }

    *(s++) = '0';
    *s = 0;

    return 1;
}

std::string itod(int n)
{
    char tmp[sizeof (n)*3 + 2];
    itod(n, tmp);
    return std::string(tmp);
}

size_t itod(long n, char * s, bool plus)
{
    if (n > 0)

    {
        if (plus) *(s++) = '+';
        return utod(unsign(n), s);
    }

    if (n < 0)
    {
        *(s++) = '-';
        return utod(unsign(-n), s);
    }

    *(s++) = '0';
    *s = 0;

    return 1;
}

std::string itod(long n)
{
    char tmp[sizeof (n)*3 + 2];
    itod(n, tmp);
    return std::string(tmp);
}

std::string ftod(double x)
{
    char str[256];
    snprintf(str, DIM(str), "%.15g", x);
    return std::string(str);
}

std::string ftox(double x)
{
    char str[256];
    snprintf(str, DIM(str), "%a", x);
    return str;
}

std::string realeng(double x, unsigned int digits, bool plus)
{
    ASSERT(digits >= 3);
    ASSERT(digits < 128);

    char strf[16];
    snprintf(strf, DIM(strf), "%%+.0%ue", digits);

    char str[16 + digits];
    snprintf(str, DIM(str), strf, x);
    int expo = atoi(strchr(str, 'e') + 1);
    int modu = expo % 3;
    if (modu < 0) modu += 3;
    std::string s;
    if ((plus && x > 0.0) || str[0] == '-') s += str[0]; // + or -, but only copy -
    s += str[1]; // 1st digit
    // skip 2, it is .
    LOOP(j, modu) s += str[3 + j]; // copy digits
    s += '.';
    for (size_t j = 3 + modu; str[j] != 'e'; ++j) s += str[j];
    if (expo - modu)
    {
        s += 'e';
        s += itod(expo - modu);
    }

    return s;
}

std::string realfix(double x, unsigned int digits)
{
    double k = std::numeric_limits<double>::epsilon();
    if (!inoo(::fabs(x), k, 1 / k))
        return realeng(x, std::max(digits, 3U));
    else
    {
        char str[256]; // dovrebbe bastare in relazione a epsilon
        ::snprintf(str, DIM(str), "%.*f", digits, x);
        return std::string(str);
    }
}

std::string trim(const std::string & s, const char * pattern)
{
    size_t n = s.size();
    size_t j;
    for (j = 0; j < n; ++j) if (!strchr(pattern, s[j])) break;
    size_t k;
    for (k = n; k > 0; --k) if (!strchr(pattern, s[k - 1])) break;

    return s.substr(j, k - j);
}

std::string rtrim(const std::string & s, const char * pattern)
{
    size_t n = s.size();
    size_t k;
    for (k = n; k > 0; --k) if (!strchr(pattern, s[k - 1])) break;
    return s.substr(0, k);
}

double stof(const char * s)
{
    char * endptr;
    const double x = strtod(s, &endptr);
    if (s + strlen(s) != endptr)
        return std::numeric_limits<double>::signaling_NaN();
    return x;
}

double stof(const std::string & s)
{
    return stof(s.data());
}

unsigned int stou(const char * s, char ** endptr)
{
    if (isdigit(s[0])) return static_cast<unsigned int> (strtoul(s, endptr, 0)); //gestisce anche i casi 0 (ottale) e 0x (hex)
    switch (s[0])
    {
    case '$': return static_cast<unsigned int> (strtoul(s + 1, endptr, 16));
    case '%': return static_cast<unsigned int> (strtoul(s + 1, endptr, 2));
    case 'o': return static_cast<unsigned int> (strtoul(s + 1, endptr, 8));
    default: THROWDATAEXCEPTION(quote(s));
    }

    ASSERT(false);
    return 0;
}

unsigned int stou(const char * s)
{
    char * endptr = 0;
    int n = stou(s, &endptr);
    if (s + strlen(s) != endptr)
        THROWDATAEXCEPTION(quote(s));
    return n;
}

unsigned int stou(const std::string & s)
{
    return stou(s.data());
}

unsigned int stou(const wchar_t * s, wchar_t ** endptr)
{
    if (iswdigit(s[0])) return static_cast<unsigned int> (wcstoul(s, endptr, 10));
    switch (s[0])
    {
    case '$': return static_cast<unsigned int> (wcstoul(s + 1, endptr, 16));
    case '%': return static_cast<unsigned int> (wcstoul(s + 1, endptr, 2));
    case 'o': return static_cast<unsigned int> (wcstoul(s + 1, endptr, 8));
    default: THROWDATAEXCEPTION(quote(s));
    }

    ASSERT(false);
    return 0;
}

unsigned int stou(const wchar_t * s)
{
    wchar_t * endptr = 0;
    int n = stou(s, &endptr);
    if (s + wcslen(s) != endptr)
        THROWDATAEXCEPTION(quote(s));
    return n;
}

unsigned int stou(const std::wstring & s)
{
    return stou(s.data());
}

double stof(const wchar_t * s)
{
    wchar_t * endptr;
    const double x = wcstod(s, &endptr);
    if (s + wcslen(s) != endptr)
        return std::numeric_limits<double>::signaling_NaN();
    return x;
}

double stof(const std::wstring & s)
{
    return stof(s.data());
}

unsigned long stoul(const char * s, char ** endptr)
{
    if (isdigit(s[0])) return strtoul(s, endptr, 0);
    switch (s[0])
    {
    case '$': return strtoul(s + 1, endptr, 16);
    case '%': return strtoul(s + 1, endptr, 2);
    case 'o': return strtoul(s + 1, endptr, 8);
    default: THROWDATAEXCEPTION(s);
    }

    ASSERT(false);
    return 0;
}

unsigned long stoul(const char * s)
{
    char * endptr = 0;
    int n = stoul(s, &endptr);
    if (s + strlen(s) != endptr)
        THROWDATAEXCEPTION(s);
    return n;
}

unsigned long stoul(const std::string & s)
{
    return stoul(s.data());
}

uint64_t stou64(const char * s)
{
    char * endptr = 0;
    const uint64_t n = stou64(s, &endptr);
    if (s + strlen(s) != endptr)
        THROWDATAEXCEPTION(s);
    return n;
}

uint64_t stou64(const std::string & s)
{
    return stou64(s.data());
}

uint64_t stou64(const char * s, char ** endptr)
{
    if (isdigit(s[0])) return strtoull(s, endptr, 0);
    switch (s[0])
    {
    case '$': return strtoull(s + 1, endptr, 16);
    case '%': return strtoull(s + 1, endptr, 2);
    case 'o': return strtoull(s + 1, endptr, 8);
    default: THROWDATAEXCEPTION(s);
    }
}

int64_t stoi64(const char * s)
{
    char * endptr = 0;
    const int64_t n = stoi64(s, &endptr);
    if (s + strlen(s) != endptr)
        THROWDATAEXCEPTION(s);
    return n;
}

int64_t stoi64(const std::string & s)
{
    return stoi64(s.data());
}

int64_t stoi64(const char * s, char ** endptr)
{
    return strtoll(s, endptr, 0);
}


#if 0

int stoi(const char * s, char ** endptr)
{
#if defined(_ILP32)
    return static_cast<int> (strtol(s, endptr, 0));
#else
    const long x = strtol(s, endptr, 0);
    if (!incc(
              x,
              static_cast<long> (std::numeric_limits<int>::min()),
              static_cast<long> (std::numeric_limits<int>::max())
              ))
        THROWDATAEXCEPTION(Format("overflow: %$") % s);
    return static_cast<int> (x);
#endif
}

int stoi(const char * s)
{
    char * endptr = 0;
    int n = stoi(s, &endptr);
    if (s + strlen(s) != endptr)
        THROWDATAEXCEPTION(quote(s));
    return n;
}

int stoi(const std::string & s)
{
    return stoi(s.data());
}

int stoi(const wchar_t * s, wchar_t ** endptr)
{
#if defined(_ILP32)
    return static_cast<int> (wcstol(s, endptr, 10));
#else
    const long x = wcstol(s, endptr, 10);
    if (!incc(
              x,
              static_cast<long> (std::numeric_limits<int>::min()),
              static_cast<long> (std::numeric_limits<int>::max())
              ))
        THROWDATAEXCEPTION(Format("overflow: %$") % x);
    return static_cast<int> (x);
#endif
}

int stoi(const wchar_t * s)
{
    wchar_t * endptr = 0;
    int n = stoi(s, &endptr);
    if (s + wcslen(s) != endptr)
        THROWDATAEXCEPTION(quote(s));
    return n;
}

int stoi(const std::wstring & s)
{
    return stoi(s.data());
}

#endif

std::string ptrtoa(const void * p, const char * prefix)
{
    char a[strlen(prefix) + 2 * sizeof (p) + 16];
    ::strncpy(a, prefix, DIM(a));
    char * q = a + strlen(a);

#ifdef _LP64
    unsigned long n = reinterpret_cast<unsigned long> (p);
    utobase(n, q, 16, 2 * sizeof (p));
    return std::string(a);
#endif

#ifdef _ILP32
    size_t n = reinterpret_cast<size_t> (p);
    utobase(n, q, 16, 2 * sizeof (p));
    return std::string(a);
#endif

#if !defined(_LP64) && !defined(_ILP32) 
    if (sizeof (void*) == sizeof (unsigned long))
    {
        unsigned long n = reinterpret_cast<unsigned long> (p);
        utobase(n, q, 16, 2 * sizeof (p));
        return string(a);
    }
    if (sizeof (void*) == sizeof (unsigned int))
    {
        unsigned int n = reinterpret_cast<unsigned int> (p);
        utobase(n, q, 16, 2 * sizeof (p));
        return std::string(a);
    }

    sprintf(q, "%p", p);
    return std::string(a);
#endif



}

std::string ptrtoas(const void * p, const char * prefix)
{
    std::string a = ptrtoa(p, "");
    for (size_t j = sizeof (p); j > 0; j /= 2)
        if (a.length() == j * 2 && a.substr(0, j) == std::string(j, '0'))
            a.erase(0, j);
    return std::string(prefix) + a;
}

std::string threadidtoa(pthread_t t)
{
    if (sizeof (pthread_t)>sizeof (void*)) return std::string("@n/a");
    return ptrtoa(reinterpret_cast<void*> (t), "@$");
}




#if 0
//*/////////////////// Format

Format::Format(const char * s)
: _form(s)
{
}

Format::Format(const std::string & s)
: _form(s)
{
}

Format & Format::operator%(const char * s)
{
    push_str(s);
    return *this;
}

Format & Format::operator%(const std::string & s)
{
    push_str(s);
    return *this;
}

Format & Format::operator%(const Stringable & a)
{
    push_ref(a);
    return *this;
}

Format & Format::operator%(int x)
{
    push_str(itod(x));
    return *this;
}

Format & Format::operator%(unsigned int x)
{
    push_str(utod(x));
    return *this;
}

Format & Format::operator%(long x)
{
    push_str(itod(x));
    return *this;
}

Format & Format::operator%(unsigned long x)
{
    push_str(utod(x));
    return *this;
}

Format & Format::operator%(double x)
{
    push_str(ftod(x));
    return *this;
}

#ifndef _LP64

Format & Format::operator%(int64_t x)
{
    char s[32];
    sprintf(s, "%lld", x);
    push_str(std::string(s));
    return *this;
}

Format & Format::operator%(uint64_t x)
{
    char s[32];
    sprintf(s, "%llu", x);
    push_str(std::string(s));
    return *this;
}

#endif

std::string Format::str() const
{
    size_t strndx = 0;
    size_t refndx = 0;

    std::string q;

    size_t pos = 0;

    const char * frm = _form.c_str();
    while (*frm)
    {
        if (*frm != '%')
        {
            q += (*frm);
            frm += 1;
            continue;
        };

        if (*(frm + 1) == '$')
        {
            ASSERT(pos < _type.size());
            switch (_type[pos++])
            {
            case e_str:
                ASSERT(strndx < _strdata.size());
                q += _strdata[strndx++];
                break;
            case e_ref:
                ASSERT(refndx < _refdata.size());
                q += _refdata[refndx++].get(); // here lazy evaluation wakes
                break;
            default: ASSERT(false);
            }
            frm += 2;
            continue;
        };

        if (*(frm + 1) == '%')
        {
            q += '%';
            frm += 2;
            continue;
        };

        THROWINTERFACEEXCEPTION(quote(frm));

    };

    if (pos != _type.size())
    {
        const auto myerr = xsprintf(
            "in %s: tag_count = %zd, data_size = %zu",
            cxu::quote(frm),
            pos,
            _type.size()
        );
        CXU_THROWINTERFACEEXCEPTION(myerr);
    }


    return q;
}




//** WFormat **************************

WFormat::WFormat(const wchar_t * s)
: _form(s)
{
}

WFormat::WFormat(const char * s)
: _form(widen(std::string(s)))
{
}

WFormat::WFormat(const std::wstring & s)
: _form(s)
{
}

WFormat::WFormat(const std::string & s)
: _form(widen(s))
{
}

WFormat & WFormat::operator%(const wchar_t * s)
{
    _data.push_back(std::wstring(s));
    return *this;
}

WFormat & WFormat::operator%(const char * s)
{
    _data.push_back(widen(std::string(s)));
    return *this;
}

WFormat & WFormat::operator%(const std::wstring & s)
{
    _data.push_back(s);
    return *this;
}

WFormat & WFormat::operator%(const std::string & s)
{
    _data.push_back(widen(s));
    return *this;
}

WFormat & WFormat::operator%(const Stringable & a)
{
    _data.push_back(widen(a.str()));
    return *this;
}

WFormat & WFormat::operator%(int x)
{
    _data.push_back(widen(itod(x)));
    return *this;
}

WFormat & WFormat::operator%(unsigned int x)
{
    _data.push_back(widen(utod(x)));
    return *this;
}

WFormat & WFormat::operator%(long x)
{
    _data.push_back(widen(itod(x)));
    return *this;
}

WFormat & WFormat::operator%(unsigned long x)
{
    _data.push_back(widen(utod(x)));
    return *this;
}

WFormat & WFormat::operator%(double x)
{
    _data.push_back(widen(ftod(x)));
    return *this;
}

#ifndef _LP64

WFormat & WFormat::operator%(int64_t x)
{
    char s[32];
    sprintf(s, "%lld", x);
    _data.push_back(widen(std::string(s)));
    return *this;
}

WFormat & WFormat::operator%(uint64_t x)
{
    char s[32];
    sprintf(s, "%llu", x);
    _data.push_back(widen(std::string(s)));
    return *this;
}

#endif

std::wstring WFormat::str() const
{
    return format(_form.data(), _data);
}

#endif

//******************

std::vector<std::string> parse
(
 const std::string & src,
 bool usespc,
 const char * toks,
 bool toka,
 bool sdla,
 const char * coms
 )
{
    std::set<char> tok;
    if (toks)
        for (; *toks; ++toks)
            tok.insert(*toks);

    if (usespc)
    {
        tok.erase(' ');
        tok.erase('\t');
    }

    std::set<char> com;
    if (coms)
        for (; *coms; ++coms)
            if (tok.find(*coms) == tok.end())
                com.insert(*coms);


    std::vector<std::string> array;
    std::string build;
    int state = 6;

    for (auto it : src)
    {
        char current = it;
        bool curriscom = com.find(current) != com.end();
        bool currissdl = (current == '\"');
        bool currisspc = strchr(" \t", current);

        switch (state)
        {
        case 6:
        {
            if (curriscom) return array;
            if (usespc && currisspc)
            {
                state = 1;
                break;
            };
            if (tok.find(current) != tok.end())
            {
                array.push_back(build);
                build.clear();
                if (toka) array.push_back(std::string(1, current));
                state = 1;
                break;
            };
            if (currissdl)
            {
                if (sdla) build += current;
                state = 4;
                break;
            };
            build += current;
            state = 2;
        };
            break;

        case 1:
        {
            if (curriscom)
            {
                array.push_back(build);
                return array;
            }
            if (usespc) if (currisspc) break;
            if (tok.find(current) != tok.end())
            {
                array.push_back(build);
                build.clear();
                if (toka) array.push_back(std::string(1, current));
                break;
            };
            if (currissdl)
            {
                if (sdla) build += current;
                state = 4;
                break;
            };
            build += current;
            state = 2;
        };
            break;

        case 2:
        {
            if (curriscom)
            {
                array.push_back(build);
                return array;
            };
            if (usespc) if (currisspc)
                {
                    state = 3;
                    break;
                };
            if (tok.find(current) != tok.end())
            {
                array.push_back(build);
                build.clear();
                if (toka) array.push_back(std::string(1, current));
                state = 1;
                break;
            };
            if (currissdl)
            {
                array.push_back(build);
                build.clear();
                if (sdla) build += current;
                state = 4;
                break;
            };
            build += current;

        };
            break;

        case 3:
        {
            if (curriscom)
            {
                array.push_back(build);
                return array;
            };
            if (usespc) if (currisspc) break;
            if (tok.find(current) != tok.end())
            {
                array.push_back(build);
                build.clear();
                if (toka) array.push_back(std::string(1, current));
                state = 1;
                break;
            };
            if (currissdl)
            {
                array.push_back(build);
                build.clear();
                if (sdla) build += current;
                state = 4;
                break;
            };

            array.push_back(build);
            build.clear();
            build += current;
            state = 2;
        };
            break;

        case 4:
        {
            if (currissdl)
            {
                if (sdla) build += current;
                state = 3;
                break;
            };
            if (current == '\\')
            {
                state = 5;
                break;
            };
            build += current;
        };
            break;

        case 5:
        {
            build += current;
            state = 4;
        };
            break;

        default: ASSERT(false);
        };
    };

    if (state == 1 || state == 2 || state == 3) array.push_back(build);

    return array;
}

std::vector<std::string> dqparse(const std::string & src, char comment)
{
    std::vector<std::string> array;
    std::string build;
    int state = 1;

    for (auto it : src)
    {
        char current = it;
        bool curriscom = (current == comment);
        bool currissdl = (current == '\"');
        bool currisspc = isspace(current);

        switch (state)
        {
        case 1:
        {
            if (curriscom) return array;
            if (currisspc) break;
            if (currissdl)
            {
                build += current;
                state = 4;
                break;
            }
            build += current;
            state = 2;
        };
            break;

        case 2:
        {
            if (curriscom)
            {
                array.push_back(build);
                return array;
            }
            if (currisspc)
            {
                state = 3;
                break;
            }
            if (currissdl)
            {
                array.push_back(build);
                build.clear();
                build += current;
                state = 4;
                break;
            }
            build += current;

        };
            break;

        case 3:
        {
            if (curriscom)
            {
                array.push_back(build);
                return array;
            }
            if (currisspc) break;
            if (currissdl)
            {
                array.push_back(build);
                build.clear();
                build += current;
                state = 4;
                break;
            }

            array.push_back(build);
            build.clear();
            build += current;
            state = 2;
        };
            break;

        case 4:
        {
            if (currissdl)
            {
                build += current;
                state = 3;
                break;
            }
            if (current == '\\')
            {
                build += current;
                state = 5;
                break;
            };
            build += current;
        };
            break;

        case 5:
        {
            switch (current)
            {
            case '\\':
            case '\"':
                build += current;
                break;

            default: THROWDATAEXCEPTION(Format("invalid string escape: %$") % quote(src));
            }
            state = 4;
        };
            break;

        default: ASSERT(false);
        };
    };

    if (state == 2 || state == 3) array.push_back(build);

    return array;
}

void strupr(char * str)
{
    while (*str)
    {
        *str = static_cast<char> (::toupper(*str));
        ++str;
    }
}

void strlwr(char * str)
{
    while (*str)
    {
        *str = static_cast<char> (::tolower(*str));
        ++str;
    }
}

void makeupper(std::string & str)
{
    size_t n = str.size();
    LOOP(j, n) str.at(j) = static_cast<char> (::toupper(str.at(j)));
}

void makelower(std::string & str)
{
    size_t n = str.size();
    LOOP(j, n) str.at(j) = static_cast<char> (::tolower(str.at(j)));
}

std::string toupper(const std::string & a)
{
    std::string b;
    for (auto it : a) b += static_cast<char> (::toupper(it));
    return b;
}

std::string tolower(const std::string & a)
{
    std::string b;
    for (auto it : a) b += static_cast<char> (::tolower(it));
    return b;
}

/*
std::string rjus(const std::string & a, std::string::size_type n, char c)
{
        std::string::size_type w = a.size();
        if(w>=n) return a;
        return std::string(n-w,c)+a;
}

std::string ljus(const std::string & a, std::string::size_type n, char c)
{
        std::string::size_type w = a.size();
        if(w>=n) return a;
        return a+std::string(n-w,c);
}
 */

std::string quote(char k)
{
    std::string a;
    a.append("\'");
    switch (k)
    {
    case '\\': a.append("\\\\");
        break;
    case '\'': a.append("\\\'");
        break;
    case '\"': a.append("\\\"");
        break;
    case '\n': a.append("\\n");
        break;
    case '\r': a.append("\\r");
        break;

    default:
        if (static_cast<signed char> (k) < 32)
        {
            char tmp[] = "\\xHH";
            CXU_ASSERT(sizeof (tmp) == 5);
            utobase(unsign(k), tmp + 2, 16, 2);
            a += tmp;
        }
        else
            a += k;
    }

    a.append("\'");
    return a;
}

std::string quote(const char * s)
{
    std::string a;
    a.append("\"");

    while (*s)
    {
        switch (*s)
        {
        case '\\': a.append("\\\\");
            break;
        case '\'': a.append("\\\'");
            break;
        case '\"': a.append("\\\"");
            break;
        case '\n': a.append("\\n");
            break;
        case '\r': a.append("\\r");
            break;

        default:
            if (static_cast<signed char> (*s) < 32)
            {
                char tmp[] = "\\xHH";
                CXU_ASSERT(sizeof (tmp) == 5);
                utobase(unsign(*s), tmp + 2, 16, 2);
                a += tmp;
            }
            else
                a += *s;
        }
        ++s;
    }

    a.append("\"");
    return a;
}

std::string quote(const std::string & s)
{
    return quote(s.data());
}

std::string quoteifneeded(const char * ks)
{
    const char * s = ks;
    bool q = false;
    while (*s)
    {
        if (*s <= ' ') q = true;
        if (*s > '~') q = true;
        if (*s == '#') q = true; // per i commenti, potrebbe essere superfluo in altri casi
        ++s;
    }

    return q ? quote(ks) : ks;
}

std::string quoteifneeded(const std::string & s)
{
    return quoteifneeded(s.data());
}

std::string unquote(const char * s)
{
    std::string x;

    while (*s)
    {
        while (isspace(*s)) ++s;
        if (*s != '"') THROWDATAEXCEPTION(Format("invalid quoted string: ") % s);
        ++s;

        while (*s != '"')
        {
            if (*s == '\\')
            {
                ++s;
                switch (*s)
                {
                case '\\':
                case '\'':
                case '"':
                    x += *s;
                    break;

                case 'r':
                    x += L'\r';
                    break;

                case 'n':
                    x += L'\n';
                    break;

                case 'x':
                {
                    const size_t z = 2;
                    char k[2 + z];
                    k[0] = '$';
                    k[1 + z] = '\0';

                    LOOP(j, z)
                    {
                        ++s;
                        if (!*s) THROWDATAEXCEPTION(Format("invalid quoted string: ") % s);
                        k[j + 1] = *s;
                    }
                    x += stou(k);
                };
                    break;

                default:
                    THROWDATAEXCEPTION(Format("invalid quoted string: ") % s);
                }
            }
            else
                x += static_cast<wchar_t> (*s);

            ++s;
        }
        ++s;
    }

    return x;
}

std::string unquote(const std::string & s)
{
    return unquote(s.data());
}

std::string tryunquote(const char * s)
{
    const size_t z = strlen(s);
    if (z == 0) return s;
    if (s[0] != '"') return s;
    if (s[z - 1] != '"') return s;
    return unquote(s);
}

std::string tryunquote(const std::string & s)
{
    const size_t z = s.size();
    if (z == 0) return s;
    if (s[0] != '"') return s;
    if (s[z - 1] != '"') return s;
    return unquote(s);
}

std::string quote(const wchar_t * s)
{
    std::string a;
    a.append("\"");

    while (*s)
    {
        switch (*s)
        {
        case '\\': a.append("\\\\");
            break;
        case '\'': a.append("\\\'");
            break;
        case '\"': a.append("\\\"");
            break;
        case '\n': a.append("\\n");
            break;
        case '\r': a.append("\\r");
            break;

        default:
            if (*s < 32 || *s > 127)
            {
                char tmp[] = "\\xHH";
                CXU_ASSERT(sizeof (tmp) == 5);
                utobase(unsign(*s), tmp + 2, 16, 2);
                a += tmp;
            }
            else if (*s >= 0x100)
            {
                if (sizeof (wchar_t) == 2)
                {
                    char tmp[] = "\\uHHHH";
                    utobase(*s, tmp + 2, 16, 4);
                    a += tmp;
                }
                else if (sizeof (wchar_t) == 4)
                {
                    char tmp[] = "\\wHHHHHHHH";
                    utobase(*s, tmp + 2, 16, 8);
                    a += tmp;
                }
                else
                {
                    // TODO different unicode sizes
                    ASSERT(false);
                }
            }
            else
                a += *s;
        }
        ++s;
    }

    a.append("\"");
    return a;
}

std::string quote(const std::wstring & s)
{
    return quote(s.data());
}

std::wstring wunquote(const char * s)
{
    std::wstring x;

    while (*s)
    {
        while (isspace(*s)) ++s;
        if (*s != '"') THROWDATAEXCEPTION(Format("invalid quoted string: ") % s);
        ++s;

        while (*s != '"')
        {
            if (*s == '\\')
            {
                ++s;
                switch (*s)
                {
                case '\\':
                case '\'':
                case '"':
                    x += *s;
                    break;

                case 'r':
                    x += L'\r';
                    break;

                case 'n':
                    x += L'\n';
                    break;

                case 'x':
                {
                    const size_t z = 2;
                    char k[2 + z];
                    k[0] = '$';
                    k[1 + z] = '\0';

                    LOOP(j, z)
                    {
                        ++s;
                        if (!*s) THROWDATAEXCEPTION(Format("invalid quoted string: ") % s);
                        k[j + 1] = *s;
                    }
                    x += stou(k);
                };
                    break;

                case 'u':
                {
                    const size_t z = 4;
                    char k[2 + z];
                    k[0] = '$';
                    k[1 + z] = '\0';

                    LOOP(j, z)
                    {
                        ++s;
                        if (!*s) THROWDATAEXCEPTION(Format("invalid quoted string: ") % s);
                        k[j + 1] = *s;
                    }
                    x += stou(k);
                };
                    break;

                case 'w':
                {
                    const size_t z = 8;
                    char k[2 + z];
                    k[0] = '$';
                    k[1 + z] = '\0';

                    LOOP(j, z)
                    {
                        ++s;
                        if (!*s) THROWDATAEXCEPTION(Format("invalid quoted string: ") % s);
                        k[j + 1] = *s;
                    }
                    x += stou(k);
                };
                    break;

                default:
                    THROWDATAEXCEPTION(Format("invalid quoted string: ") % s);

                }
            }
            else
                x += static_cast<wchar_t> (*s);

            ++s;
        }
        ++s;
    }

    return x;
}

std::wstring wunquote(const std::string & s)
{
    return wunquote(s.data());
}

std::wstring wtryunquote(const char * s)
{
    const size_t z = strlen(s);
    if (z == 0) return widen(s);
    if (s[0] != '"') return widen(s);
    if (s[z - 1] != '"') return widen(s);
    return wunquote(std::string(s)); //std::string for a bug in msvc
}

std::wstring wtryunquote(const std::string & s)
{
    const size_t z = s.size();
    if (z == 0) return widen(s);
    if (s[0] != '"') return widen(s);
    if (s[z - 1] != '"') return widen(s);
    return wunquote(s);
}

std::wstring widen(const std::string & s)
{
    std::wstring w;
    for (auto it : s)
        w += static_cast<wchar_t> (it);
    return w;
}

std::string forceascii(const std::wstring & s, char k)
{
    std::string a;
    for (auto it : s)
    {
        if (it < 0x100) a += it;
        else a += k;
    }

    return a;
}

std::string escape(const char * s)
{
    std::string a;
    while (*s)
    {
        switch (*s)
        {
        case ' ': a.append("\\ ");
            break;
        case '\\': a.append("\\\\");
            break;
        case '\'': a.append("\\\'");
            break;
        case '\"': a.append("\\\"");
            break;
        case '\n': a.append("\\n");
            break;
        case '\r': a.append("\\r");
            break;

        default:
            if (static_cast<signed char> (*s) < 32)
            {
                char tmp[] = "\\xHH";
                CXU_ASSERT(sizeof (tmp) == 5);
                utobase(unsign(*s), tmp + 2, 16, 2);
                a += tmp;
            }
            else
                a += *s;
        }
        ++s;
    }
    return a;
}

std::string escape(const std::string & s)
{
    return escape(s.data());
}

std::string xsprintf(const char * format, ...)
{
    char * p = nullptr;
    int x = -1;

    va_list para;
    va_start(para, format);
    x = vasprintf(&p, format, para);
    va_end(para);

    if (x < 0)
        THROWTEXTEXCEPTION("vasprintf failed");

    std::string s(p);
    free(p);
    return s;
}

CXU_NS_END

//.
