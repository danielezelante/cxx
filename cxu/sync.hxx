// YAL zeldan

#ifndef CXU_SYNC_HXX_
#define CXU_SYNC_HXX_

#include <set>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include <pthread.h>

#include "types.hxx"
#include "object.hxx"

CXU_NS_BEGIN


class Lockable
{
public:
    virtual ~Lockable() = default;
    virtual void lock() = 0;
    virtual void unlock() = 0;
};

class RWLockable
{
public:
    virtual ~RWLockable() = default;
    virtual void lockR() = 0;
    virtual void lockW() = 0;
    virtual void unlock() = 0;
};

class Lock : Entity
{
public:
    explicit Lock(Lockable &);
    virtual ~Lock();

protected:
    Lockable & _la;
};

class RLock : Entity
{
public:
    explicit RLock(RWLockable &);
    ~RLock();

protected:
    RWLockable & _la;
};

class WLock : Entity
{
public:
    explicit WLock(RWLockable &);
    ~WLock();

protected:
    RWLockable & _la;
};

class Mutex : public Lockable
{
public:
    virtual ~Mutex();

    void lock() override;
    bool trylock();
    void unlock() override;

    pthread_mutex_t * ptr()
    {
        return &_mutex;
    }

protected:
    explicit Mutex(int kind);
    pthread_mutexattr_t _mutex_attr;
    pthread_mutex_t _mutex;
};


// recursive mutex

class RMutex : public Mutex
{
public:

    RMutex() : Mutex(PTHREAD_MUTEX_RECURSIVE)
    {
    }
};

// non recursive mutex (checked in debug)

class NRMutex : public Mutex
{
public:
    NRMutex();
};

class Condition : Entity
{
public:
    Condition();
    ~Condition();

    void wait(Mutex &);
    bool wait(Mutex &, const struct timespec & timeout_ms);
    void signal();
    void broadcast();

    pthread_cond_t * ptr()
    {
        return &_cond;
    }


protected:
    pthread_cond_t _cond;


};

class Sync : public Lockable
{
public:
    void lock() override;
    void unlock() override;

protected:
    RMutex _mutex;
};

class RWSync : public RWLockable
{
public:
    RWSync();
    virtual ~RWSync();
    void lockR() override;
    void lockW() override;
    void unlock() override;

protected:
    pthread_rwlock_t _;
};

template <typename T> class ObjSync
{
    friend class ObjLock<T>;
    friend class ConstObjLock<T>;

public:
    virtual ~ObjSync() = default;
    ObjSync() = default;
    virtual RWLockable & rwl() const = 0;

protected:
    virtual T & obj() = 0;
    virtual const T & obj() const = 0;
};

template <typename T> class VarObjSync : public ObjSync<T>
{
public:

    RWLockable & rwl() const override
    {
        return _sy;
    }

protected:

    T & obj() override
    {
        return _obj;
    }

    const T & obj() const override
    {
        return _obj;
    }

private:
    mutable RWSync _sy;
    T _obj;
};

template <typename T> class RefObjSync : public ObjSync<T>
{
public:

    explicit RefObjSync(T & x) : _obj(x)
    {
    }

    RWLockable & rwl() const override
    {
        return _sy;
    }

protected:

    T & obj() override
    {
        return _obj;
    }

    const T & obj() const override
    {
        return _obj;
    }

private:
    mutable RWSync _sy;
    T & _obj;
};

template <typename T> class ObjLock : Entity
{
public:

    explicit ObjLock(ObjSync<T> & so)
    : _lock(so.rwl())
    , _so(so)
    {
    }

    T & operator*() const
    {
        return _so.obj();
    }

    T * operator->() const
    {
        return &(_so.obj());
    }

protected:
    WLock _lock;
    ObjSync<T> & _so;
};

template <typename T> class ConstObjLock : Entity
{
public:

    explicit ConstObjLock(const ObjSync<T> & so)
    : _lock(so.rwl())
    , _so(so)
    {
    }

    const T & operator*() const
    {
        return _so.obj();
    }

    const T * operator->() const
    {
        return &(_so.obj());
    }

protected:
    RLock _lock;
    const ObjSync<T> & _so;
};

class Notification
{
    friend class Waiter;
    friend class Notifier;

public:

    Notification() = default;
    virtual ~Notification();


protected:

    void notify();
    void notifyAll();

    void lock();
    void unlock();

    void wait();
    bool wait(const DeltaTime & timeout);
    bool wait(const Time & upto);

    void insertNotifier(const Notifier *);
    void eraseNotifier(const Notifier *);

    void insertWaiter(const Waiter *);
    void eraseWaiter(const Waiter *);


    Condition _cond;
    NRMutex _lock;

    typedef std::set<const Waiter*> t_waiters;
    VarObjSync<t_waiters>_waiters;

    typedef std::set<const Notifier*> t_notifiers;
    VarObjSync<t_notifiers> _notifiers;
};

class Waiter : Entity
{
    friend class Notification;

public:
    explicit Waiter(Notification &);
    virtual ~Waiter();

    void wait();
    bool wait(const DeltaTime & timeout); // false = timeout
    bool wait(const Time & upto); // :> false = timeout

protected:
    Notification & _na;


};

class Notifier : Entity
{
public:
    explicit Notifier(Notification &);
    virtual ~Notifier();

    void notify();
    void notifyAll();


protected:
    Notification & _na;

};

template <class T> class ObjWaiter;
template <class T> class ObjNotifier;

template <class T> class ObjNotification : Entity
{
    friend class ObjWaiter<T>;
    friend class ObjNotifier<T>;

public:

    const Notification & notification() const
    {
        return _notification;
    }

    Notification & notification()
    {
        return _notification;
    }

protected:

    T & obj()
    {
        return _obj;
    }

    const T & obj() const
    {
        return _obj;
    }

private:
    Notification _notification;
    T _obj;
};

template <class T> class ObjWaiter : Entity
{
public:

    explicit ObjWaiter(ObjNotification<T> & n)
    : _obj(n.obj())
    , _waiter(n.notification())
    {
    }

    void wait()
    {
        _waiter.wait();
    }

    T & operator*()
    {
        return _obj;
    }

    const T & operator*() const
    {
        return _obj;
    }

    T * operator->()
    {
        return &_obj;
    }

    const T * operator->() const
    {
        return &_obj;
    }


protected:
    T & _obj;
    Waiter _waiter;

};

template <class T> class ObjNotifier : Entity
{
public:

    explicit ObjNotifier(ObjNotification<T> & n)
    : _obj(n.obj())
    , _notifier(n.notification())
    {
    }

    void notify()
    {
        _notifier.notify();
    }

    T & operator*()
    {
        return _obj;
    }

    const T & operator*() const
    {
        return _obj;
    }

    T * operator->()
    {
        return &_obj;
    }

    const T * operator->() const
    {
        return &_obj;
    }


protected:
    T & _obj;
    Notifier _notifier;

};

class MutexIPC : Entity
{
public:
    MutexIPC(const char * device, int proj_id, int perm = 0666);
    virtual ~MutexIPC();

protected:
    key_t _key;
    int _id;
};





CXU_NS_END

#define CXU_MAINTHREAD (CXU_NS::Init::s()->mainthread())

#endif /*SYNC_HXX_*/

//.
