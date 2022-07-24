// YAL zeldan


#include <sys/time.h>
#include <math.h>

#include <string>
#include <time.h>

using namespace std;

#include "time.hxx"

#include "types.hxx"
#include "defs.hxx"
#include "integer.hxx"
#include "exception.hxx"
#include "string.hxx"

#include "mymacros.hxx"


CXU_NS_BEGIN


        static const int64_t s_giga = 1000000000;
static const int64_t s_mega = 1000000;
static const int64_t s_kilo = 1000;



/*********************************************************/
// Time


const Time Time::s_null = Time();
const Time & Time::O = Time::s_null;

Time::Time()
{
    _t.tv_sec = 0;
    _t.tv_nsec = 0;
}

// attenzione, questo costruisce in base alla ora locale

Time::Time(
           unsigned int year,
           unsigned int month,
           unsigned int day,
           unsigned int hour,
           unsigned int minute,
           unsigned int second,
           unsigned int nano
           )
{
    struct tm x;
    ZERO(x);
    x.tm_year = year - 1900;
    x.tm_mon = month - 1;
    x.tm_mday = day;
    x.tm_hour = hour;
    x.tm_min = minute;
    x.tm_sec = second;
    x.tm_isdst = -1; //autodetect

    const time_t w = ::mktime(&x);
    if (w == static_cast<time_t> (-1)) THROWINTERFACEEXCEPTION("invalid time fields");
    _t.tv_sec = w;
    if (nano >= s_giga) THROWINTERFACEEXCEPTION("invalid time nano field");
    _t.tv_nsec = nano;
}

std::string Time::str() const
{
    if (!is()) return "O";
    struct tm x;
    ::gmtime_r(&_t.tv_sec, &x);
    const size_t buflen = 256;
    char buf[buflen];
    ::strftime(buf, buflen, "%FT%T", &x);
    return Format("%$.%$UTC") % buf % rjus(utod(_t.tv_nsec / s_mega), 3, '0');
}

Time::Time(const struct timespec & t) : _t(t)
{
    normalize();
}

Time::Time(const struct timeval & t)
{
    _t.tv_sec = t.tv_sec;
    _t.tv_nsec = t.tv_usec * s_kilo;
    normalize();
}

Time::Time(const time_t x)
{
    _t.tv_sec = x;
    _t.tv_nsec = 0;
    //normalize(); // non serve qui dato che tv_nesc==0
}

void Time::normalize()
{
    if (_t.tv_nsec < 0)
    {
        _t.tv_nsec += s_giga;
        --_t.tv_sec;
    }

    _t.tv_sec += _t.tv_nsec / s_giga;
    _t.tv_nsec %= s_giga;
}

Time Time::now()
{
    struct timeval tv;
    ERRCALLS(::gettimeofday(&tv, 0), "gettimeofday");
    return Time(tv);
}

Time Time::jtime(uint64_t x)
{
    struct timespec tv;
    tv.tv_sec = x / s_kilo;
    tv.tv_nsec = (x % s_kilo) * s_mega;
    return Time(tv);
}

int Time::sdo_TotalOrder_compare(const Time & a, const Time & b)
{
    if (a._t.tv_sec < b._t.tv_sec) return -1;
    if (a._t.tv_sec > b._t.tv_sec) return +1;

    if (a._t.tv_nsec < b._t.tv_nsec) return -1;
    if (a._t.tv_nsec > b._t.tv_nsec) return +1;

    return 0;
}

Time Time::sdo_Point_apply(const Time & p, const DeltaTime & v)
{
    struct timespec t = p._t;
    t.tv_sec += v._d.tv_sec;
    t.tv_nsec += v._d.tv_nsec;
    return Time(t);
}

DeltaTime Time::sdo_Point_unapply(const Time & t0, const Time & t1)
{
    struct timespec ts;
    ts.tv_sec = t1._t.tv_sec - t0._t.tv_sec;
    ts.tv_nsec = t1._t.tv_nsec - t0._t.tv_nsec;
    return DeltaTime(ts);
}

bool Time::is() const
{
    return *this != Time::O;
}

void Time::_Serialization_serialize(BOStorage & s) const
{
    const struct timespec & x = *this;
    s << x.tv_sec;
    s << x.tv_nsec;
}

void Time::_Serialization_serialize(BIStorage & s)
{
    struct timespec & x = *this;
    s >> x.tv_sec;
    s >> x.tv_nsec;
}

