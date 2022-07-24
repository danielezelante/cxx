// YAL zeldan

#ifndef CXU_FILE_HXX
#define CXU_FILE_HXX

#if __linux__
#include <sys/signalfd.h>
#endif

#include <poll.h>

#include <set>

#include "object.hxx"
#include "memory.hxx"
#include "time.hxx"
#include "signals.hxx"

CXU_NS_BEGIN


class FileDescriptor
: public Set<FileDescriptor, true>
, public TotalOrder<FileDescriptor>
, public XStringation<FileDescriptor, char>
{
public:

    virtual ~FileDescriptor() = default;

    FileDescriptor() : _fd(-1)
    {
    }

    explicit FileDescriptor(int x) : _fd(x)
    {
    }

    int operator+() const
    {
        return _fd;
    }

    bool operator!() const
    {
        return _fd < 0;
    }

    static int sdo_TotalOrder_compare(const FileDescriptor &, const FileDescriptor &);

    std::string _XStringation_char_str() const
    {
        return Format("FileDescriptor(%$)") % +(*this);
    }


protected:

    int _fd;

};

class File : Entity
{
public:
    virtual ~File() = default;
    virtual const FileDescriptor & fd() const = 0;
};

class Pollable : virtual public File
{ // to poll abstract things not based on i/o sych as Thread
public:
    virtual short events() const = 0;
    virtual bool match(short) const = 0;
};

class InPollableFile : virtual public File
{
};

class OutPollableFile : virtual public File
{
};

class RealFile : virtual public File
{
    friend class Process;

public:
    virtual ~RealFile();
    virtual const FileDescriptor & fd() const override;

    virtual bool close();
    bool close_nothrow();



protected:

    RealFile()
    {
    }

    void setup(const FileDescriptor &);
    void clear();

private:
    FileDescriptor _fd;
};

class ReadableFile : virtual public InPollableFile
{
protected:
    ReadableFile() = default;


public:

    virtual size_t read(const Segment &);
    size_t read_bufsize() const;
    bool readAll(const Segment &);
};

class WritableFile : virtual public OutPollableFile
{
protected:
    WritableFile() = default;


public:

    virtual size_t write(const ConstSegment &);
    size_t write_bufsize() const;
    bool writeAll(const ConstSegment &);

};

class ZableFile
: virtual public ReadableFile
, virtual public WritableFile
{
public:

    // upcasting functions useful in Poller

    const ReadableFile & rf() const
    {
        return *this;
    }

    ReadableFile & rf()
    {
        return *this;
    }

    const WritableFile & wf() const
    {
        return *this;
    }

    WritableFile & wf()
    {
        return *this;
    }

};

class SeekableFile : virtual public File
{
protected:
    SeekableFile() = default;


public:
    virtual size_t getOfs() const = 0;
    virtual void setOfs(off_t pos) = 0;
    virtual void setOfsRel(off_t pos) = 0;
    virtual void setOfsEnd() = 0;

    virtual size_t getSize() const = 0;
    virtual void setSize(size_t) = 0;
};



#if __linux__
//! File usable to expect a signal with a read

class SignalFile
: public RealFile
, virtual public ReadableFile
{
public:

    explicit SignalFile(int);
    SignalFile(const std::set<int> &);

    void readSig(signalfd_siginfo *);

protected:
    SignalBlocker _sb;

};
#endif

/*! \brief File.
 *
 * regular file
 */
class RegularFile
: public RealFile
, virtual public SeekableFile
, virtual public ZableFile
{
public:

    enum t_openmode
    {
        R, // read only
        RW, // read & write
        RWC, // read & write, create if not exists
        RWCT, // read & write, create if not exists, always truncate
    };


    virtual ~RegularFile();

    RegularFile(
                const char * lpFileName, //!< filename
                t_openmode om
                );


    //!\brief File constructor.
    //!\sa See also <a href="man:open(2)">open(2)</a>
    RegularFile(
                const char * lpFileName, //!< filename
                int flags, //!< flags as in open(2)
                mode_t mode //!< mode_t as in open(2)
                );


    virtual size_t getOfs() const override;
    virtual void setOfs(off_t pos) override;
    virtual void setOfsRel(off_t pos) override;
    virtual void setOfsEnd() override;

    virtual size_t getSize() const override;
    virtual void setSize(size_t) override;

};

