// YAL zeldan

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/uio.h>

#if __linux__
#include <sys/signalfd.h>
#endif

#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#include <set>

#ifdef __HAIKU__
#define INFTIM (-1)
#endif

#include "file.hxx"

#include "defs.hxx"
#include "sync.hxx"
#include "thread.hxx"
#include "exception.hxx"
#include "string.hxx"
#include "text.hxx"

#include "mymacros.hxx"

using namespace std;

CXU_NS_BEGIN


//** File


size_t ReadableFile::read(const Segment & s)
{
    const ssize_t x = ::read(+fd(), s.ptr(), s.size());
    if (x < 0)
        THROWERREXCEPTIONHERES(
                               Format("read(%$, %$, %$)") % fd() % ptrtoa(s.ptr()) % s.size()
                               );
    return x;
}

size_t WritableFile::write(const ConstSegment & s)
{
    const ssize_t x = ::write(+fd(), s.ptr(), s.size());
    if (x < 0)
        THROWERREXCEPTIONHERES(
                               Format("write(%$, %$, %$)") % fd() % ptrtoa(s.ptr()) % s.size()
                               );
    return x;
}

int FileDescriptor::sdo_TotalOrder_compare(const FileDescriptor & f0, const FileDescriptor & f1)
{
    const int fd0 = +f0;
    const int fd1 = +f1;
    if (fd0 < fd1) return -1;
    if (fd0 > fd1) return +1;
    return 0;
}

size_t ReadableFile::read_bufsize() const
{
    struct stat buf;
    ERRCALLS(::fstat(+fd(), &buf), Format("fstat(%$, ...)") % fd());
    return buf.st_blksize;
}

size_t WritableFile::write_bufsize() const
{
    struct stat buf;
    ERRCALLS(::fstat(+fd(), &buf), Format("fstat(%$, ...)") % fd());
    return buf.st_blksize;
}


#if __linux__
//** SignalFile

SignalFile::SignalFile(int s)
: _sb(s)
{
    sigset_t ss;
    ZERO(ss);
    ERRCALLS(
             ::sigaddset(&ss, s),
             Format("sigaddset(?, %$)") % s
             );

    const int fdx = ::signalfd(-1, &ss, SFD_CLOEXEC);
    if (fdx < 0)
        THROWERREXCEPTIONHERES(
                               Format("signalfd(-1, <%$>, SFD_CLOEXEC") % s
                               );

    setup(FileDescriptor(fdx));
}

SignalFile::SignalFile(const set<int> & q)
: _sb(q)
{
    sigset_t ss;
    ZERO(ss);
    for (auto s : q)
        ERRCALLS(
                 ::sigaddset(&ss, s),
                 Format("sigaddset(?, %$)") % s
                 );

    const int fdx = ::signalfd(-1, &ss, SFD_CLOEXEC);
    if (fdx < 0)
        THROWERREXCEPTIONHERES("signalfd(-1, ?, SFD_CLOEXEC");

    setup(FileDescriptor(fdx));
}

void SignalFile::readSig(signalfd_siginfo * a)
{
    ssize_t x = -1;
    if (a)
    {
        x = ::read(+fd(), a, sizeof (*a));
    }
    else
    {
        char dummy[sizeof (*a)];
        x = ::read(+fd(), dummy, sizeof (*a));
    }
    if (x != sizeof (*a)) THROWOSEXCEPTION(Format("wrong read size : %$") % x);
}

#endif

bool WritableFile::writeAll(const ConstSegment & s)
{
    size_t tot = 0;
    while (tot < s.size())
    {
        const size_t n = write(s + tot);
        if (!n) return false;
        tot += n;
        ASSERT(tot <= s.size());
    }
    return true;
}

bool ReadableFile::readAll(const Segment & s)
{
    size_t tot = 0;
    while (tot < s.size())
    {
        const size_t n = read(s + tot);
        if (!n) return false;
        tot += n;
        ASSERT(tot <= s.size());
    }
    return true;
}





