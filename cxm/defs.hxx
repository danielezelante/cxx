// YAL zeldan

#ifndef CXM_DEFS_HXX
#define CXM_DEFS_HXX

#include <stddef.h>

//** make system dependent macros

#define CXM_CFG_TIMEIDSTR CXU_STRINGIZE(ZMK_TIMEID)
#define CXM_CFG_PRODUCT ZMK_PRODUCT
#define CXM_CFG_PRODUCTSTR CXU_STRINGIZE(ZMK_PRODUCT)
#define CXM_CFG_MAJVERS ZMK_MAJVERS
#define CXM_CFG_MINVERS ZMK_MINVERS
#define CXM_CFG_RELEASE ZMK_RELEASE
#define CXM_CFG_BUILDNR CXU_STRINGIZE(ZMK_REPVER)
#define CXM_CFG_ISDEBUG ZMK_ISDEBUG



//**** CXM defines

#ifdef cxm_DEBUG
#ifdef NDEBUG
#error "cannot use CXM in debug mode for user software in release mode"
#endif
#endif


#ifndef CXM_NS
#define CXM_NS cxm
#endif

#define CXM_NS_BEGIN namespace CXM_NS {
#define CXM_NS_END }


#endif

//.