void Time::_Serialization_serialize(TOStorage & s) const
{
    std::string a = Format("(%$)") % str();
    s << a.data();
}

void Time::_Serialization_serialize(TIStorage & s)
{
    const std::string pattern = Time().str();
    const size_t patlen = pattern.size();
    char buf[patlen];

    s >> "(";
    LOOP(j, patlen)
    buf[j] = s.read();
    s >> ")";

    parse(buf);
}

void Time::parse(const char * s)
{
    if (!strcmp(s, "O"))
    {
        *this = Time();
        return;
    }

    if (strlen(s) < 26) THROWDATAEXCEPTION(s);

    if (
            s[ 4] != '-' ||
            s[ 7] != '-' ||
            s[10] != 'T' ||
            s[13] != ':' ||
            s[16] != ':' ||
            s[19] != '.' ||
            s[23] != 'U' ||
            s[24] != 'T' ||
            s[25] != 'C'
            )
        THROWDATAEXCEPTION(s);

    const unsigned int year = ::strtoul(s + 0, 0, 10);
    const unsigned int month = ::strtoul(s + 5, 0, 10);
    const unsigned int day = ::strtoul(s + 8, 0, 10);
    const unsigned int hour = ::strtoul(s + 11, 0, 10);
    const unsigned int minute = ::strtoul(s + 14, 0, 10);
    const unsigned int second = ::strtoul(s + 17, 0, 10);
    const unsigned int millis = ::strtoul(s + 20, 0, 10);

    struct tm ttt;
    memset(&ttt, 0, sizeof (ttt));
    ttt.tm_sec = second;
    ttt.tm_min = minute;
    ttt.tm_hour = hour;
    ttt.tm_mday = day;
    ttt.tm_mon = month;
    ttt.tm_year = year;

    _t.tv_sec = mktime(&ttt);
    _t.tv_nsec = millis * s_mega;

}

time_t Time::as_time_t() const
{
    return _t.tv_sec;
}

uint64_t Time::as_jtime() const
{
    return static_cast<uint64_t> (_t.tv_sec) * s_kilo + _t.tv_nsec / s_mega;
}

std::string Time::format(const std::string & pattern, bool utc) const
{
    if (!is()) return std::string();
    const std::string p = std::string("#") + pattern; // to avoid zero length output

    const time_t tt = as_time_t();
    struct tm tmx;

    if (utc) ::gmtime_r(&tt, &tmx);
    else ::localtime_r(&tt, &tmx);

    std::string ooo;
    size_t z = p.size()*2 + 256; // stima iniziale non vincolante

    for (;;)
    {
        char buf[z];
        const size_t k = ::strftime(buf, z, p.data(), &tmx);
        if (k)
        {
            ooo = buf + 1; // remove # at beginning
            break;
        }
        z *= 2;
    }

    return ooo;
}

void Time::to_localtime(struct tm & a) const
{
    const time_t t0 = as_time_t();
    if (!::localtime_r(&t0, &a))
        THROWOSEXCEPTION("locatime_r failed");
}

Time Time::from_localtime(const struct tm & a)
{
    struct tm x = a;
    return Time(mktime(&x));
}


/********************************************************/
// DeltaTime


const DeltaTime DeltaTime::s_null = DeltaTime();

DeltaTime::DeltaTime()
{
    _d.tv_sec = 0;
    _d.tv_nsec = 0;
}

DeltaTime::DeltaTime(const struct timespec & ts)
{
    _d.tv_sec = ts.tv_sec;
    _d.tv_nsec = ts.tv_nsec;
    normalize();
}

void DeltaTime::normalize()
{
    _d.tv_sec += _d.tv_nsec / s_giga;
    _d.tv_nsec %= s_giga;

    if (_d.tv_sec >= 0)
    {
        if (_d.tv_nsec < 0)
        {
            _d.tv_nsec += s_giga;
            --_d.tv_sec;
        }
    }
    else
    {
        if (_d.tv_nsec > 0)
        {
            _d.tv_nsec -= s_giga;
            ++_d.tv_sec;
        }
    }

}

DeltaTime DeltaTime::sdo_Abel_opposite(const DeltaTime & a)
{
    struct timespec ts;
    ts.tv_sec = -a._d.tv_sec;
    ts.tv_nsec = -a._d.tv_nsec;
    return DeltaTime(ts);
}