const void * MMAP_FAILED = reinterpret_cast<void*> (-1);
static const off_t FNPOS = static_cast<off_t> (-1);

RegularFile::~RegularFile()
{
}

RegularFile::RegularFile(const char * name, int flags, mode_t mode)
{
    const int fdx = ::open(name, flags | O_CLOEXEC, mode);
    if (fdx < 0)
        THROWERREXCEPTIONHERES(
                               Format("open(%$, %$, %$)")
                               % quote(name)
                               % utox(flags | O_CLOEXEC)
                               % utoo(mode)
                               );
    setup(FileDescriptor(fdx));
}

RegularFile::RegularFile(const char * name, t_openmode om)
{
    int flags = 0;

    switch (om)
    {
    case R:
        flags = O_RDONLY;
        break;

    case RW:
        flags = O_RDWR;
        break;

    case RWC:
        flags = O_RDWR | O_CREAT;
        break;

    case RWCT:
        flags = O_RDWR | O_CREAT | O_TRUNC;
        break;


    default:
        ASSERT(false);
    }

    const int fdx = ::open(name, flags | O_CLOEXEC, 0666);
    if (fdx < 0)
        THROWERREXCEPTIONHERES(
                               Format("open(%$, %$, 0666)")
                               % quote(name)
                               % utox(flags | O_CLOEXEC)
                               );
    setup(FileDescriptor(fdx));

}

void RegularFile::setOfs(off_t pos)
{
    const off_t x = ::lseek(+fd(), pos, pos >= 0 ? SEEK_SET : SEEK_END);
    if (x == FNPOS)
        THROWERREXCEPTIONHERES(
                               Format("lseek(%$, %$, %$)")
                               % fd() % pos % (pos >= 0 ? "SEEK_SET" : "SEEK_END")
                               );
}

void RegularFile::setOfsRel(off_t pos)
{
    const off_t x = ::lseek(+fd(), pos, SEEK_CUR);
    if (x == FNPOS)
        THROWERREXCEPTIONHERES(
                               Format("lseek(%$, %$, SEEK_CUR)") % fd() % pos
                               );
}

void RegularFile::setOfsEnd()
{
    const off_t x = ::lseek(+fd(), 0, SEEK_END);
    if (x == FNPOS)
        THROWERREXCEPTIONHERES(
                               Format("lseek(%$, 0, SEEK_END)") % fd()
                               );
}

size_t RegularFile::getOfs() const
{
    const off_t pos = ::lseek(+fd(), 0, SEEK_CUR);
    if (pos == FNPOS)
        THROWERREXCEPTIONHERES(
                               Format("lseek(%$, 0, SEEK_CUR") % fd()
                               );
    return pos;
}

size_t RegularFile::getSize() const
{
    struct stat a;
    ERRCALLS(::fstat(+fd(), &a), Format("fstat(%$, ?)") % fd());
    return a.st_size;
}

void RegularFile::setSize(size_t n)
{
    ERRCALLS(::ftruncate(+fd(), n), Format("ftruncate(%$, %$)") % fd() % n);
}



//********************** FileLock

//TODO check use of fcntl / flock / lockf

FileLock::FileLock(File & f, bool write, bool wait)
: _file(f)
{
    struct flock k;
    memset(&k, 0, sizeof (k));
    k.l_whence = SEEK_SET;
    k.l_start = 0;
    k.l_len = 0;
    k.l_type = write ? F_WRLCK : F_RDLCK;

    if (-1 == ::fcntl(+_file.fd(), wait ? F_SETLKW : F_SETLK, &k))
        THROWERREXCEPTIONHERES(
                               Format("fcntl(%$, ...)") % _file.fd()
                               );

}

FileLock::~FileLock()
{
    struct flock k;
    memset(&k, 0, sizeof (k));
    k.l_whence = SEEK_SET;
    k.l_start = 0;
    k.l_len = 0;
    k.l_type = F_UNLCK;

    if (-1 == ::fcntl(+_file.fd(), F_SETLK, &k))
        LOGERREXCEPTIONHERE(Logger::e_error);
}


