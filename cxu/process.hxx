// YAL zeldan


#ifndef CXU_PROCESS_HXX
#define CXU_PROCESS_HXX


#include "sync.hxx"
#include "memory.hxx"
#include "signals.hxx"
#include "file.hxx"

CXU_NS_BEGIN


class Process : public Entity
{
    friend class ProcessPool;

public:

    Process(ProcessPool *,
            const std::string &path
            );

    Process(ProcessPool *,
            const std::string &path,
            const std::vector<std::string> &argv
            );


    Process(ProcessPool *,
            const std::string &path,
            const std::vector<std::string> &argv,
            const std::vector<std::string> &env
            );

    virtual ~Process();

#if __linux__
    void usePath(bool);
#endif

    void start(Pipe * pIn, Pipe * pOut, Pipe * pErr);
    void start();

    int wait();
    int waitNoHang();
    void kill(int sig = SIGTERM);


    int getExitCode() const;
    int getSignal() const;

    int getStatus() const
    {
        return _exitStatus;
    }


private:
    int wait(int);

    ProcessPool * _pool;

    bool _usepath;
    const std::string _path;
    const std::vector<std::string> _argv;
    const std::vector<std::string> _env;

    bool _started;

    int _exitStatus;
    pid_t _pid;

};

class ProcessPool
: virtual public Pollable
{
public:
    virtual ~ProcessPool();
    ProcessPool();

    Process * waitAny();
    Process * waitAny(const DeltaTime &);

    bool empty() const
    {
        return _procs.empty();
    }

    void add(Process *);
    void del(Process *);

    // Pollable
    // after poll match, waitAny will not block

    const FileDescriptor & fd() const override
    {
        return _pipe.rdep().fd();
    }

    short events() const override
    {
        return POLLIN;
    }

    bool match(short x) const override
    {
        return x & POLLIN;
    }

    void killAll(int);

protected:
    Pipe _pipe; // to be notified by SIGCHLD handler

    std::set<Process*> _procs;

};


CXU_NS_END



#endif /*CXU_PROCESS_HXX*/

//.
