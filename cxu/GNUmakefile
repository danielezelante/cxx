# YAL zeldan

PRODUCT := cxu
TARGETS := lib

include ../zmk/defs.mk

COMMONFLAGS += -std=gnu++11
ZMK_LDFLAGS += -Wl,--build-id

ifneq (x86_64-unknown-haiku,$(MACHINE))
 COMMONFLAGS += -pthread
else
 ZMK_LDLIBS += -lpthread
endif

.PHONY: all
all: zmk_all

.PHONY: install
install: zmk_install

.PHONY: install-strip
install-strip: zmk_install-strip

.PHONY: clean
clean: zmk_clean
* rm -rf _*
* rm -f libcxu-*

include ../zmk/build.mk

#.
