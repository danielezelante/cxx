// YAL zeldan

#ifndef CXU_THREAD_HXX_
#define CXU_THREAD_HXX_

#include <pthread.h>


#include "sync.hxx"
#include "file.hxx"

CXU_NS_BEGIN


class Thread : Entity
{
    friend class WorkerThread;
    friend class MainThread;
    friend class Init;
    //friend class ExternalThread;


public:
    virtual ~Thread() = default;
    typedef void * t_retval;

    virtual const std::string name() const = 0;

    virtual bool isJoint() const = 0;
    virtual bool isStarted() const = 0;

    pthread_t getID() const
    {
        return _id;
    }

    // internal use on current thread
    static Thread * self();

    static void sleepit(const DeltaTime &);
    static void halt();
    static void yield();
    static void test();

    bool internal() const
    {
        return _setup && ::pthread_equal(getID(), ::pthread_self());
    }

protected:

    Thread();
    void setup(pthread_t);
    void setdown();


private:

    static pthread_key_t s_key_self;
    pthread_t _id;
    bool _setup;


};

class WorkerThread : public Thread, public Pollable
{
public:

    virtual ~WorkerThread();

    // external use
    void start();
    void join();
    virtual bool isJoint() const override;
    virtual bool isStarted() const override;

    t_retval getRV() const;

    void cancel();

    // Pollable
    virtual const FileDescriptor & fd() const override;
    virtual short events() const override;
    virtual bool match(short) const override;


    virtual const std::string name() const override;


protected:

    explicit WorkerThread(const std::string &);

    virtual t_retval run() = 0;
    static WorkerThread * specself();



private:

    std::string _name;
    bool _started; // just for a user point of view
    bool _joint;
    t_retval _rv;

    Pipe _endingPipe; // used to multiplex

    static void * s_proc(void *);

};

class MainThread : public Thread
{
public:

    MainThread();
    virtual ~MainThread();

    virtual const std::string name() const override;

    virtual bool isJoint() const override;
    virtual bool isStarted() const override;

    void trapSIGINT(void (*funct)(void));
    void trapSIGQUIT(void (*funct)(void));
    void trapSIGTERM(void (*funct)(void));
    void trapSIGHUP(void (*funct)(void));

    // ignore SIGPIPE
    //void ignoreSIGPIPE(); // fatto comunque da Main

    static const MainThread * specself();

    void addPoolPipe(const FileDescriptor &);
    void delPoolPipe(const FileDescriptor &);

protected:

    void trapSIG(int s, bool);
    void ignoreSIG(int s);

    void (*_mysigint)(void);
    void (*_mysigquit)(void);
    void (*_mysigterm)(void);

    void (*_mysighup)(void);
    static void s_sighandler(int);

    static void s_child(int);
    FdSet _poolpipes;


};

#if 0

class ExternalThread : public Thread
{
public:
    ExternalThread(const std::string &);
    virtual ~ExternalThread();

    //virtual bool isEnding() const;
    virtual bool isJoint() const;
};
#endif


CXU_NS_END

#define CXU_MAINTHREAD (CXU_NS::Init::s()->mainthread())



#endif /*CXU_THREAD_HXX_*/

//.
