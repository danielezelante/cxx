// YAL zeldan

#ifndef CXU_ZSTRING_HXX
#define CXU_ZSTRING_HXX

#include <string.h>
#include <string>
#include <vector>
#include <utility>
#include <functional>

#include <limits.h>


#include "defs.hxx"
#include "globals.hxx"
#include "exception.hxx"
#include "integer.hxx"
#include "object.hxx"
#include "algebra.hxx"


CXU_NS_BEGIN

char val2sym(int);
int sym2val(char);

template <typename T> size_t utobase(T n, char * str, unsigned int base, size_t minwidth)
{
    CXU_ASSERT(str != 0);
    CXU_ASSERT(inoc(base, 1u, 36u));

    char tmp[sizeof (n) * CHAR_BIT + 1]; //1 for null terminator
    char * p = tmp;

    T m = n;
    for (; m; ++p)
    {
        unsigned int module = static_cast<unsigned int> (m % base);
        m /= base;
        *p = val2sym(module);
    }

    char * pe = str + minwidth - (p - tmp);
    char * s = str;
    for (; s < pe; ++s) *s = '0';
    for (; p > tmp; ++s, --p) *s = *(p - 1);

    if (s == str)
    {
        *s = '0';
        ++s;
    }

    *s = 0;
    return s - str;
}

template <typename T> size_t utod(T n, char * str)
{
    return utobase(unsign(n), str, 10, 1);
}

template <typename T> size_t utob(T n, char * str, char prefix)
{
    if (prefix) *str = prefix;
    return utobase(unsign(n), str + (prefix ? 1 : 0), 2, sizeof (n) * CHAR_BIT);
}

template <typename T> size_t utox(T n, char * str, char prefix)
{
    if (prefix) *str = prefix;
    return utobase(unsign(n), str + (prefix ? 1 : 0), 16, sizeof (n) * CHAR_BIT / 4);
}

template <typename T> size_t utoo(T n, char * str, char prefix)
{
    if (prefix) *str = prefix;
    return utobase(unsign(n), str + (prefix ? 1 : 0), 8, sizeof (n)*3);
}

