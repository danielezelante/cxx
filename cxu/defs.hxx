// YAL zeldan

#ifndef CXU_DEFS_HXX
#define CXU_DEFS_HXX

#include <stddef.h>


// independent macros
#define CXU_APPLY(M,L) M(L)
#define CXU_APPLY2(M,X,Y) M(X,Y)

#define CXU_STR(L) #L
#define CXU_CAT(A,B) A##B

#define CXU_STRINGIZE(L) CXU_APPLY(CXU_STR,L)

#define CXU_XVAR CXU_APPLY2(CXU_CAT,_cxu_,__COUNTER__)


//** make system dependent macros


#define CXU_CFG_TIMEIDSTR CXU_STRINGIZE(ZMK_TIMEID)
#define CXU_CFG_PRODUCT ZMK_PRODUCT
#define CXU_CFG_PRODUCTSTR CXU_STRINGIZE(ZMK_PRODUCT)
#define CXU_CFG_FIXVER CXU_STRINGIZE(ZMK_FIXVER)
#define CXU_CFG_REPVER CXU_STRINGIZE(ZMK_REPVER)
#define CXU_CFG_HASHVER CXU_STRINGIZE(ZMK_HASHVER)
#define CXU_CFG_ISDEBUG ZMK_ISDEBUG




//** endian check
#if (BYTE_ORDER != LITTLE_ENDIAN)
#error little endian supported only
#endif



//** architecture macros

#if !defined(_LP64) 
#define _ILP32
//no other archs by now
#endif





//** system includes

#include <errno.h>
#include <signal.h>



//**** CXU defines

#ifdef cxu_DEBUG
#ifdef NDEBUG
#error "cannot use CXU in debug mode for user software in release mode"
#endif
#endif


#ifndef CXU_NS
#define CXU_NS cxu
#endif

#define CXU_NS_BEGIN namespace CXU_NS {
#define CXU_NS_END }


#define CXU_STACKALLOC(T,n) (T*)_alloca(n*sizeof(T))


#define CXU_EVER for(;;)

#define CXU_STRCLR(s) (s)[0]=0

#define CXU_DIM(x) (sizeof(x)/sizeof((x)[0]))
#define CXU_CALLPTR(p,f) if((p)) p->f
#define CXU_ZERO(x) ::memset(&x, 0, sizeof(x))

#define CXU_LAZYSTR1(F,V) cxu::StringableConst(F(V))


#define CXU_INITMAIN CXU_NS::Init init_(argv[0]);


#define CXU_MAIN(MYMAIN)          \
int main(int argc, char ** argv)        \
{                \
        CXU_NS::LibInfoRegister lir(CXU_CFG_PRODUCT::libinfo()); \
        return CXU_NS::cxumain(argc, argv, MYMAIN);     \
}                \



// use following macros (lowercase function-like) in headers
//#define CXU_throwINTERFACEEXCEPTION(...) throw CXU_NS::InterfaceException(__FUNCTION__,__VA_ARGS__, __FILE__, __LINE__, CXU_NS::stacktrace())
//#define CXU_throwDATAEXCEPTION(...) throw CXU_NS::DataException(__VA_ARGS__, __FILE__, __LINE__, CXU_NS::stacktrace())
//#define CXU_logINTERFACEEXCEPTION(TEXT) {CXU_NS::InterfaceException ex(__FUNCTION__,TEXT,__FILE__,__LINE__,CXU_NS::stacktrace());ex.logerror();}

#define CXU_assertDBG(x) CXU_NS::assertdbg(x, #x, __FILE__, __LINE__)



#ifdef cxu_DEBUG
#define CXU_THROWEXCEPTION(TYPE, ...) throw TYPE(__VA_ARGS__, __FILE__, __LINE__, CXU_NS::stacktrace())
#define CXU_LOGEXCEPTION(LEVEL, TYPE, ...) \
                do {TYPE cxu_ex_(__VA_ARGS__, __FILE__, __LINE__, CXU_NS::stacktrace()); cxu_ex_.log((LEVEL));} while (false)
#define CXU_THROWINTERFACEEXCEPTION(...) throw CXU_NS::InterfaceException(__FUNCTION__, __VA_ARGS__, __FILE__, __LINE__, CXU_NS::stacktrace())
#define CXU_LOGINTERFACEEXCEPTION(LEVEL, ...) \
                do {CXU_NS::InterfaceException cxu_ex_(__FUNCTION__, __VA_ARGS__, __FILE__, __LINE__, CXU_NS::stacktrace()); cxu_ex_.log((LEVEL));} while (false)
