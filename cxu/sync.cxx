// YAL zeldan


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>


#include "sync.hxx"

#include "defs.hxx"
#include "exception.hxx"
#include "thread.hxx"
#include "memory.hxx"
#include "mystd.hxx"
#include "time.hxx"

#include "mymacros.hxx"


using namespace std;

CXU_NS_BEGIN


Lock::Lock(Lockable & la)
: _la(la)
{
    _la.lock();
}

Lock::~Lock()
{
    try
    {
        _la.unlock();
    }
    catch (const Exception & ex)
    {
        ex.log(Logger::e_error);
    }
}

RLock::RLock(RWLockable & la)
: _la(la)
{
    _la.lockR();
}

RLock::~RLock()
{
    try
    {
        _la.unlock();
    }
    catch (const Exception & ex)
    {
        ex.log(Logger::e_error);
    }
}

WLock::WLock(RWLockable & la)
: _la(la)
{
    _la.lockW();
}

WLock::~WLock()
{
    try
    {
        _la.unlock();
    }
    catch (const Exception & ex)
    {
        ex.log(Logger::e_error);
    }
}

Mutex::Mutex(int kind)
{
    ::pthread_mutexattr_init(&_mutex_attr);
    ::pthread_mutexattr_settype(&_mutex_attr, kind);
    ::pthread_mutex_init(&_mutex, &_mutex_attr);
}

Mutex::~Mutex()
{
    ::pthread_mutex_destroy(&_mutex);
    ::pthread_mutexattr_destroy(&_mutex_attr);
}

void Mutex::lock()
{
    const int x = ::pthread_mutex_lock(&_mutex);
    if (x) THROWERREXCEPTIONS(x, "pthread_mutex_lock");
}

bool Mutex::trylock()
{
    const int x = ::pthread_mutex_trylock(&_mutex);
    if (x == EBUSY) return false;
    if (x) THROWERREXCEPTIONS(x, "pthread_mutex_trylock");
    return true;
}

void Mutex::unlock()
{
    const int x = ::pthread_mutex_unlock(&_mutex);
    if (x) THROWERREXCEPTIONS(x, "pthread_mutex_unlock");
}

//** NRMutex

NRMutex::NRMutex() : Mutex
(
#ifdef NDEBUG
PTHREAD_MUTEX_NORMAL
#else
PTHREAD_MUTEX_ERRORCHECK
#endif
)
{
}



//** Condition

Condition::Condition()
{
    ::pthread_cond_init(&_cond, 0);
}

Condition::~Condition()
{
    int x = ::pthread_cond_destroy(&_cond);
    if (x)
        LOGERREXCEPTION(Logger::e_error, x);
}

void Condition::wait(Mutex & mutex)
{
    int x = ::pthread_cond_wait(&_cond, mutex.ptr());
    if (x) THROWERREXCEPTIONS(x, "pthread_cond_wait");
}

bool Condition::wait(Mutex & mutex, const struct timespec & timeout)
{
    int x = ::pthread_cond_timedwait(&_cond, mutex.ptr(), &timeout);
    if (x == ETIMEDOUT) return false;
    if (x) THROWERREXCEPTIONS(x, "pthread_cond_timedwait");
    return true;
}

void Condition::signal()
{
    int x = ::pthread_cond_signal(&_cond);
    if (x) THROWERREXCEPTIONS(x, "pthread_cond_signal");
}

void Condition::broadcast()
{
    int x = ::pthread_cond_broadcast(&_cond);
    if (x) THROWERREXCEPTIONS(x, "pthread_cond_broadcast");
}

void Sync::lock()
{
    _mutex.lock();
}

void Sync::unlock()
{
    _mutex.unlock();
}

RWSync::RWSync()
{
    RETERRCALLS(::pthread_rwlock_init(&_, 0), "pthread_rwlock_init(?, 0)");
}

RWSync::~RWSync()
{
    const int x = ::pthread_rwlock_destroy(&_);
    if (x) LOGERREXCEPTION(Logger::e_error, x);
}

void RWSync::lockR()
{
    RETERRCALLS(::pthread_rwlock_rdlock(&_), "pthread_rwlock_rdlock");
}

void RWSync::lockW()
{
    RETERRCALLS(::pthread_rwlock_wrlock(&_), "pthread_rwlock_wrlock");
}

void RWSync::unlock()
{
    RETERRCALLS(::pthread_rwlock_unlock(&_), "pthread_rwlock_unlock");
}


