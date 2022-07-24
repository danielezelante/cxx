# YAL zeldan

PRODUCT := cxm
TARGETS := lib

include ../zmk/defs.mk

COMMONFLAGS += -std=gnu++11
CPPFLAGS += -I..
LDFLAGS += -L../cxu


ifneq (x86_64-unknown-haiku,$(MACHINE))
 COMMONFLAGS += -pthread
else
 ZMK_LDLIBS += -lcxu -lpthread
endif

.PHONY: all
all: zmk_all

.PHONY: install
install: zmk_install

.PHONY: install-strip
install-strip: zmk_install-strip

.PHONY: clean
clean: zmk_clean


include ../zmk/build.mk

#.