#else
#define CXU_THROWEXCEPTION(TYPE, ...) throw TYPE(__VA_ARGS__)
#define CXU_LOGEXCEPTION(LEVEL, TYPE, ...) \
                do {TYPE cxu_ex_(__VA_ARGS__); cxu_ex_.log((LEVEL));} while (false)
#define CXU_THROWINTERFACEEXCEPTION(...) throw CXU_NS::InterfaceException(__FUNCTION__, __VA_ARGS__)
#define CXU_LOGINTERFACEEXCEPTION(LEVEL, ...) do {CXU_NS::InterfaceException cxu_ex_(__FUNCTION__, __VA_ARGS__); cxu_ex_.log((LEVEL));} while (false)
#endif


#define CXU_THROWTEXTEXCEPTION(...) CXU_THROWEXCEPTION(CXU_NS::TextException, __VA_ARGS__)
#define CXU_LOGTEXTEXCEPTION(LEVEL, ...) CXU_LOGEXCEPTION((LEVEL), CXU_NS::TextException, __VA_ARGS__)

#define CXU_THROWDATAEXCEPTION(...) CXU_THROWEXCEPTION(CXU_NS::DataException, __VA_ARGS__)
#define CXU_LOGDATAEXCEPTION(LEVEL, ...) CXU_LOGEXCEPTION((LEVEL), CXU_NS::DataException, __VA_ARGS__)

#define CXU_THROWOSEXCEPTION(...) CXU_THROWEXCEPTION(CXU_NS::OSException, __VA_ARGS__)
#define CXU_LOGOSEXCEPTION(LEVEL, ...) CXU_LOGEXCEPTION((LEVEL), CXU_NS::OSException, __VA_ARGS__)



#define CXU_THROWERREXCEPTION(ERR) CXU_THROWEXCEPTION(CXU_NS::ErrException,(ERR))
#define CXU_THROWERREXCEPTIONS(ERR,S) CXU_THROWEXCEPTION(CXU_NS::ErrException,(ERR), S)
#define CXU_THROWERREXCEPTIONHERE() CXU_THROWEXCEPTION(CXU_NS::ErrException, errno)
#define CXU_THROWERREXCEPTIONHERES(S) CXU_THROWEXCEPTION(CXU_NS::ErrException, errno, S)
#define CXU_LOGERREXCEPTION(LEVEL, ERR) CXU_LOGEXCEPTION((LEVEL), CXU_NS::ErrException, (ERR))
#define CXU_LOGERREXCEPTIONHERE(LEVEL) CXU_LOGEXCEPTION((LEVEL), CXU_NS::ErrException, errno)
#define CXU_LOGERREXCEPTIONHERES(LEVEL,S) CXU_LOGEXCEPTION((LEVEL), CXU_NS::ErrException, errno, S)


#define CXU_ERRCALL(f) \
        do {if (-1==(f)) {CXU_THROWERREXCEPTIONHERE();}} while (false)

#define CXU_ERRCALLS(f,S) \
        do {if (-1==(f)) {CXU_THROWERREXCEPTIONHERES(S);}} while (false)

#define CXU_RETERRCALL(f) \
        do {const int x_ = f; if (x_) {CXU_THROWERREXCEPTION(x_);}} while(false)

#define CXU_RETERRCALLS(f,S) \
        do {const int x_ = f; if (x_) {CXU_THROWERREXCEPTIONS(x_,S);}} while(false)

#ifdef cxu_DEBUG
#define CXU_DEBUGONLY(A) A
#else
#define CXU_DEBUGONLY(A) ((void)0)
#endif



#ifdef cxu_DEBUG
#define CXU_TRACE(x) CXU_NS::tracedbg(x,__FILE__,__LINE__)
#define CXU_VERIFY(x) CXU_NS::assertdbg(x, #x, __FILE__, __LINE__)
#define CXU_ASSERT(x) CXU_NS::assertdbg(x, #x, __FILE__, __LINE__)
#define CXU_TODO CXU_NS::assertdbg(false,"TODO",__FILE__,__LINE__)
#else
#define CXU_TRACE(x) ((void)0)
#define CXU_VERIFY(x) (x)
#define CXU_ASSERT(x) ((void)0)
#define CXU_TODO do {::raise(SIGABRT);} while (false)
#endif

#ifdef _LP64
#define CXU_PRINTF_I64 "l"
#else
#define CXU_PRINTF_I64 "ll"
#endif


#endif

//.