class FileLock : public Entity
{
public:
    FileLock(File & file, bool write, bool wait = false);
    virtual ~FileLock();

private:
    File & _file; // TODO va fatto forse invece su FileDescriptor
};


//! read only memory mapping

class ConstFileMappingData : public Data
{
public:
    explicit ConstFileMappingData(const char * filename);
    virtual ~ConstFileMappingData();


protected:
    RegularFile _file;
    void * _map;

};


//! read-wirte memory mapping on a fixed size regular file

class FileMappingData : public Data
{
public:
    explicit FileMappingData(const char * filename);
    virtual ~FileMappingData();



protected:
    RegularFile _file;
    void * _map;


};


//! read-write-append memory mapping on a growable regular file

class FileMappingBuffer : public Buffer
{
public:
    FileMappingBuffer(const char * filename, bool create, size_t growby);
    virtual ~FileMappingBuffer();

    //using Buffer::operator =;
    FileMappingBuffer & operator=(const FileMappingBuffer &) = delete; // TODO implement ?

    virtual void resize(size_t) override;
    virtual void clear(size_t) override;
    virtual void clear() override;

private:
    RegularFile _file;
    void * _map;

    //size_t _filesize;
    size_t _growby;

    void map(size_t z);
    void unmap();
    size_t roundupz(size_t);
};

class Poller : Entity
{
public:
    virtual ~Poller() = default;

    void do_in(const FileDescriptor &);
    void do_out(const FileDescriptor &);

    Poller & operator|=(const InPollableFile & f)
    {
        do_in(f.fd());
        return *this;
    }

    Poller & operator|=(const OutPollableFile & f)
    {
        do_out(f.fd());
        return *this;
    }
    Poller & operator|=(const Pollable &);

    void wait();
    bool wait(const DeltaTime &);

    bool operator&(const FileDescriptor &) const; // return if File is in requested set

    bool operator&(const File & f) const
    {
        return (*this) & f.fd();
    } // return if File is in requested set

    bool operator&(const Pollable & p) const
    {
        return (*this) & p.fd();
    } // return if File is in requested set

    bool is_ok(const FileDescriptor &) const; // return if nor error or hangups

    bool is_ok(const File & f) const
    {
        return is_ok(f.fd());
    }

    bool is_ok(const Pollable & p) const
    {
        return is_ok(p.fd());
    }

    bool is_in(const FileDescriptor &) const;

    bool is_in(const File & f) const
    {
        return is_in(f.fd());
    }

    bool is_in(const Pollable & p) const
    {
        return is_in(p.fd());
    }

    bool is_out(const FileDescriptor &) const;

    bool is_out(const File & f) const
    {
        return is_out(f.fd());
    }

    bool is_out(const Pollable & p) const
    {
        return is_out(p.fd());
    }

    short operator[](const FileDescriptor &) const; // returns revents like in poll

    short operator[](const File & f) const
    {
        return (*this)[f.fd()];
    } // returns revents like in poll

    short operator[](const Pollable & p) const
    {
        return (*this)[p.fd()];
    } // returns revents like in poll

    bool operator/(const InPollableFile & f) const
    {
        return ((*this) & f) && ((*this)[f] & POLLIN);
    } // match test

    bool operator/(const OutPollableFile & f) const
    {
        return ((*this) & f) && ((*this)[f] & POLLOUT);
    } // match test

    bool operator/(const Pollable & p) const
    {
        return ((*this) & p) && (p.match((*this)[p]));
    } // match test

protected:

    struct Events
    {

        Events() : _req(0), _res(0)
        {
        }
        short _req;
        short _res;
    };


