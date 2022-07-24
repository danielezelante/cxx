# YAL zeldan

.SUFFIXES:=
.RECIPEPREFIX:=*

MYDIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

PKGMK := _pkg.mk
-include $(PKGMK)

# warning selection 

WARNINGS := \
 -Wall \
 -Wpointer-arith \
 -Wcast-qual \
 -Wcast-align \
 -Wwrite-strings \
 -Wsign-compare \
 -Wfloat-equal \
 -Wendif-labels \
 -Wshadow \
 -Wextra


WARNINGSC := $(WARNINGS)

WARNINGSCXX := $(WARNINGS) \
 -Woverloaded-virtual \
 -Wsynth \
 -Wold-style-cast


INSTALL_TARGETS := $(addprefix install_, $(TARGETS))
INSTALL_STRIP_TARGETS := $(addprefix install-strip_, $(TARGETS))
CLEAN_TARGETS := $(addprefix clean_, $(TARGETS))

ifneq (x86_64-pc-cygwin,$(MACHINE))
 DPIC := -DPIC
 FPIC := -fPIC
else
 DPIC:=
 FPIC:=
endif

# sources and headers discovery

HEADERS :=
HEADERS += $(shell $(FIND) ./ -maxdepth 1 -name '*.h')
HEADERS += $(shell $(FIND) ./ -maxdepth 1 -name '*.hh')
HEADERS += $(shell $(FIND) ./ -maxdepth 1 -name '*.hxx')
HEADERS += $(shell $(FIND) ./ -maxdepth 1 -name '*.hpp')

SOURCESC := $(shell $(FIND) ./ -maxdepth 1 -name '*.c' -exec basename {} \; | sort)
SOURCESCC := $(shell $(FIND) ./ -maxdepth 1 -name '*.cc' -exec basename {} \; | sort)
SOURCESCXX := $(shell $(FIND) ./ -maxdepth 1 -name '*.cxx' -exec basename {} \; | sort)
SOURCESCPP := $(shell $(FIND) ./ -maxdepth 1 -name '*.cpp' -exec basename {} \; | sort)
SOURCES := $(SOURCESC) $(SOURCESCC) $(SOURCESCXX) $(SOURCESCPP)



OBJECTS :=
OBJECTS += $(addprefix $(BUILDDIR)/,$(notdir $(SOURCESC:.c=.o)))
OBJECTS += $(addprefix $(BUILDDIR)/,$(notdir $(SOURCESCC:.cc=.o)))
OBJECTS += $(addprefix $(BUILDDIR)/,$(notdir $(SOURCESCXX:.cxx=.o)))
OBJECTS += $(addprefix $(BUILDDIR)/,$(notdir $(SOURCESCPP:.cpp=.o)))


LOBJECTS :=
LOBJECTS += $(addprefix $(BUILDDIR)/,$(notdir $(SOURCESC:.c=.os)))
LOBJECTS += $(addprefix $(BUILDDIR)/,$(notdir $(SOURCESCC:.cc=.os)))
LOBJECTS += $(addprefix $(BUILDDIR)/,$(notdir $(SOURCESCXX:.cxx=.os)))
LOBJECTS += $(addprefix $(BUILDDIR)/,$(notdir $(SOURCESCPP:.cpp=.os)))


# prepare dependencies file
DEPENDS := _$(CROSS)$(CBRAND)_depends$(LIBTYPE)


# preprocessor zmk

MZCPPFLAGS := -DZMK
MZCPPFLAGS += -DZMK_BUILDING_$(PRODUCT)
MZCPPFLAGS += -DZMK_PRODUCT=$(PRODUCT)
MZCPPFLAGS += -DZMK_FIXVER=$(FIXVER)
MZCPPFLAGS += -DZMK_REPVER=$(REPVER)
MZCPPFLAGS += -DZMK_HASHVER=$(HASHVER)
MZCPPFLAGS += -DZMK_TIMEID=$(TIMEID)


ifneq ($(USESTATIC),0)
 STATIC := -static
else
 STATIC :=
endif

ifneq ($(DEBUGBUILD),0)
 MZCPPFLAGS += -DZMK_DEBUG -DZMK_ISDEBUG=1 -D$(PRODUCT)_DEBUG
else
 MZCPPFLAGS += -DZMK_ISDEBUG=0
endif

MZCPPFLAGS += -D_REENTRANT -D_FILE_OFFSET_BITS=64 -D__STDC_LIMIT_MACROS -D_GNU_SOURCE

