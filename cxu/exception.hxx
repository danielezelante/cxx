// YAL zeldan


#ifndef CXU_EXCEPTION_HXX
#define CXU_EXCEPTION_HXX


#include <errno.h>
#include <vector>
#include <string>

#include "defs.hxx"
#include "logger.hxx"

CXU_NS_BEGIN


//! Generic exception
class Exception : public Stringable
{
public:

    typedef std::vector<std::string> t_stack;

    //! Default constructor for DEBUG

    Exception(const char * file, int line, const t_stack & st)
    : _file(file), _line(line), _stack(st)
    {
    }
    //! Default constructor

    Exception() : _line(-1)
    {
    }

    //! Descriptive full text
    std::string str() const override;
    void log(Logger::t_level) const;

    void printStackTrace(FILE*, const char * prefix = "") const;


protected:
    //! Descriptive text generator
    virtual std::string vtext() const = 0;



private:
    std::string _file;
    int _line;
    t_stack _stack;


};



//! Exception encapsulating \c errno

class ErrException : public Exception
{
public:

    //! Constructor for DEBUG

    ErrException(int e, const char * file, int line, const t_stack & st)
    : Exception(file, line, st), _err(e)
    {
    }

    ErrException(int e, const char * info, const char * file, int line, const t_stack & st)
    : Exception(file, line, st), _err(e), _info(info)
    {
    }

    ErrException(int e, const std::string & info, const char * file, int line, const t_stack & st)
    : Exception(file, line, st), _err(e), _info(info)
    {
    }
    //! Constructor

    explicit ErrException(int e) : _err(e)
    {
    }

    ErrException(int e, const char * info) : _err(e), _info(info)
    {
    }

    ErrException(int e, const std::string & info) : _err(e), _info(info)
    {
    }


    //! Get errno

    int err() const
    {
        return _err;
    }

    static std::string s_text(int);

protected:
    virtual std::string vtext() const override;

private:
    int _err;
    std::string _info;

    static Sync s_os;

};



//! TextException

class TextException : public Exception
{
public:

    //! Constructor for DEBUG

    TextException(const std::string & t, const char * file, int line, const t_stack & st)
    : Exception(file, line, st), _text(t)
    {
    }
    //! Constructor

    explicit TextException(const std::string & t) : _text(t)
    {
    }



protected:
    virtual std::string vtext() const override;

private:
    std::string _text;

};



// thrown when bad parameters are used

class InterfaceException : public TextException
{
public:
    InterfaceException(const char * function, const std::string & parameter);
    InterfaceException(const char * function, const std::string & parameter, const char * file, int line, const t_stack & st);

};



// thrown when bad data or protocol is detected

class DataException : public TextException
{
public:

    explicit DataException(const std::string & data);
    DataException(const std::string & data, const char * file, int line, const t_stack & st);

    const std::string getData() const
    {
        return _data;
    }


protected:

    const std::string _data;
};


// thrown when OS behaves unexpectedly

class OSException : public TextException
{
public:
    explicit OSException(const std::string & data);
    OSException(const std::string & data, const char * file, int line, const t_stack & st);

};





CXU_NS_END

#endif

//.
