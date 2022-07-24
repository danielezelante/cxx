//YAL zeldan

#ifndef CXU_OBJECT_HXX_
#define CXU_OBJECT_HXX_

#include <set>

#include "defs.hxx"
#include "types.hxx"
#include "globals.hxx"

CXU_NS_BEGIN



#define CXU_CRTP_OV(T,V) (static_cast<T&>(V))
#define CXU_CRTP_CV(T,V) (static_cast<const T&>(V))

#define CXU_CRTP_O(T) CXU_CRTP_OV(T,*this)
#define CXU_CRTP_C(T) CXU_CRTP_CV(T,*this)


// previous verion was based on virtual
// it was polymorfic in output but not in input
// so no advantage
// this version is CRTP based and so not polymorfic
//! Serialization class
/*! Inherit <> from this class to make a class serializable */
template <class T> class Serialization
{
    //CXU_CRTP_DEFINES(Serialization, T)

public:
    //! Serialize to Binary Output Storage

    void serialize(BOStorage & s) const
    {
        CXU_CRTP_C(T)._Serialization_serialize(s);
    }

    //! Serialize from Binary Input Storage

    void serialize(BIStorage & s)
    {
        CXU_CRTP_O(T)._Serialization_serialize(s);
    }

    //! Serialize to Text Output Storage

    void serialize(TOStorage & s) const
    {
        CXU_CRTP_C(T)._Serialization_serialize(s);
    }

    //! Serialize from Text Input Storage

    void serialize(TIStorage & s)
    {
        CXU_CRTP_O(T)._Serialization_serialize(s);
    }

    //! Get pretty class name

    /*! Should be used to implement text serialization */
    const char * classname() const
    {
        return CXU_CRTP_C(T)._Serialization_classname();
    }
};

#define CXU_DECLARE_SERIALIZABLE(T) \
public: const char * _Serialization_classname() const {return #T;} protected:



// non assignable

class Constant
{
protected:
    constexpr Constant() = default;
    Constant(const Constant &) = default;
    Constant(Constant &&) = default; // allow moving, by doing nothing

private:
    Constant & operator=(const Constant &) = delete;
};


//! Non copiable class constraint

/*! Inherit from this class to make a class uncopiable */
class Entity : public Constant
{
protected:
    constexpr Entity() = default;
    Entity(Entity &&) = default; // allow moving, by doing nothing

private:
    Entity(const Entity &) = delete;
};


//! Dumpable to TextOutput class interface

class Dumpable
{
public:
    virtual ~Dumpable() = default;

    /*! 
     * dump to TextOutput
     * @param to TextOutput to dump to
     * @param prefix a string to prefix for each dumped line
     */
    virtual void dump(TextOutput & to, const std::string & prefix) const = 0;
};

template <class T> class XStringable
{
protected:
    XStringable() = default;

public:
    virtual ~XStringable() = default;

    /*!
     * represents object as a string
     * @return user friendly string, should not contain '\\n'
     */
    virtual std::basic_string<T> str() const = 0;

    operator std::basic_string<T> () const
    {
        return str();
    }

};


typedef XStringable<char> Stringable;
typedef XStringable<wchar_t> WStringable;

template <class T, class K> class XStringation
{
    //CXU_CRTP_DEFINES(XStringation,T,K)
};

template <class T> class XStringation<T, char>
{
    //CXU_CRTP_DEFINES(XStringation,T,char)
public:

    std::string str() const
    {
        return CXU_CRTP_C(T)._XStringation_char_str();
    }
};

template <class T> class XStringation<T, wchar_t>
{
    //CXU_CRTP_DEFINES(XStringation,T,wchar_t)
public:

    std::wstring str() const
    {
        return CXU_CRTP_C(T)._XStringation_wchar_str();
    }
};

template <class T> class GlobalPointerSetup : Entity
{
public:

    virtual ~GlobalPointerSetup()
    {
        if (_gptr) *_gptr = 0;
    }

    GlobalPointerSetup(T ** gptr, T & obj)
    : _gptr(gptr)
    {
        if (_gptr) *gptr = &obj;
    }


private:
    T ** _gptr;
};




CXU_NS_END


#endif

//.