//** Notification

Notification::~Notification()
{
    ObjLock<t_notifiers> notifiers(_notifiers);
    ObjLock<t_waiters> waiters(_waiters);

    if (!notifiers->empty())
        LOGGER.error("Notification destructed with one or more pending Notifier");

    if (!waiters->empty())
        LOGGER.error("Notification destructed with one or more pending Waiter");

}

void Notification::wait()
{
    _cond.wait(_lock);
}

bool Notification::wait(const Time & upto)
{
    const bool x = _cond.wait(_lock, upto);
    return x;
}

bool Notification::wait(const DeltaTime & timeout)
{
    return wait(Time::now() + timeout);
}

void Notification::notify()
{
    _cond.signal();
}

void Notification::notifyAll()
{
    _cond.broadcast();
}

void Notification::lock()
{
    _lock.lock();
}

void Notification::unlock()
{
    _lock.unlock();
}

void Notification::insertNotifier(const Notifier * n)
{
    ObjLock<t_notifiers> notifiers(_notifiers);

    if (!notifiers->insert(n).second)
        LOGGER.error("duplicate Notifier in Notification");
}

void Notification::eraseNotifier(const Notifier * n)
{
    ObjLock<t_notifiers> notifiers(_notifiers);

    if (!notifiers->erase(n))
        LOGGER.error("Notifier not found in Notification");
}

void Notification::insertWaiter(const Waiter * w)
{
    ObjLock<t_waiters> waiters(_waiters);

    if (!waiters->insert(w).second)
        LOGGER.error("duplicate Waiter in Notification");
}

void Notification::eraseWaiter(const Waiter * w)
{
    ObjLock<t_waiters> waiters(_waiters);

    if (!waiters->erase(w))
        LOGGER.error("Waiter not found in Notification");
}


//** Waiter

Waiter::Waiter(Notification & na)
: _na(na)
{
    _na.lock();
    _na.insertWaiter(this);
}

Waiter::~Waiter()
{
    try
    {
        _na.eraseWaiter(this);
        _na.unlock();
    }
    catch (const Exception & ex)
    {
        ex.log(Logger::e_error);
    }
}


// cancellation point here

void Waiter::wait()
{
    _na.wait();
}

// cancellation point here

bool Waiter::wait(const DeltaTime & timeout)
{
    return _na.wait(timeout);
}

// cancellation point here

bool Waiter::wait(const Time & upto)
{
    return _na.wait(upto);
}




//** Notifier

Notifier::Notifier(Notification & na)
: _na(na)
{
    _na.lock();
    _na.insertNotifier(this);
}

Notifier::~Notifier()
{
    try
    {
        _na.eraseNotifier(this);
        _na.unlock();
    }
    catch (const Exception & ex)
    {
        ex.log(Logger::e_error);
    }
}

// one thread is notified

void Notifier::notify()
{
    _na.notify();
}


// all threads are notified

void Notifier::notifyAll()
{
    _na.notifyAll();
}

MutexIPC::MutexIPC(const char * device, int proj_id, int perm)
{
    _key = ftok(device, proj_id);

    union semun
    {
        int val;
        struct semid_ds *buf;
        unsigned short * array;
        struct seminfo *__buf;
    } argument;

    _id = semget(_key, 1, perm | IPC_CREAT | IPC_EXCL);
    if (_id >= 0)
    {
        argument.val = 1;
        ERRCALLS(
                 semctl(_id, 0, SETVAL, argument),
                 "semctl(?, 0 , SETVAL, ?)"
                 );
    }
    else
    {
        if (errno == EEXIST)
            ERRCALLS(
                     _id = semget(_key, 1, 0666),
                     "semget(?, 1, 0666)"
                     );
        else
            THROWERREXCEPTIONHERE(); // TODO smazzare logica contorta
    }

    struct sembuf operations[1];
    operations[0].sem_num = 0;
    operations[0].sem_op = -1;
    operations[0].sem_flg = SEM_UNDO;

    ERRCALLS(
             semop(_id, operations, 1),
             "semop(?, ?, 1)"
             );

}

MutexIPC::~MutexIPC()
{
    struct sembuf operations[1];

    operations[0].sem_num = 0;
    operations[0].sem_op = 1;
    operations[0].sem_flg = SEM_UNDO;

    if (-1 == semop(_id, operations, 1))
        LOGERREXCEPTIONHERE(Logger::e_error);
}



CXU_NS_END

//.
