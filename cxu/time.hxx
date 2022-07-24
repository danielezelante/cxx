// YAL zeldan


#ifndef CXU_TIME_HXX_
#define CXU_TIME_HXX_

#include <math.h>
#include <float.h>
#include <sys/time.h>

#include "defs.hxx"
#include "types.hxx"
#include "algebra.hxx"
#include "globals.hxx"
#include "integer.hxx"
#include "object.hxx"
#include "exception.hxx"
#include "bstorage.hxx"
#include "tstorage.hxx"


CXU_NS_BEGIN

//using namespace algebra;

class DeltaTime;


//! Time interval

class DeltaTime
: public Stringable
, public Serialization<DeltaTime>
, public Abel<DeltaTime>
, public TotalOrder<DeltaTime>
, public Set<DeltaTime, true>
{
    friend class Time;

public:

    DeltaTime(const DeltaTime &) = default;
    DeltaTime & operator=(const DeltaTime &) = default;

    static const DeltaTime s_null;

    //! zero length time interval
    DeltaTime(); // 0 delta time

    // algebra interface

    static const DeltaTime & sdo_Abel_O()
    {
        return s_null;
    }
    static DeltaTime sdo_Abel_sum(const DeltaTime &, const DeltaTime &);
    static DeltaTime sdo_Abel_opposite(const DeltaTime &);
    static int sdo_TotalOrder_compare(const DeltaTime &, const DeltaTime &);


    static DeltaTime ns(int64_t);
    static DeltaTime us(int64_t x);
    static DeltaTime ms(int64_t x);
    static DeltaTime s(int64_t x);
    static DeltaTime m(int64_t x);
    static DeltaTime h(int64_t x);
    static DeltaTime d(int64_t x);

    operator const struct timespec & () const
    {
        return _d;
    }

    operator struct timespec & ()
    {
        return _d;
    }

    const timespec * timespec_cptr() const
    {
        return &_d;
    }

    // tot : total

    //! interval in nanoseconds, rounded down
    int64_t tot_ns() const;
    int64_t tot_us() const;
    int64_t tot_ms() const;
    int64_t tot_s() const;
    int64_t tot_m() const;
    int64_t tot_h() const;
    int64_t tot_d() const;

    // totq : total rounded

    //! interval in nanoseconds, rounded at nearest
    int64_t totq_ns() const;
    int64_t totq_us() const;
    int64_t totq_ms() const;
    int64_t totq_s() const;
    int64_t totq_m() const;
    int64_t totq_h() const;
    int64_t totq_d() const;


    // par : partial
    int64_t par_ns() const;
    int64_t par_us() const;
    int64_t par_ms() const;
    int64_t par_s() const;
    int64_t par_m() const;
    int64_t par_h() const;
    int64_t par_d() const;

    int64_t parq_ns() const;
    int64_t parq_us() const;
    int64_t parq_ms() const;
    int64_t parq_s() const;
    int64_t parq_m() const;
    int64_t parq_h() const;
    int64_t parq_d() const;


    void _Serialization_serialize(BOStorage &) const;
    void _Serialization_serialize(BIStorage &);
    void _Serialization_serialize(TOStorage &) const;
    void _Serialization_serialize(TIStorage &);

    std::string str() const override;

    std::string str_d() const;
    std::string str_h() const;
    std::string str_m() const;
    std::string str_s() const;
    std::string str_ms() const;
    std::string str_us() const;
    std::string str_ns() const;

    int sgn() const;

protected:

    explicit DeltaTime(const struct timespec &);
    void normalize();


private:

    struct timespec _d;

};

class Time :
public Stringable,
public Serialization<Time>,
public Operabel<Time, DeltaTime>,
public TotalOrder<Time>,
public Set<Time, true>
{
    //CXU_DECLARE_SERIALIZABLE(Time);

    friend class DeltaTime;

public:


    //! construct with null value
    /*! construct wth null value, i.e. time_t==0 or tv_sec==0 and tv_nsec==0 */
    Time();

    Time(const struct timeval &);
    Time(const struct timespec &);
    Time(time_t);

    static const Time s_null;
    static const Time & O;

    operator const struct timeval() const;

    operator const struct timespec & () const
    {
        return _t;
    }

    operator struct timespec & ()
    {
        return _t;
    }

    std::string str() const override;
    void parse(const char *);

    //! non-null check
    /*! return true if it has a non-null value */
    bool is() const;

    //! return current Time
    /*! return current Time */
    static Time now();

    static Time jtime(uint64_t);

    Time(
         unsigned int year,
         unsigned int month,
         unsigned int day,
         unsigned int hour = 0,
         unsigned int minute = 0,
         unsigned int second = 0,
         unsigned int nano = 0
         );


    void _Serialization_serialize(BOStorage &) const;
    void _Serialization_serialize(BIStorage &);
    void _Serialization_serialize(TOStorage &) const;
    void _Serialization_serialize(TIStorage &);

    std::string format(const std::string & pattern, bool utc = false) const; //as in strftime

    time_t as_time_t() const;

    uint64_t as_jtime() const;

    void to_localtime(struct tm &) const;
    static Time from_localtime(const struct tm &);


    // algebra interface
    static Time sdo_Point_apply(const Time &, const DeltaTime &);
    static int sdo_TotalOrder_compare(const Time &, const Time &);
    static DeltaTime sdo_Point_unapply(const Time &, const Time &);

protected:


    void normalize();


private:

    struct timespec _t;

};




CXU_NS_END


#endif /*TIME_HXX_*/

//.