ifeq (gcc,$(CBRAND))
 MZCPPFLAGS += -D_GLIBCXX_USE_C99 -D_GLIBCXX_USE_C99_MATH 
endif


# system definitions

COMMONFLAGS += -pipe

ifeq ($(DEBUGBUILD),0)
 MZCPPFLAGS += -DNDEBUG
 COMMONFLAGS += -O
else
 MZCPPFLAGS += -D_GLIBCXX_CONCEPT_CHECKS -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC
 COMMONFLAGS += -ggdb
 LDFLAGS += -rdynamic
endif


# myflags composition

MYCPPFLAGS := $(MZCPPFLAGS) $(ZMK_CPPFLAGS) $(CPPFLAGS)
MYCFLAGS := $(WARNINGSC) $(COMMONFLAGS) $(ZMK_CFLAGS) $(CFLAGS)
MYCXXFLAGS := $(WARNINGSCXX) $(COMMONFLAGS) $(ZMK_CXXFLAGS) $(CXXFLAGS)
MYLDFLAGS := $(WARNINGSCXX) $(COMMONFLAGS) $(ZMK_LDFLAGS) $(LDFLAGS)


# product full name composition

PRODUCTV := $(PRODUCT)-$(FIXVER)
PRODUCTVB := $(PRODUCTV)-$(REPVER)


APPLICATION := $(CROSS)$(PRODUCT)$(LIBTYPE)
APPLICATIONV := $(CROSS)$(PRODUCTV)$(LIBTYPE)




# default compile rules

$(BUILDDIR)/%.o : %.cxx
*   $(C_CXX) -c $(MYCPPFLAGS) $(MYCXXFLAGS) $< -o $@

$(BUILDDIR)/%.o : %.cpp
*   $(C_CXX) -c $(MYCPPFLAGS) $(MYCXXFLAGS) $< -o $@

$(BUILDDIR)/%.o : %.cc
*   $(C_CXX) -c $(MYCPPFLAGS) $(MYCXXFLAGS) $< -o $@

$(BUILDDIR)/%.o : %.c
*   $(C_CC) -c $(MYCPPFLAGS) $(MYCFLAGS) $< -o $@

$(BUILDDIR)/%.os : %.cxx
*   $(C_CXX) -c $(DPIC) $(FPIC) $(MYCPPFLAGS) $(MYCXXFLAGS) $< -o $@

$(BUILDDIR)/%.os : %.cpp
* $(C_CXX) -c $(DPIC) $(FPIC) $(MYCPPFLAGS) $(MYCXXFLAGS) $< -o $@

$(BUILDDIR)/%.os : %.cc
*   $(C_CXX) -c $(DPIC) $(FPIC) $(MYCPPFLAGS) $(MYCXXFLAGS) $< -o $@

$(BUILDDIR)/%.os : %.c
*   $(C_CC) -c $(DPIC) $(FPIC) $(MYCPPFLAGS) $(MYCFLAGS) $< -o $@




.PHONY: zmk_all
zmk_all:
*   $(MAKE) pkgconfig
*   $(MAKE) depend
*   $(MAKE) binary
	


.PHONY: init
init:

ifeq ($(PRODUCT),)
*   $(error PRODUCT must be defined)
endif
ifeq ($(FIXVER),)
*   $(error FIXVER must be defined)
endif
ifeq ($(REPVER),)
*   $(error REPVER must be defined)
endif
ifeq ($(HASHVER),)
*   $(error HASHVER must be defined)
endif

ifeq ($(strip $(SOURCESCXX) $(SOURCESCPP) $(SOURCESCC)),)
*   $(shell [ -e "model/$(PRODUCT).c" ] && cp "model/$(PRODUCT).c" "_$(PRODUCT).c")
*   -touch -c "_$(PRODUCT).c"
else
*   $(shell [ -e "model/$(PRODUCT).cxx" ] && cp "model/$(PRODUCT).cxx" "_$(PRODUCT).cxx")
*   -touch -c "_$(PRODUCT).cxx"
endif
*   $(MKDIR) $(BUILDDIR)


.PHONY: pkgconfig
pkgconfig:
*   ${MYDIR}/zmk_pkgfile.sh "pkg.cfg" $(PKGMK)