DeltaTime DeltaTime::sdo_Abel_sum(const DeltaTime & a, const DeltaTime & b)
{
    //TRACE(Format("DeltaTime::sdo_Abel_sum(%$,%$)") %a %b);
    const struct timespec ts ={
        .tv_sec = a._d.tv_sec + b._d.tv_sec,
        .tv_nsec = a._d.tv_nsec + b._d.tv_nsec,
    };
    //const DeltaTime x(ts);
    //TRACE(Format("DeltaTime::sdo_Abel_sum => %$") %x);
    return DeltaTime(ts);
}

DeltaTime DeltaTime::ns(int64_t x)
{
    //return DeltaTime(x / s_giga, (x % s_giga) * (s_giga/s_giga));
    // simplified :
    struct timespec ts;
    ts.tv_sec = x / s_giga;
    ts.tv_nsec = x % s_giga;
    return DeltaTime(ts);
}

DeltaTime DeltaTime::us(int64_t x)
{
    struct timespec ts;
    ts.tv_sec = x / s_mega;
    ts.tv_nsec = x % s_mega * (s_giga / s_mega);
    return DeltaTime(ts);
}

DeltaTime DeltaTime::ms(int64_t x)
{
    struct timespec ts;
    ts.tv_sec = x / s_kilo;
    ts.tv_nsec = x % s_kilo * (s_giga / s_kilo);
    return DeltaTime(ts);
}

DeltaTime DeltaTime::s(int64_t x)
{
    struct timespec ts;
    ts.tv_sec = x;
    ts.tv_nsec = 0;
    return DeltaTime(ts);
}

DeltaTime DeltaTime::h(int64_t x)
{
    struct timespec ts;
    ts.tv_sec = x * 3600;
    ts.tv_nsec = 0;
    return DeltaTime(ts);
}

DeltaTime DeltaTime::d(int64_t x)
{
    struct timespec ts;
    ts.tv_sec = x * 3600 * 24;
    ts.tv_nsec = 0;
    return DeltaTime(ts);
}

int64_t DeltaTime::tot_ns() const
{
    if (_d.tv_sec > std::numeric_limits<int64_t>::max() / s_giga) THROWDATAEXCEPTION("too large");
    if (_d.tv_sec < std::numeric_limits<int64_t>::min() / s_giga) THROWDATAEXCEPTION("too large");
    return _d.tv_sec * s_giga + _d.tv_nsec;
}

int64_t DeltaTime::tot_us() const
{
    if (_d.tv_sec > std::numeric_limits<int64_t>::max() / s_mega) THROWDATAEXCEPTION("too large");
    if (_d.tv_sec < std::numeric_limits<int64_t>::min() / s_mega) THROWDATAEXCEPTION("too large");
    return _d.tv_sec * s_mega + _d.tv_nsec / (s_giga / s_mega);
}

int64_t DeltaTime::tot_ms() const
{
    if (_d.tv_sec > std::numeric_limits<int64_t>::max() / s_kilo) THROWDATAEXCEPTION("too large");
    if (_d.tv_sec < std::numeric_limits<int64_t>::min() / s_kilo) THROWDATAEXCEPTION("too large");
    return _d.tv_sec * s_kilo + _d.tv_nsec / (s_giga / s_kilo);
}

int64_t DeltaTime::tot_s() const
{
    return _d.tv_sec;
}

int64_t DeltaTime::tot_m() const
{
    return tot_s() / 60;
}

int64_t DeltaTime::tot_h() const
{
    return tot_m() / 60;
}

int64_t DeltaTime::tot_d() const
{
    return tot_h() / 24;
}

int64_t DeltaTime::totq_ns() const
{
    return tot_ns();
}

int64_t DeltaTime::totq_us() const
{
    const int64_t subpart = par_ns();
    int rnd = 0;
    if (subpart >= 500) rnd = +1;
    if (subpart < -500) rnd = -1;
    return tot_us() + rnd;
}

int64_t DeltaTime::totq_ms() const
{
    const int64_t subpart = par_us();
    int rnd = 0;
    if (subpart >= 500) rnd = +1;
    if (subpart < -500) rnd = -1;
    return tot_ms() + rnd;
}

int64_t DeltaTime::totq_s() const
{
    const int64_t subpart = par_ms();
    int rnd = 0;
    if (subpart >= 500) rnd = +1;
    if (subpart < -500) rnd = -1;
    return tot_s() + rnd;
}