//***** ConstFileMappingData

ConstFileMappingData::ConstFileMappingData(const char * filename) :
_file(filename, RegularFile::R)
{
    const size_t z = _file.getSize();

    if (z)
    {
        _map = ::mmap(
                      0,
                      z,
                      PROT_READ,
                      MAP_PRIVATE, // we do now write anyway
                      +_file.fd(),
                      0
                      );

        if (_map == MMAP_FAILED)
            THROWERREXCEPTIONHERES(
                                   Format("mmap(0, %$, PROT_READ, MAP_PRIVATE, %$, 0)")
                                   % z % _file.fd()
                                   );
        csetup(reinterpret_cast<const char *> (_map), z);
    };
}

ConstFileMappingData::~ConstFileMappingData()
{
    if (_map)
    {
        if (-1 ==
                ::munmap
                (_map, size())
                )
            LOGERREXCEPTIONHERE(Logger::e_error);
    }
}



//***** FileMappingData

FileMappingData::FileMappingData(const char * filename) :
_file(filename, RegularFile::RW)
{
    const size_t z = _file.getSize();

    if (z) // TODO check if mmap of 0 size is ok
    {
        _map = ::mmap(
                      0,
                      z,
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED,
                      +_file.fd(),
                      0
                      );

        if (_map == MMAP_FAILED)
            THROWERREXCEPTIONHERES(
                                   Format("mmap(0, %$, PROT_READ|PROT_WRITE, MAP_SHARED, %$, 0)")
                                   % z % _file.fd()
                                   );

        setup(reinterpret_cast<char*> (_map), z);
    }
}

FileMappingData::~FileMappingData()
{
    if (_map)
    {
        if (-1 == ::munmap(_map, size()))
            LOGERREXCEPTIONHERE(Logger::e_error);
    }
}



//**** FileMappingBuffer

FileMappingBuffer::FileMappingBuffer(
                                     const char * filename,
                                     bool create,
                                     size_t growby
                                     ) :
_file(filename, create ? RegularFile::RWC : RegularFile::RW)
{
    //TRACE(Format("FileMappingBuffer(%$) BEGIN") %quote(filename));

    struct stat s;
    ::fstat(+_file.fd(), &s);
    if (!S_ISREG(s.st_mode)) THROWINTERFACEEXCEPTION("not regular");

    const size_t gran = std::max(
                                 static_cast<size_t> (sysconf(_SC_PAGESIZE)),
                                 static_cast<size_t> (s.st_blksize)
                                 );

    _growby = growby - growby % gran;
    _growby = std::max(_growby, gran);
    if (create) _file.setSize(growby);
    map(create ? 0 : _file.getSize());

    //TRACE("FileMappingBuffer END");

}

FileMappingBuffer::~FileMappingBuffer()
{
    //TRACE("~FileMappingBuffer BEGIN");

    if (_map)
    {
        if (-1 == ::munmap(_map, _file.getSize()))
            LOGERREXCEPTIONHERE(Logger::e_error);
    }

    if (-1 == ::ftruncate(+_file.fd(), size()))
        LOGERREXCEPTIONHERE(Logger::e_error);


    //TRACE("~FileMappingBuffer END");
}

void FileMappingBuffer::map(size_t z)
{
    const size_t q = _file.getSize();
    ASSERT(z <= q);

    _map = static_cast<char*> (
            ::mmap(
                   0, q, PROT_READ | PROT_WRITE, MAP_SHARED, +_file.fd(), 0
                   )
            );
    if (_map == MMAP_FAILED)
        THROWERREXCEPTIONHERES(
                               Format("mmap(0, %$, PROT_READ|PROT_WRITE, MAP_SHARE, %$, 0)")
                               % q % _file.fd()
                               );
    ASSERT(_map);
    setup(reinterpret_cast<char*> (_map), z);
}

