// YAL zeldan

#ifndef CXU_LOGGER_HXX_
#define CXU_LOGGER_HXX_

#include <map>
#include <stdio.h>


#include "object.hxx"
#include "init.hxx"
#include "sync.hxx"

CXU_NS_BEGIN

class Logger : Entity
{
    friend class Init;

private:
    explicit Logger(const char * progname);
    virtual ~Logger();

public:

    enum t_level
    {
        e_nil = 0,
        e_error = 1,
        e_warning = 2,
        e_info = 3,
        e_debug = 4
    };

    static bool parselevel(int, t_level &); // return true if ok
    static bool parselevel(const char *, t_level &); // return true if ok

    void log(const char *, t_level);
    void log(const std::string &, t_level);

    void error(const char *);
    void error(const std::string &);

    void warning(const char *);
    void warning(const std::string &);

    void info(const char *);
    void info(const std::string &);

    void debug(const char *);
    void debug(const std::string &);

    void fatal(const char *);

    // non uso una TextOutput per potenziale ricorsione infinita di sue derivate
    bool addout(FILE * f, t_level uptoinc);
    bool delout(FILE * f);
    bool deldefault();

    FILE * getdefault() const
    {
        return _default;
    }

    static std::string timestamp();

    void usesyslog(t_level);


protected:

    static const char * s_levelstr(t_level);

    void printout(const char * message, t_level);

    typedef std::map<FILE*, t_level> t_map;
    t_map _out;

    FILE * _default;

    t_level _sysloglevel;
    const std::string _progname;
};

std::string xstrerror(int e);


CXU_NS_END

#define CXU_LOGGER (CXU_NS::Init::s()->logger())


#endif /*LOGGER_HXX_*/

//.
