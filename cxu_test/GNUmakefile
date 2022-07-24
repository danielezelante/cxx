# YAL zeldan

TOTEST := cxu
PRODUCT := $(TOTEST)_test
TARGETS := app


include ../zmk/defs.mk

COMMONFLAGS += -std=gnu++11

ifeq (x86_64-pc-cygwin,$(MACHINE))
 CHECKSTATIC ?= 1
endif

CHECKSTATIC ?= 0

ZMK_CPPFLAGS := -I../

ifneq (x86_64-unknown-haiku,$(MACHINE))
 COMMONFLAGS += -pthread
else
 ZMK_LDLIBS += -lpthread -lnetwork
endif


ifneq ($(DEBUGBUILD),0)
 ZMK_CPPFLAGS += -Dcxu_DEBUG
endif

ifeq ($(CHECKSTATIC),0)
 ZMK_LDFLAGS += -Xlinker -rpath -Xlinker $(CURDIR)/../$(TOTEST)
 ZMK_LDLIBS += ../$(TOTEST)/lib$(TOTEST)$(LIBTYPE).so
else
 ZMK_LDLIBS += ../$(TOTEST)/lib$(TOTEST)-$(VERSION)$(LIBTYPE).a
endif

ifeq ($(OSTYPE),SunOS)
 ZMK_LDLIBS += -lsocket -lnsl
endif


.PHONY: all
all: zmk_all

.PHONY: install
install: zmk_install

.PHONY: clean
clean: zmk_clean

include ../zmk/build.mk

#.