void FileMappingBuffer::unmap()
{
    ASSERT(ptr());
    ERRCALLS(::munmap(_map, _file.getSize()), "munmap(...)");
    setup(0, 0);
}

void FileMappingBuffer::resize(size_t n)
{
    if (n == size()) return;

    if (n > size())
    {
        if (n > _file.getSize())
        {
            unmap();
            _file.setSize(roundupz(n));
            map(n);
        }
        else
            setup(ptr(), n);
    }

    if (n < size())
    {
        if (_file.getSize() - n > 2 * _growby)
        {
            unmap();
            _file.setSize(roundupz(n));
            map(n);
        }
        else
            setup(ptr(), n);

    }
}

void FileMappingBuffer::clear(size_t n)
{
    TRACE(Format("FileMappingBuffer::clear(%$)") % n);
    resize(n);
}

void FileMappingBuffer::clear()
{
    TRACE("FileMappingBuffer::clear()");
    clear(0);
}

size_t FileMappingBuffer::roundupz(size_t n)
{
    return n + _growby - (n % _growby);
}


//** Poller

void Poller::do_in(const FileDescriptor & f)
{
    Events & p = _map[f];
    p._req |= POLLIN;
}

void Poller::do_out(const FileDescriptor & f)
{
    Events & p = _map[f];
    p._req |= POLLOUT;
}

Poller & Poller::operator|=(const Pollable & p)
{
    Events & q = _map[p.fd()];
    q._req |= p.events();
    return *this;
}

void Poller::wait()
{
    if (!wait(DeltaTime::s(-1)))
        THROWOSEXCEPTION("got timeout, when no timeout requested");
}

bool Poller::wait(const DeltaTime & to)
{
    for (auto & it : _map) it.second._res = 0;

    const size_t z = _map.size();
    struct pollfd fds[z];
    {
        size_t j = 0;
        for (auto const & it : _map)
        {
            fds[j].fd = +it.first;
            fds[j].events = it.second._req;
            fds[j].revents = 0;
            ++j;
        }
    }

    for (;;)
    {
#if __linux__ || __sun__
        const int x =
                ::ppoll(fds, z, to >= DeltaTime::O() ? to.timespec_cptr() : NULL, NULL);
#else
        const long toms = to.tot_ms();
        const int x =
                ::poll(fds, z, toms >= 0 ? toms : INFTIM);
#endif
        if (x < 0)
        {
            if (errno == EINTR)
            {
                //LOGGER.debug("EINTR in poll");
                continue;
            }

            const string excmsg =
#if __linux__ || __sun__
                    Format("ppoll(?, %$, %$, NULL, NULL)")
                    % z
                    % (to >= DeltaTime::O() ? to.str() : "NULL")
#else
                    Format("poll(?, %$, %$)") % z % (toms >= 0 ? toms : INFTIM)
#endif     
                    ;

            THROWERREXCEPTIONHERES(excmsg);

        }
        if (!x) return false;
        break;
    }

    LOOP(j, z)
    {
        const t_map::iterator itx = _map.find(FileDescriptor(fds[j].fd));
        if (itx == _map.end())
            THROWOSEXCEPTION(Format("fd not found after poll: %$") % fds[j].fd);
        itx->second._res = fds[j].revents;
    }

    return true;

}

bool Poller::operator&(const FileDescriptor & f) const
{
    return _map.find(f) != _map.end();
}

bool Poller::is_in(const FileDescriptor & f) const
{
    if (!((*this) & f)) return false;
    return ((*this)[f]) & POLLIN;
}

bool Poller::is_out(const FileDescriptor & f) const
{
    if (!((*this) & f)) return false;
    return ((*this)[f]) & POLLOUT;
}

bool Poller::is_ok(const FileDescriptor & f) const
{
    if (!((*this) & f)) return false;
    return !(((*this)[f]) & (POLLHUP | POLLERR | POLLNVAL
#if __linux__
            | POLLRDHUP
#endif
            ));
}

