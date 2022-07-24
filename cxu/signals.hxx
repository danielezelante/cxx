// YAL zeldan

#ifndef CXU_SIGNALS_HXX
#define CXU_SIGNALS_HXX

#include "object.hxx"

CXU_NS_BEGIN

class SignalBlocker : Entity
{
public:
    virtual ~SignalBlocker();
    SignalBlocker();
    explicit SignalBlocker(int);
    SignalBlocker(const std::set<int> &);

protected:
    sigset_t _ss;
};





CXU_NS_END


#endif