template <typename T> size_t itod(T n, char * s, bool plus = false)
{
    if (n > 0)
    {
        if (plus)
            *(s++) = '+';
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

template <typename T> std::string utod(T n)
{
    char tmp[sizeof (n)*3 + 1];
    utod(n, tmp);
    return std::string(tmp);
}

template <typename T> std::string utob(T n, char prefix = '%')
{
    char tmp[sizeof (n)*8 + 2];
    utob(n, tmp, prefix);
    return std::string(tmp);
}

template <typename T> std::string utox(T n, char prefix = '$')
{
    char tmp[sizeof (n)*2 + 2]; // +2 for '$' '\0'
    utox(n, tmp, prefix);
    return std::string(tmp);
}

template <typename T> std::string utoo(T n, char prefix = '^')
{
    char tmp[sizeof (n)*3 + 2];
    utoo(n, tmp, prefix);
    return std::string(tmp);
}

template <typename T> std::string itod(T n)
{
    char tmp[sizeof (n)*3 + 2];
    itod(n, tmp);
    return std::string(tmp);
}
#define CXU_itod_L(n) CXU_LAZYSTR1(itod, n)

class StringableConst : public Stringable
{
public:
    virtual ~StringableConst();

    explicit StringableConst(const std::string & s)
    : _str(s)
    {
    }

    explicit StringableConst(const char * s)
    : _str(s)
    {
    }

    virtual std::string str() const override;

protected:
    std::string _str;
};


unsigned int stou(const char * str, char ** endptr);
unsigned int stou(const char * str);
unsigned int stou(const std::string &);

unsigned long stoul(const char * str, char ** endptr);
unsigned long stoul(const char * str);
unsigned long stoul(const std::string &);


std::string ftod(double);
std::string ftox(double);

std::string realeng(double x, unsigned int ddigits, bool plus = false);
std::string realfix(double x, unsigned int ddigits);

double stof(const char *);
double stof(const std::string &);

std::string ptrtoa(const void *, const char * prefix = "$");
std::string ptrtoas(const void *, const char * prefix = "$");







std::string trim(const std::string &, const char * pattern = " \t\r\n");
std::string rtrim(const std::string &, const char * pattern = " \t\r\n");

void strupr(char *);
void strlwr(char *);
void makeupper(std::string &);
void makelower(std::string &);
std::string toupper(const std::string &);
std::string tolower(const std::string &);

template <typename K> std::basic_string<K> rjus(const std::basic_string<K> & a, typename std::basic_string<K>::size_type n, K c = static_cast<K> (' '))
{
    size_t w = a.size();
    if (w >= n) return a;
    return std::basic_string<K>(n - w, c) + a;
}

template <typename K> std::basic_string<K> ljus(const std::basic_string<K> & a, typename std::basic_string<K>::size_type n, K c = static_cast<K> (' '))
{
    std::string::size_type w = a.size();
    if (w >= n) return a;
    return a + std::basic_string<K>(n - w, c);
}


std::string quote(char);

std::string quote(const char *);
std::string quote(const std::string &);

std::string quote(const wchar_t *);
std::string quote(const std::wstring &);


#if 0

template <typename K> std::basic_string<K> format(const K * frm, const std::vector<std::basic_string<K> > & dta)
{
    std::basic_string<K> str;

    size_t pos = 0;

    while (*frm)
    {
        if (*frm != static_cast<K> ('%'))
        {
            str += (*frm);
            frm += 1;
            continue;
        };

        if (*(frm + 1) == static_cast<K> ('$'))
        {
            CXU_ASSERT(pos < dta.size());
            str += (dta[pos++]);
            frm += 2;
            continue;
        };

        if (*(frm + 1) == static_cast<K> ('%'))
        {
            str += static_cast<K> ('%');
            frm += 2;
            continue;
        };

        CXU_THROWINTERFACEEXCEPTION(quote(frm));

    };

    if (pos != dta.size())
    {
        char myerr[256];
        const std::string qfrm = cxu::quote(qfrm);
        sprintf(
                myerr,
                "in %s: tag_count = %zd, data_size = %zu",
                qfrm.c_str(),
                pos,
                dta.size()
                );
        CXU_THROWINTERFACEEXCEPTION(myerr);
    }

    CXU_ASSERT(pos == dta.size());

    return str;
}

#endif

std::string quoteifneeded(const char *);
std::string quoteifneeded(const std::string &);

std::string unquote(const char *);
std::string unquote(const std::string &);

std::string tryunquote(const char *);
std::string tryunquote(const std::string &);


std::string escape(const char *);
std::string escape(const std::string &);



std::vector<std::string> parse(
                               const std::string & str,
                               bool usespc,
                               const char * tok,
                               bool toka,
                               bool sdla,
                               const char * com
                               );

template <class T> class XFormat : public XStringable<T>
{
protected:
    std::basic_string<T> _form;

    enum t_data
    {
        e_str, e_ref
    };

    std::vector<t_data> _type;

    std::vector<std::basic_string<T> > _strdata;

    // this needs virtual system and not CRTP
    std::vector<std::reference_wrapper<XStringable<T> const> > _refdata;

    void push_str(const T * s)
    {
        _type.push_back(e_str);
        _strdata.push_back(s);
    }

    void push_str(const std::basic_string<T> & s)
    {
        _type.push_back(e_str);
        _strdata.push_back(s);
    }

    template <class A> void push_str(const XStringation<A, T> & s)
    {
        _type.push_back(e_str);
        _strdata.push_back(s.str());
    }

    void push_ref(const XStringable<T> & r)
    {
        _type.push_back(e_ref);
        _refdata.push_back(std::cref(r));
    }


public:

    explicit XFormat(const T * s) : _form(s)
    {
    }

    explicit XFormat(const std::basic_string<T> & s) : _form(s)
    {
    }

    XFormat<T> & operator%(const T * s)
    {
        push_str(s);
        return *this;
    }

    XFormat<T> & operator%(const std::basic_string<T> & s)
    {
        push_str(s);
        return *this;
    }

    XFormat<T> & operator%(const XStringable<T> & r)
    {
        push_ref(r);
        return *this;
    }

    template <class A> XFormat<T> & operator%(const XStringation<A, T> & s)
    {
        push_str(s);
        return *this;
    }

    XFormat<T> & operator%(int x)
    {
        push_str(itod(x));
        return *this;
    }

    XFormat<T> & operator%(unsigned int x)
    {
        push_str(utod(x));
        return *this;
    }

    XFormat<T> & operator%(long x)
    {
        push_str(itod(x));
        return *this;
    }

    XFormat<T> & operator%(unsigned long x)
    {
        push_str(utod(x));
        return *this;
    }

#ifndef _LP64

    XFormat<T> & operator%(int64_t x)
    {
        push_str(itod(x));
        return *this;
    }

    XFormat<T> & operator%(uint64_t x)
    {
        push_str(utod(x));
        return *this;
    }

#endif

#if __CYGWIN__

    XFormat<T> & operator%(long long int x)
    {
        push_str(itod(x));
        return *this;
    }

    XFormat<T> & operator%(unsigned long long int x)
    {
        push_str(utod(x));
        return *this;
    }

#endif

    XFormat<T> & operator%(double x)
    {
        push_str(ftod(x));
        return *this;
    }

    std::basic_string<T> str() const override
    {
        size_t strndx = 0;
        size_t refndx = 0;

        std::basic_string<T> q;

        size_t pos = 0;

        const T * frm = _form.c_str();
        while (*frm)
        {
            if (*frm != static_cast<T> ('%'))
            {
                q += (*frm);
                frm += 1;
                continue;
            };

            if (*(frm + 1) == static_cast<T> ('$'))
            {
                CXU_ASSERT(pos < _type.size());
                switch (_type[pos++])
                {
                case e_str:
                    CXU_ASSERT(strndx < _strdata.size());
                    q += _strdata[strndx++];
                    break;
                case e_ref:
                    CXU_ASSERT(refndx < _refdata.size());
                    q += _refdata[refndx++].get(); // here lazy evaluation wakes
                    break;
                default: CXU_ASSERT(false);
                }
                frm += 2;
                continue;
            };

            if (*(frm + 1) == static_cast<T> ('%'))
            {
                q += static_cast<T> ('%');
                frm += 2;
                continue;
            };

            CXU_THROWINTERFACEEXCEPTION(quote(_form));

        };

        if (pos != _type.size())
        {
            char myerr[256];
            const std::string qfrm = cxu::quote(_form);
            snprintf(
                     myerr, CXU_DIM(myerr),
                     "in %s: tag_count = %zu, data_size = %zu",
                     qfrm.c_str(),
                     pos,
                     _type.size()
                     );
            CXU_THROWINTERFACEEXCEPTION(myerr);
        }

        return q;
    }
};


typedef XFormat<char> Format;
typedef XFormat<wchar_t> WFormat;


// parse using blanks and consider "" but do not unescape
std::vector<std::string> dqparse(const std::string & str, char comment);

template <class T> std::string spacecompose(T x0, T x1)
{
    std::string x;
    for (auto it = x0; it != x1; ++it)
    {
        x.append(quoteifneeded(*it));
        x.append(" ");
    }
    return x;
}

template <class T> std::vector<std::basic_string<T> >
patternparse(const std::basic_string<T> & s, const T * p)
{
    std::vector<std::basic_string<T> > v;


    auto it = s.begin();
    while (*p)
    {
        char k = *p;

        if (k == '%')
        {
            switch (*(p + 1))
            {
            case '$': k = '\0';
                ++p;
                break;
            case '%': ++p;
                break;
            default: CXU_THROWDATAEXCEPTION(
                                            Format("invalid pattern: %$") % quote(p)
                                            );
                break;
            }
        }
        ++p;


        if (k == ' ') while (*p == ' ') ++p; // consecutive blanks in pattern are shrinked

        if (k)
        {
            //CXU_LOGGER.debug(Format("k=%$ ; *it=%$") %std::string(1,k) %std::string(1,*it));
            if (k == ' ')
            {
                while (*it == ' ') ++it;
            }
            else
            {
                if (*it != k) break;
                ++it;
            }
        }
        else
        {
            std::basic_string<T> b;
            while (it != s.end())
            {
                //CXU_LOGGER.debug(Format("k=0 ; *it=%$") %std::string(1,*it));
                if (*it != *p) b.push_back(*it), ++it;
                else break;
            }
            if (!b.empty() || (it != s.end())) v.push_back(b);
        }


    }

    return v;
}



unsigned int stou(const wchar_t * str);
unsigned int stou(const std::wstring &);

#if 0
int stoi(const wchar_t * str);
int stoi(const std::wstring &);
#endif

double stof(const wchar_t *);
double stof(const std::wstring &);

void strupr(char *);
void strlwr(char *);
void makeupper(std::string &);
void makelower(std::string &);
std::string toupper(const std::string &);
std::string tolower(const std::string &);

void strupr(wchar_t *);
void strlwr(wchar_t *);
void makeupper(std::wstring &);
void makelower(std::wstring &);
std::wstring toupper(const std::wstring &);
std::wstring tolower(const std::wstring &);

std::wstring widen(const std::string &);

inline std::wstring widen(const std::wstring & s)
{
    return s;
}

std::string forceascii(const std::wstring &, char k);

inline std::string forceascii(const std::string & s, char)
{
    return s;
}

std::wstring wunquote(const std::string &);
std::wstring wunquote(const char * &);

std::wstring trywunquote(const std::string &);
std::wstring trywunquote(const char * &);

#if 0

class WFormat : public WStringable
{
protected:
    std::wstring _form;
    std::vector<std::wstring> _data;

public:
    WFormat(const char *);
    WFormat(const wchar_t *);
    WFormat(const std::string &);
    WFormat(const std::wstring &);

    WFormat & operator%(const char *);
    WFormat & operator%(const wchar_t *);
    WFormat & operator%(const std::string &);
    WFormat & operator%(const std::wstring &);

    WFormat & operator%(const Stringable &);
    WFormat & operator%(int);
    WFormat & operator%(unsigned int);
    WFormat & operator%(long);
    WFormat & operator%(unsigned long);
    WFormat & operator%(double);

    std::wstring str() const;

#ifndef _LP64
    WFormat & operator%(int64_t);
    WFormat & operator%(uint64_t);
#endif

};


#endif



uint64_t stou64(const char * str, char ** endptr);
uint64_t stou64(const char * str);
uint64_t stou64(const std::string &);

int64_t stoi64(const char * str, char ** endptr);
int64_t stoi64(const char * str);
int64_t stoi64(const std::string &);



std::string threadidtoa(pthread_t);

template <class T> bool beginsWith(
                                   const std::basic_string<T> & s,
                                   const T * beg)
{
    for (auto it = s.begin(); it != s.end() && *beg && *it == *beg; ++it, ++beg);
    return !*beg;
}

template <class T> bool beginsWith(
                                   const std::basic_string<T> & s,
                                   const std::basic_string<T> & b)
{
    auto ib = b.begin();
    for
        (
            auto it = s.begin();
            it != s.end() && ib != b.end() && *it == *ib;
            ++it, ++ib
            );
    return ib == b.end();
}


std::string xsprintf(const char * format, ...) __attribute__ ((format(printf, 1, 2)));



CXU_NS_END

#endif

//.
