// YAL zeldan

#ifndef CXU_INIT_HXX_
#define CXU_INIT_HXX_

#include <string>

#include "object.hxx"

CXU_NS_BEGIN

class Init : Entity
{
public:
    explicit Init(const char * progname);

    virtual ~Init();

    static Init * s()
    {
        return s_s;
    }

    Logger & logger();
    MainThread & mainthread();


protected:

    std::string _progname;
    Logger * _logger;
    MainThread * _mainthread;

private:
    static Init * s_s;

};


int cxumain(int argc, char ** argv, int (*func)(int, char**));



CXU_NS_END

#endif /*INIT_HXX_*/

//.
