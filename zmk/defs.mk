# YAL zeldan

# user avaliable targets :

# detect
# zmk_all
# zmk_install
# zmk_install-strip
# zmk_clean
# dist
# backup


# may be defined by user in environment :

# CPPFLAGS
# CFLAGS
# CXXFLAGS
# LDFLAGS
# LDLIBS


# must be defined by user (before of after this file) :

#PRODUCT=productnme
#MAYVERS=number
#MINVERS=number
#RELEASE=number
#TARGETS={app | {alib, solib} }


# may be defined by user (before including this file) :

# DEBUGBUILD={0,1}
# USESTATIC={0,1} for app.mk only
# CROSS to select cross compilation
# prefix where to install
# DISTFILENAMEFILE name where to store tarball name for auto-publish

# ZMK_CPPFLAGS
# ZMK_CFLAGS
# ZMK_CXXFLAGS
# ZMK_LDFLAGS
# ZMK_LDLIBS



# defined by this file and user available (beside standards) :

# OSTYPE build environment
# MACHINE cpu type
# GNUPREFIX string to prefix GNU tools (useful for not GNU native systems)
# TIMEID timestamp in ordered version
# LIBTYPE suffix to identify debug or static versions
# BUILDDIR directory where target are placed
# SHORTCROSS short architecture form derived from CROSS


# ----------------------------------------------------------------

#begin of code

.SUFFIXES:=
.RECIPEPREFIX:=*

# standard directories according to GNU Makefile conventions

DESTDIR ?=

# standard tools

CROSS ?=
MKDIR ?= mkdir -p
MV ?= mv
SYMLINK ?= ln -sfn

## trusco per impostare CC:=gcc ma solo se non e` definita in envirnoment
#CC_SHELL:=$(shell echo -n "$$CC")
#ifeq (,$(CC_SHELL))
# CC := gcc
#endif

#CPP ?= cpp
#CXX ?= g++

# CPP, CC, CXX sono predefiniti da gmake


C_AR := $(CROSS)ar
C_RANLIB := $(CROSS)ranlib
C_STRIP := $(CROSS)strip
C_CPP := $(CROSS)$(CPP)
C_AS := $(CROSS)as
C_CPP := $(CROSS)$(CPP)
C_CC := $(CROSS)$(CC)
C_CXX := $(CROSS)$(CXX)
C_LD := $(CROSS)ld

ZMK_CFLAGS ?=
ZMK_CXXFLAGS ?=
LDCONFIG ?= /sbin/ldconfig


# defaults for user parameters

DEBUGBUILD ?= 0
USESTATIC ?= 0
DISTFILENAMEFILE ?= /dev/null


# begin detection

COMMONFLAGS :=

TIMEID := $(strip $(shell date -u +%Y_%m_%d_%H_%M_%S))
OSTYPE := $(shell uname | sed 's/-.*$$//')
MACHINE := $(shell $(C_CC) $(COMMONFLAGS) $(CFLAGS) $(ZMK_CFLAGS) -dumpmachine)

VER ?= $(shell ../scmrev)
FIXVER ?= $(shell echo $(VER) | sed -r 's/-.*//')
REPVER ?= $(shell echo $(VER) | sed -r 's/[^-]*-//;s/\+.*//')
HASHVER ?= $(shell echo $(VER) | sed -r 's/.*\+//')



ifeq ($(OSTYPE),Linux)
 GNUPREFIX :=
endif

ifeq ($(OSTYPE),FreeBSD)
 GNUPREFIX := g
endif

ifeq ($(OSTYPE),NetBSD)
 GNUPREFIX := g
endif

ifeq ($(OSTYPE),SunOS)
 GNUPREFIX := g
endif




# compute LIBTYPE

ifneq ($(DEBUGBUILD),0)
 LIBTYPE := $(LIBTYPE)-dbg
endif

CBRAND:=$(shell $(CC) --version | head -n 1 | awk '{print $$1}')

BUILDDIR := _$(CROSS)$(CBRAND)_bins$(LIBTYPE)

# prefixed GNU tools

FIND ?= $(GNUPREFIX)find
TAR ?= $(GNUPREFIX)tar
SED ?= $(GNUPREFIX)sed
INSTALL ?= $(GNUPREFIX)install
INSTALL_PROGRAM ?= $(INSTALL)
INSTALL_DATA ?= $(INSTALL) -m 644 

SHORTCROSS := $(shell echo $(CROSS) | $(SED) -r 's/-.*//')
ifneq (,$(SHORTCROSS))
 SHORTCROSS := -$(SHORTCROSS)
endif


prefix ?= /usr/local

ifneq (,$(CROSS))
 LDCONFIG:=
endif

exec_prefix ?= $(prefix)
ifeq (x86_64-pc-linux-gnu,$(findstring x86_64-pc-linux-gnu,$(MACHINE)))
 libdir ?= $(prefix)/lib64
else
 libdir ?= $(prefix)/lib
endif
includedir ?= $(prefix)/include
datarootdir ?= $(prefix)/share
bindir ?= $(exec_prefix)/bin
mandir ?= $(datarootdir)/man
docdir ?= $(datarootdir)/doc/$(PRODUCTV)
sysconfdir ?= $(prefix)/etc

#.
