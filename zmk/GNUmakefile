# YAL zeldan

.RECIPEPREFIX:=*

PRODUCT := zmk

include ./defs.mk

.PHONY: all
all:
*@echo "Nothing to do. You can type make install."

.PHONY: install
install:
*$(MKDIR) $(DESTDIR)$(includedir)/$(PRODUCT)
*$(INSTALL) -m 0644 defs.mk $(DESTDIR)$(includedir)/$(PRODUCT)
*$(INSTALL) -m 0644 build.mk $(DESTDIR)$(includedir)/$(PRODUCT)
*$(MKDIR) $(DESTDIR)$(docdir)/template
*$(INSTALL) -m 0644 template/GNUmakefile $(DESTDIR)$(docdir)/template
*$(INSTALL) -m 0755 zmk_pkg.sh $(DESTDIR)$(bindir)
*$(INSTALL) -m 0755 zmk_pkgfile.sh $(DESTDIR)$(bindir)


.PHONY: clean
clean:


include ./build.mk

#.
