// YAL zeldan

#include <stdio.h>

#include "exception.hxx"

#include "string.hxx"
#include "globals.hxx"
#include "mystd.hxx"
#include "logger.hxx"
#include "mymacros.hxx"

CXU_NS_BEGIN

//--------------------- Exception


std::string Exception::str() const
{
    if (_line >= 0)
        return Format("%$ : %$(%$)") % vtext() % _file % itod(_line);
    else
        return vtext();
}

void Exception::log(Logger::t_level w) const
{
    LOGGER.log(str().data(), w);
}

void Exception::printStackTrace(FILE * file, const char * prefix) const
{
    if (_stack.empty())
        fprintf(file, "%s%s\n", prefix, "(no stack trace available)");
    else
        for (auto const & it : _stack)
            fprintf(file, "%s%s\n", prefix, it.data());
}



//---------------------- ErrException


Sync ErrException::s_os;

std::string ErrException::s_text(int e)
{
    Lock lock(s_os);
    return xstrerror(e);
}

std::string ErrException::vtext() const
{
    return Format("ErrException : %$ (%$) : %$") % s_text(_err) % itod(_err) % _info;
}


//** TextException

std::string TextException::vtext() const
{
    return _text;
}



//** DataException

DataException::DataException(const std::string & parameter)
: TextException(Format("DataException(%$)") % parameter)
, _data(parameter)
{
}

DataException::DataException(const std::string & parameter, const char * file, int line, const t_stack & st)
: TextException(Format("DataException(%$)") % parameter, file, line, st)
, _data(parameter)
{
}

//** OSException

OSException::OSException(const std::string & data)
: TextException(Format("OSException(%$)") % data)
{
}

OSException::OSException(const std::string & data, const char * file, int line, const t_stack & st)
: TextException(Format("OSException(%$)") % data, file, line, st)
{
}

//** InterfaceException

InterfaceException::InterfaceException(const char * function, const std::string & parameter)
: TextException(Format("%$(%$)") % function % parameter)
{
}

InterfaceException::InterfaceException(const char * function, const std::string & parameter, const char * file, int line, const t_stack & st)
: TextException(Format("%$(%$)") % function % parameter, file, line, st)
{
}




CXU_NS_END

//.