depend: init $(SOURCES) $(HEADERS)
ifeq ($(strip $(SOURCESCXX) $(SOURCESCPP) $(SOURCESCC)),)
*   $(C_CC) -DZMK_DEPENDS -MM $(MYCPPFLAGS) $(MYCFLAGS) $(SOURCES) | $(SED) -r '/^[^ ]+\.o/s/^/$(BUILDDIR)\//' > $(DEPENDS)_o
else
*   $(C_CXX) -DZMK_DEPENDS -MM $(MYCPPFLAGS) $(MYCXXFLAGS) $(SOURCES) | $(SED) -r '/^[^ ]+\.o/s/^/$(BUILDDIR)\//' > $(DEPENDS)_o
endif
*   $(SED) -r 's/.o:/.oz:/g' < $(DEPENDS)_o > $(DEPENDS)_os
*   cat $(DEPENDS)_o $(DEPENDS)_os > $(DEPENDS)

-include $(DEPENDS)

binary: $(TARGETS)


# app

app: $(APPLICATIONV)

$(APPLICATIONV) : $(OBJECTS)
ifeq ($(strip $(SOURCESCXX) $(SOURCESCPP) $(SOURCESCC)),)
*   $(C_CC) $(STATIC) $(MYLDFLAGS) -o $@ $^ $(ZMK_LDLIBS) $(LDLIBS)
else
*   $(C_CXX) $(STATIC) $(MYLDFLAGS) -o $@ $^ $(ZMK_LDLIBS) $(LDLIBS)
endif
*   $(SYMLINK) $(APPLICATIONV) $(APPLICATION)

install_app: app
*   $(NORMAL_INSTALL)	
*   $(MKDIR) $(DESTDIR)$(bindir)
*   $(INSTALL_PROGRAM) $(APPLICATIONV) $(DESTDIR)$(bindir)/
*   $(POST_INSTALL)	
*   cd $(DESTDIR)$(bindir); $(SYMLINK) $(APPLICATIONV) $(APPLICATION)


.PHONY: install-strip_app
install-strip_app: install_app


# alib

ALIBRARY := lib$(PRODUCT)$(LIBTYPE)$(SHORTCROSS)
ANAME := $(ALIBRARY).a

ALIBRARYV := lib$(PRODUCTV)$(LIBTYPE)$(SHORTCROSS)
ANAMEV := $(ALIBRARYV).a



alib: $(ANAMEV)


$(ANAMEV): $(OBJECTS)
*   $(RM) $@
*   $(C_AR) qc $@ $^
*   $(C_RANLIB) $@
*   $(SYMLINK) $(ANAMEV) $(ANAME)


.PHONY: install-strip_alib
install-strip_alib:

install_alib: $(ANAMEV) install_headers
*   $(NORMAL_INSTALL)
*   $(MKDIR) $(DESTDIR)$(libdir)
*   $(INSTALL_DATA) $(ANAMEV) $(DESTDIR)$(libdir)/
*   $(POST_INSTALL)	
*   cd $(DESTDIR)$(libdir); $(SYMLINK) $(ANAMEV) $(ANAME)



SOLIBRARY := lib$(PRODUCT)$(LIBTYPE)$(SHORTCROSS)

SONAME := $(SOLIBRARY).so
SONAMEV := $(SONAME).$(FIXVER)
SONAMEVB := $(SONAMEV)-$(REPVER)


solib: $(SONAMEVB)


$(SONAMEVB) : $(LOBJECTS)
ifeq ($(strip $(SOURCESCXX) $(SOURCESCPP) $(SOURCESCC)),)
*   $(C_CC) -shared $(FPIC) $(MYLDFLAGS) -o $@ -Xlinker -soname=$(SONAMEV) $^ $(ZMK_LDLIBS) $(LDLIBS)
else
*   $(C_CXX) -shared $(FPIC) $(MYLDFLAGS) -o $@ -Xlinker -soname=$(SONAMEV) $^ $(ZMK_LDLIBS) $(LDLIBS)
endif
*   $(SYMLINK) $(SONAMEVB) $(SONAMEV); $(SYMLINK) $(SONAMEVB) $(SONAME)
	

install_solib: $(SONAMEVB)
*   $(NORMAL_INSTALL)	
*   $(MKDIR) $(DESTDIR)$(libdir)
*   $(INSTALL_PROGRAM) $(SONAMEVB) $(DESTDIR)$(libdir)/
*   $(POST_INSTALL)	
*   cd $(DESTDIR)$(libdir); $(SYMLINK) $(SONAMEVB) $(SONAMEV)
*   cd $(DESTDIR)$(libdir); $(SYMLINK) $(SONAMEVB) $(SONAME)
*   $(LDCONFIG)