int64_t DeltaTime::totq_m() const
{
    const int64_t subpart = par_s();
    int rnd = 0;
    if (subpart >= 30) rnd = 1;
    if (subpart < -30) rnd = -1;
    return tot_m() + rnd;
}

int64_t DeltaTime::totq_h() const
{
    const int64_t subpart = par_m();
    int rnd = 0;
    if (subpart >= 30) rnd = 1;
    if (subpart < -30) rnd = -1;
    return tot_h() + rnd;
}

int64_t DeltaTime::totq_d() const
{
    const int64_t subpart = par_h();
    int rnd = 0;
    if (subpart >= 12) rnd = 1;
    if (subpart < -12) rnd = -1;
    return tot_d() + rnd;
}

int64_t DeltaTime::par_d() const
{
    return tot_d();
}

int64_t DeltaTime::par_h() const
{
    return (tot_ns() % (s_giga * 60 * 60 * 24)) / (s_giga * 60 * 60);
}

int64_t DeltaTime::par_m() const
{
    return (tot_ns() % (s_giga * 60 * 60)) / (s_giga * 60);
}

int64_t DeltaTime::par_s() const
{
    return (tot_ns() % (s_giga * 60)) / s_giga;
}

int64_t DeltaTime::par_ms() const
{
    return (tot_ns() % s_giga) / s_mega;
}

int64_t DeltaTime::par_us() const
{
    return (tot_ns() % s_mega) / s_kilo;
}

int64_t DeltaTime::par_ns() const
{
    return tot_ns() % s_kilo;
}

int64_t DeltaTime::parq_d() const
{
    const int64_t subpart = par_h();
    int rnd = 0;
    if (subpart >= 12) rnd = +1;
    if (subpart < -12) rnd = -1;
    return par_d() + rnd;
}

int64_t DeltaTime::parq_h() const
{
    const int64_t subpart = par_m();
    int rnd = 0;
    if (subpart >= 60) rnd = +1;
    if (subpart < -60) rnd = -1;
    return par_h() + rnd;
}

int64_t DeltaTime::parq_m() const
{
    const int64_t subpart = par_s();
    int rnd = 0;
    if (subpart >= 60) rnd = +1;
    if (subpart < -60) rnd = -1;
    return par_m() + rnd;
}

int64_t DeltaTime::parq_s() const
{
    const int64_t subpart = par_ms();
    int rnd = 0;
    if (subpart >= 500) rnd = +1;
    if (subpart < -500) rnd = -1;
    return par_s() + rnd;
}

int64_t DeltaTime::parq_ms() const
{
    const int64_t subpart = par_us();
    int rnd = 0;
    if (subpart >= 500) rnd = +1;
    if (subpart < -500) rnd = -1;
    return par_ms() + rnd;
}

int64_t DeltaTime::parq_us() const
{
    const int64_t subpart = par_ns();
    int rnd = 0;
    if (subpart >= 500) rnd = +1;
    if (subpart < -500) rnd = -1;
    return par_us() + rnd;
}

int64_t DeltaTime::parq_ns() const
{
    return par_ns();
}

int DeltaTime::sdo_TotalOrder_compare(const DeltaTime & x, const DeltaTime & y)
{
    if (x._d.tv_sec < y._d.tv_sec) return -1;
    if (x._d.tv_sec > y._d.tv_sec) return +1;
    if (x._d.tv_nsec < y._d.tv_nsec) return -1;
    if (x._d.tv_nsec > y._d.tv_nsec) return +1;
    return 0;
}

int DeltaTime::sgn() const
{
    return sdo_TotalOrder_compare(*this, DeltaTime::O());
}