short Poller::operator[](const FileDescriptor & f) const
{
    const t_map::const_iterator it = _map.find(f);
    if (it == _map.end()) THROWINTERFACEEXCEPTION(Format("fd not found: %$") % f);
    return it->second._res;
}

size_t poll(const Slice<pollfd> & a, const DeltaTime & t)
{
    const int x =
#if __linux__
            ::ppoll(
                    a.ptr(),
                    a.size(),
                    t >= DeltaTime::O() ? t.timespec_cptr() : NULL,
                    NULL
                    );
#else
            ::poll(
                   a.ptr(),
                   a.size(),
                   t.tot_ms()
                   );
#endif

    if (x < 0)
        THROWERREXCEPTIONHERES(
                               Format("ppoll(%$, %$, %$, NULL)")
                               % ptrtoa(a.ptr())
                               % a.size()
                               % (t >= DeltaTime::O() ? t.str() : "NULL")
                               );
    ASSERT(a.size() <= INT_MAX);
    if (x > static_cast<int> (a.size()))
        THROWOSEXCEPTION(
                         Format("ppoll return value too large: ret=%$ max=%$")
                         % x % a.size()
                         );

    return x;
}

size_t poll(const Slice<pollfd> & a)
{
    return poll(a, DeltaTime::s(-1));
}



// Pipe

Pipe::~Pipe()
{
    // ci pensano i distruttore di FileFD a fare le close
}

Pipe::Pipe()
{
    int x[2];
#if __linux__
    ERRCALLS(::pipe2(x, O_CLOEXEC), "pipe2(?, O_CLOEXEC)");
#else
    ERRCALLS(::pipe(x), "pipe(?)");
    ERRCALLS(::fcntl(x[0], F_SETFD, FD_CLOEXEC), "fcntl([0], F_SETFD, FD_CLOEXEC)");
    ERRCALLS(::fcntl(x[1], F_SETFD, FD_CLOEXEC), "fcntl([1], F_SETFD, FD_CLOEXEC)");
#endif
    _rdep.setup(FileDescriptor(x[0]));
    _wrep.setup(FileDescriptor(x[1]));
}


// RealFile

RealFile::~RealFile()
{
    close_nothrow();
}

bool RealFile::close()
{
    const FileDescriptor x = fd();
    if (!x) return false;
    clear();
    ERRCALLS(::close(+x), Format("close(%$)") % x);
    return true;
}

bool RealFile::close_nothrow()
{
    const FileDescriptor x = fd();
    if (!x) return false;
    clear();
    if (::close(+x) < 0)
        LOGERREXCEPTIONHERES(
                             Logger::e_error,
                             Format("close(%$)") % x
                             );
    return true;
}

void RealFile::setup(const FileDescriptor & x)
{
    ASSERT(!_fd);
    ASSERT(!!x);
    _fd = x;
}

void RealFile::clear()
{
    _fd = FileDescriptor();
}

const FileDescriptor & RealFile::fd() const
{
    return _fd;
}


//** AnsiFILE

AnsiFILE::~AnsiFILE()
{
    if (_owned)
        if (::fclose(_file))
            LOGERREXCEPTIONHERE(Logger::e_error);
}

AnsiFILE::AnsiFILE(const char * path, const char * mode)
: _owned(true)
{
    _file = ::fopen(path, mode);
    if (!_file)
        THROWERREXCEPTIONHERES(
                               Format("fopen(%$, %$)") % quote(path) % quote(mode)
                               );
}

AnsiFILE::AnsiFILE(FILE * f)
: _owned(false)
{
    _file = f;
}



//** Circular

Circular::~Circular()
{
    if (!empty())
        LOGGER.debug(Format("~Circular fill=%$") % fill());
}

