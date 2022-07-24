///YAL zeldan

#ifndef CXU_TYPES_HXX
#define CXU_TYPES_HXX


#include "defs.hxx"

CXU_NS_BEGIN


template <class T> class Serialization;
class Entity;
class Exception;
class InterfaceException;
class DataException;


class BOStorage;
class BIStorage;
class TOStorage;
class TIStorage;

class TextOutput;
class TextInput;
class TextOutputFILE;

class Socket;

class LibInfo;
class LibInfoList;

class FileMappingData;
class FileMappingBuffer;

class Logger;
class Test;
class TestSuite;

class CriticalSection;
class CriticalSectionUse;

class RWlock;
class Mutex;
class NRMutex;
class Lock;
class Sync;

class Init;
class Thread;
class MainThread;
class Process;
class ProcessPool;

class Notification;
class Notifier;
class Waiter;

class Time;
class DeltaTime;

class SerialPort;

class File;
class RegularFile;
class SeekableFile;

class InetAddress;
class Inet4Address;
class Inet6Address;
class SockAddress;
class SockDgramClient;

class Crc16;

class AnsiFILE;
class SignalBlocker;
class Pipe;

template <typename T> class ObjSync;
template <typename T> class ObjLock;
template <typename T> class ConstObjLock;

CXU_NS_END

#endif


//.