.PHONY: install-strip-solib
install-strip_solib: install_solib


lib: alib solib

install_lib: install_alib install_solib

.PHONY: install-strip_lib
install-strip_lib: install-strip_alib install-strip_solib


# common targets

.PHONY: zmk_install
zmk_install: $(INSTALL_TARGETS)

.PHONY: internal_install-strip
internal_install-strip: $(INSTALL_STRIP_TARGETS)



install_headers: $(HEADERS)
*   $(NORMAL_INSTALL)
*   $(MKDIR) $(DESTDIR)$(includedir)/$(PRODUCTV)
*   $(INSTALL_DATA) $(HEADERS) $(DESTDIR)$(includedir)/$(PRODUCTV)
*   $(POST_INSTALL)	
*   cd $(DESTDIR)$(includedir); $(SYMLINK) $(PRODUCTV) $(PRODUCT)



.PHONY: zmk_install-strip
zmk_install-strip:
*   $(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s --strip-program=$(C_STRIP)' internal_install-strip


.PHONY: backup
backup: clean
*   $(RM) -r _tmp
*   $(MKDIR) _tmp/$(PRODUCTVB)
*   $(TAR) --exclude '.*' --exclude '_*' -c -f - * | $(TAR) -x -f - -C _tmp/$(PRODUCTVB)
*   cd _tmp; $(TAR) --owner 0 --group 0 -zcf ../../$(PRODUCTVB)-$(TIMEID).tar.gz $(PRODUCTVB)
*   $(RM) -r _tmp


.PHONY: dist
dist: clean
*   $(RM) -r _tmp
*   $(MKDIR) _tmp/$(PRODUCTVB)
*   cd _tmp; ln -sf $(PRODUCTVB) $(PRODUCTV)
*   cd _tmp; ln -sf $(PRODUCTVB) $(PRODUCT)
*   $(TAR) --exclude '.*' --exclude '_*' -c -f - * | $(TAR) -x -f - -C _tmp/$(PRODUCTVB)
*   cd _tmp; $(TAR) --owner 0 --group 0 -zcf ../../$(PRODUCTVB).tar.gz .
*   $(RM) -r _tmp
*   echo $(PRODUCTVB).tar.gz > $(DISTFILENAMEFILE)


.PHONY: zmk_clean
zmk_clean:
*   $(RM) -r ./_docs/
*   $(RM) -r ./$(BUILDDIR)/
*   $(RM) $(PKGMK)
*   $(RM) $(DEPENDS) $(DEPENDS)_o $(DEPENDS)_os
*   $(RM) $(APPLICATION) $(APPLICATIONV)
*   $(RM) $(SONAME) $(SONAME).*
*   $(RM) $(ANAME) $(ANAMEV)
*   $(RM) -f "_$(PRODUCT).c"
*   $(RM) -f "_$(PRODUCT).cxx"


doc: $(HEADERS)
*   doxygen Doxyfile


doc_install: doc
*   $(MKDIR) $(mandir)
*   cp -Rv doc/man/man* $(mandir)/
*   $(MKDIR) $(docdir)
*   cp -Rv doc/html $(docdir)/


.PHONY: detect
detect:
*   @echo "TIMEID=$(TIMEID)"
*   @echo "FIXVER=$(FIXVER)"
*   @echo "REPVER=$(REPVER)"
*   @echo "DEBUGBUILD=$(DEBUGBUILD)"
*   @echo "CROSS=$(CROSS)"
*   @echo "SHORTCROSS=$(SHORTCROSS)"
*   @echo "OSTYPE=$(OSTYPE)"
*   @echo "MACHINE='$(MACHINE)'"
*   @echo "LIBTYPE=$(LIBTYPE)"
*   @echo "USESTATIC=$(USESTATIC)"
*   @echo "C_CPP=$(C_CPP)"
*   @echo "C_CC=$(C_CC)"
*   @echo "C_CXX=$(C_CXX)"
*   @echo "CBRAND=$(CBRAND)"
*   @echo "CPPFLAGS=$(CPPFLAGS)"
*   @echo "CFLAGS=$(CFLAGS)"
*   @echo "LDFLAGS=$(LDFLAGS)"
*   @echo "which C_CC=$(shell which $(C_CC))"
*   @echo "which C_CXX=$(shell which $(C_CXX))"
*   @echo "env CC=$(CC_SHELL)"
*   @echo "libdir=$(libdir)"


.PHONY: FORCE
FORCE:

#.