size_t Circular::write(WritableFile & f)
{
    ssize_t t = 0;
    if (_end >= _beg)
    {
        ConstSegment tow(_mem, _beg, _end - _beg);

        //TextOutputFILE tof(stderr);
        //tof.dump(tow, "WU: ");
        t = f.write(tow);
        //LOGGER.debug(Format("Circular write(%$,%$) => %$") %_beg %(_end - _beg) %t);
    }
    else
    {
        struct iovec q[2];
        q[0].iov_base = const_cast<char*> (_mem.cptr() + _beg); // ma a quelli di POSIX il const non glielo hanno spiegato ?
        q[0].iov_len = size() - _beg;
        q[1].iov_base = const_cast<char*> (_mem.cptr()); // ma a quelli di POSIX il const non glielo hanno spiegato ?
        q[1].iov_len = _end;
        t = ::writev(+f.fd(), q, DIM(q));
        //LOGGER.debug(Format("Circular writev({{%$,%$},{%$,%$}) => %$")
        //%(reinterpret_cast<const char*>(q[0].iov_base) - _mem.cptr())
        //%q[0].iov_len
        //%(reinterpret_cast<const char*>(q[1].iov_base) - _mem.cptr())
        //%q[1].iov_len
        //%t
        //);
        if (t < 0) THROWERREXCEPTIONHERE();
    }
    trash(t);
    return t;
}


//size_t Circular::write(File &, size_t);

size_t Circular::read(ReadableFile & f)
{
    const size_t beg1 = (_beg + size() - 1) % size();
    ssize_t t = 0;
    if (_end < beg1)
    {
        t = f.read(Segment(_mem, _end, beg1 - _end));
        //LOGGER.debug(Format("Circular read(%$,%$) => %$") %_end %(beg1 - _end) %t);
    }
    else
    {
        struct iovec q[2];
        q[0].iov_base = _mem.ptr() + _end;
        q[0].iov_len = size() - _end;
        q[1].iov_base = _mem.ptr();
        q[1].iov_len = beg1;
        t = ::readv(+f.fd(), q, DIM(q));
        //LOGGER.debug(Format("Circular readv({{%$,%$},{%$,%$}) => %$")
        //%(reinterpret_cast<char *>(q[0].iov_base) - _mem.ptr())
        //%q[0].iov_len
        //%(reinterpret_cast<char *>(q[1].iov_base) - _mem.ptr())
        //%q[1].iov_len
        //%t
        //);
        if (t < 0) THROWERREXCEPTIONHERE();
    }
    _end = (_end + t) % size();
    return t;
}


//size_t Circular::read(File &, size_t);

size_t Circular::push(const ConstSegment & s)
{
    const size_t z = std::min(s.size(), size() - fill() - 1);
    const size_t z1 = std::min(z, size() - _end);
    ::memcpy(_mem.ptr() + _end, s.ptr(), z1);
    if (z > z1) ::memcpy(_mem.ptr(), s.ptr() + z1, z - z1);
    return z;
}

size_t Circular::copy(const Segment & s)
{
    if (_end >= _beg)
    {
        const size_t n = _end - _beg;
        const size_t z = std::min(n, s.size());
        memcpy(s.ptr(), _mem.ptr() + _beg, z);
        return z;
    }

    const size_t ae = size() - _beg;
    const size_t ce = std::min(ae, s.size());
    memcpy(s.ptr(), _mem.cptr() + _beg, ce);
    const size_t cb = std::min(_end, s.size() - ce);
    memcpy(s.ptr() + ce, _mem.cptr(), cb);
    return ce + cb;
}

size_t Circular::pull(const Segment & s)
{
    const size_t z = copy(s);
    trash(z);
    return z;
}

void Circular::trash(size_t z)
{
    if (z > fill())
        THROWINTERFACEEXCEPTION(Format("out of range, fill=%$ req=%$") % fill() % z);

    _beg = (_beg + z) % size();
    if (_beg == _end) _beg = 0, _end = 0; //ottimizzazione
}