    typedef std::map<FileDescriptor, Events> t_map;
    t_map _map;

};


size_t poll(const Slice<pollfd> &, const DeltaTime &);
size_t poll(const Slice<pollfd> &);




//! wrapper for ::pipe

class Pipe : Entity
{
public:
    Pipe();
    //Pipe(int); //flags
    virtual ~Pipe();

    class RdEp
    : public RealFile
    , virtual public ReadableFile
    {
        friend class Pipe;
    };

    class WrEp
    : public RealFile
    , virtual public WritableFile
    {
        friend class Pipe;
    };

    const RdEp & rdep() const
    {
        //CXU_TRACE(Format("const Pipe this=%$") %ptrtoa(this));
        //CXU_TRACE(Format("const Pipe &_rdep=%$") %ptrtoa(&_rdep));
        return _rdep;
    }

    const WrEp & wrep() const
    {
        return _wrep;
    }

    RdEp & rdep()
    {
        //CXU_TRACE(Format("const Pipe this=%$") %ptrtoa(this));
        //CXU_TRACE(Format("const Pipe &_rdep=%$") %ptrtoa(&_rdep));
        return _rdep;
    }

    WrEp & wrep()
    {
        return _wrep;
    }



private:

    RdEp _rdep;
    WrEp _wrep;

};

class AnsiFILE : Entity
{
public:
    virtual ~AnsiFILE();
    AnsiFILE(const char * name, const char * mode);
    AnsiFILE(FILE *);

    FILE * file() const
    {
        return _file;
    }

    FILE * operator+() const
    {
        return _file;
    }

private:
    const bool _owned;
    FILE * _file;
};

class Circular : Entity
{
public:
    virtual ~Circular();

    explicit Circular(size_t z)
    : _mem(z)
    , _beg(0)
    , _end(0)
    {
    }

    size_t size() const
    {
        return _mem.size();
    }

    size_t fill() const
    {
        return (size() + _end - _beg) % size();
    }

    bool empty() const
    {
        return _beg == _end;
    }

    bool full() const
    {
        return fill() + 1 == size();
    }

    size_t write(WritableFile & f);
    size_t write(WritableFile &, size_t);
    size_t read(ReadableFile &);
    size_t read(ReadableFile &, size_t);

    size_t push(const ConstSegment &);
    size_t copy(const Segment &);
    size_t pull(const Segment &);

    void trash(size_t);

    char get();
    void put(char);

    void clear()
    {
        _beg = _end = 0;
    }


private:
    Memory _mem;
    size_t _beg;
    size_t _end;

};


// not stdc++ based

#if 0

class FdSet : Entity
{
public:
    virtual ~FdSet();
    FdSet();

    bool add(const FileDescriptor &);
    bool del(const FileDescriptor &);

    bool is(const FileDescriptor &) const;

    size_t upper() const
    {
        return _upper;
    } //use this to iterate

    size_t count() const;
    bool empty() const;

    void clear();

    FileDescriptor operator[](size_t) const;


protected:
    size_t _space;
    size_t _lhole;
    size_t _upper;
    FileDescriptor _maxim;
    int * _data;

    static const int E;

};


#endif

class FdSet : Entity
{
public:
    virtual ~FdSet();

    FdSet()
    : _count(0)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"          
        FD_ZERO(&_set);
#pragma GCC diagnostic pop           
    }


    bool add(const FileDescriptor & x);
    bool del(const FileDescriptor & x);
    bool is(const FileDescriptor & x) const;

    FileDescriptor upper() const
    {
        return _upper;
    }

    size_t count() const
    {
        return _count;
    }

    bool empty() const
    {
        return _count == 0;
    }

    void clear()
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"  
        FD_ZERO(&_set);
#pragma GCC diagnostic pop
        _count = 0;
        _upper = FileDescriptor();
    }

    FileDescriptor next(const FileDescriptor &) const;


protected:
    fd_set _set;
    FileDescriptor _upper;
    size_t _count;

};


CXU_NS_END

#endif


//.