std::string DeltaTime::str() const
{
    DeltaTime a;
    bool neg;

    if (*this >= DeltaTime::O()) a = *this, neg = false;
    else a = - * this, neg = true;

    std::string w;

    if (a.par_d())
        w = Format("%$d+%$h+%$m+%$s+%$ms+%$us+%$ns")
        % a.par_d() % a.par_h() % a.par_m() % a.par_s() % a.par_ms() % a.par_us() % a.par_ns();
    else if (a.par_h())
        w = Format("%$h+%$m+%$s+%$ms+%$us+%$ns")
        % a.par_h() % a.par_m() % a.par_s() % a.par_ms() % a.par_us() % a.par_ns();
    else if (a.par_m())
        w = Format("%$m+%$s+%$ms+%$us+%$ns")
        % a.par_m() % a.par_s() % a.par_ms() % a.par_us() % a.par_ns();
    else if (a.par_s())
        w = Format("%$s+%$ms+%$us+%$ns")
        % a.par_s() % a.par_ms() % a.par_us() % a.par_ns();
    else if (a.par_ms())
        w = Format("%$ms+%$us+%$ns")
        % a.par_ms() % a.par_us() % a.par_ns();
    else if (a.par_us())
        w = Format("%$us+%$ns")
        % a.par_us() % a.par_ns();
    else if (a.par_ns())
        w = Format("%$ns")
        % a.par_ns();
    else
        w = "0";

    if (neg) return Format("-(%$)") % w;
    else return w;
}

std::string DeltaTime::str_ns() const
{
    DeltaTime a;
    bool neg;

    if (*this >= DeltaTime::O()) a = *this, neg = false;
    else a = - * this, neg = true;

    const std::string w = Format("%$d + %$h + %$m + %$s + %$ms + %$us + %$ns")
            % a.par_d() % a.par_h() % a.par_m() % a.par_s()
            % a.par_ms() % a.par_us() % a.parq_ns()
            ;

    if (neg) return Format("-(%$)") % w;
    else return w;
}

std::string DeltaTime::str_us() const
{
    DeltaTime a;
    bool neg;

    if (*this >= DeltaTime::O()) a = *this, neg = false;
    else a = - * this, neg = true;

    const std::string w = Format("%$d + %$h + %$m + %$s + %$ms + %$us")
            % a.par_d() % a.par_h() % a.par_m() % a.par_s()
            % a.par_ms() % a.parq_us()
            ;

    if (neg) return Format("-(%$)") % w;
    else return w;
}

std::string DeltaTime::str_ms() const
{
    DeltaTime a;
    bool neg;

    if (*this >= DeltaTime::O()) a = *this, neg = false;
    else a = - * this, neg = true;

    const std::string w = Format("%$d + %$h + %$m + %$s + %$ms")
            % a.par_d() % a.par_h() % a.par_m() % a.par_s()
            % a.parq_ms()
            ;

    if (neg) return Format("-(%$)") % w;
    else return w;
}

std::string DeltaTime::str_s() const
{
    DeltaTime a;
    bool neg;

    if (*this >= DeltaTime::O()) a = *this, neg = false;
    else a = - * this, neg = true;

    const std::string w = Format("%$d + %$h + %$m + %$s")
            % a.par_d() % a.par_h() % a.par_m() % a.parq_s()
            ;

    if (neg) return Format("-(%$)") % w;
    else return w;
}

std::string DeltaTime::str_m() const
{
    DeltaTime a;
    bool neg;

    if (*this >= DeltaTime::O()) a = *this, neg = false;
    else a = - * this, neg = true;

    const std::string w = Format("%$d + %$h + %$m")
            % a.par_d() % a.par_h() % a.parq_m()
            ;

    if (neg) return Format("-(%$)") % w;
    else return w;
}

std::string DeltaTime::str_h() const
{
    DeltaTime a;
    bool neg;

    if (*this >= DeltaTime::O()) a = *this, neg = false;
    else a = - * this, neg = true;

    const std::string w = Format("%$d + %$h")
            % a.par_d() % a.parq_h()
            ;

    if (neg) return Format("-(%$)") % w;
    else return w;
}

std::string DeltaTime::str_d() const
{
    DeltaTime a;
    bool neg;

    if (*this >= DeltaTime::O()) a = *this, neg = false;
    else a = - * this, neg = true;

    const std::string w = Format("%$d")
            % a.parq_d()
            ;

    if (neg) return Format("-(%$)") % w;
    else return w;
}

void DeltaTime::_Serialization_serialize(BOStorage & z) const
{
    z << tot_ns();
}

void DeltaTime::_Serialization_serialize(BIStorage & z)
{
    int64_t k;
    z >> k;
    *this = DeltaTime::ns(k);
}

void DeltaTime::_Serialization_serialize(TOStorage & z) const
{
    z << "DeltaTime(" << tot_ns() << ")";
}

void DeltaTime::_Serialization_serialize(TIStorage & z)
{
    int64_t k;
    z >> "DeltaTime(" >> k >> ")";
    *this = DeltaTime::ns(k);
}




CXU_NS_END

//.