char Circular::get()
{
    if (empty()) THROWINTERFACEEXCEPTION("empty");
    const char k = _mem.cptr()[_beg];
    trash(1);
    return k;
}

void Circular::put(char k)
{
    if (full()) THROWINTERFACEEXCEPTION("full");
    _mem.ptr()[_end] = k;
    _end = (_end + 1) % size();
}


// FdSet

#if 0

const int FdSet::E = -1;

FdSet::~FdSet()
{
    ::free(_data);
}

FdSet::FdSet()
: _space(1)
, _lhole(0)
, _upper(0)
, _data(reinterpret_cast<int*> (::malloc(_space * sizeof (int))))
{
    LOOP(j, _space) _data[j] = E;
}

bool FdSet::add(const FileDescriptor & f)
{
    ASSERT(!!f);

    // if already present
    if (is(f)) return false;

    _maxim = std::max(_maxim, f);

    // try use _lhole
    if (_lhole < _upper && _data[_lhole] == E)
    {
        _data[_lhole] = +f;
        return true;
    }

    // try in free upper space if any
    if (_upper < _space)
    {
        _data[_upper] = +f;
        ++_upper;
        return true;
    }

    ASSERT(_upper == _space);
    // search free hole
    for (; _lhole < _upper; ++_lhole)
    {
        if (_data[_lhole] == E)
        {
            _data[_lhole] = +f;
            return true;
        }
    }

    const size_t nc = _space * 2;
    int * p = reinterpret_cast<int*> (realloc(_data, nc * sizeof (int)));
    if (!p)
        THROWOSEXCEPTION(
                         Format("realloc(%$,%$) failed")
                         % ptrtoa(_data)
                         % (nc * sizeof (int))
                         );
    _data = p;

    for (size_t j = _space + 1; j < nc; ++j) _data[j] = E;
    _data[_space] = +f;
    _lhole = _upper = _space + 1;
    _space = nc;
    return true;

}

bool FdSet::del(const FileDescriptor & f)
{
    ASSERT(!!f);

    LOOP(j, _upper)
    {
        if (_data[j] == +f)
        {
            _data[j] = E;
            while (_upper > 0)
            {
                if (_data[_upper - 1] == E) --_upper;
                else break;
            }
            _lhole = std::min(j, _upper);
            return true;
        }
    }
    return false;
}

bool FdSet::is(const FileDescriptor & f) const
{
    ASSERT(!!f);
    if (f > _maxim) return false;
    LOOP(j, _upper) if (_data[j] == +f) return true;
    return false;
}

size_t FdSet::count() const
{
    size_t z = 0;
    LOOP(j, _upper) if (_data[j] != E) ++z;
    return z;
}

bool FdSet::empty() const
{
    return _upper == 0;
}

void FdSet::clear()
{
    _upper = 0, _lhole = 0;
}

FileDescriptor FdSet::operator[](size_t k) const
{
    if (k >= _upper) return FileDescriptor();
    return FileDescriptor(_data[k]);
}

#endif

FdSet::~FdSet()
{
}

bool FdSet::add(const FileDescriptor & x)
{
    CXU_ASSERT(!!x);
    if (is(x)) return false;
    FD_SET(+x, &_set);
    _upper = std::max(_upper, x);
    ++_count;
    return true;
}

bool FdSet::del(const FileDescriptor & x)
{
    CXU_ASSERT(!!x);
    if (!is(x)) return false;
    FD_CLR(+x, &_set);
    --_count;
    return true;
}

bool FdSet::is(const FileDescriptor & x) const
{
    CXU_ASSERT(!!x);
    return FD_ISSET(+x, &_set);
}

FileDescriptor FdSet::next(const FileDescriptor & x) const
{
    for (
            FileDescriptor a = !x ? FileDescriptor(0) : FileDescriptor(+x + 1);
            a <= _upper;
            a = FileDescriptor(+a + 1)
            )
        if (is(a)) return a;

    return FileDescriptor();
}

CXU_NS_END



//.
