include Makefile.conf
VERSION:=1.16
RELEASE_DATE:=29 August 2002
PACKAGE:=mkelfImage

# You can specify DESTDIR on the command line to do a add
# a prefix to the install so it doesn't really happen
# Useful for building binary packages
DESTDIR =

pkgdatadir = $(datadir)/$(PACKAGE)
pkglibdir = $(libdir)/$(PACKAGE)
pkgincludedir = $(includedir)/$(PACKAGE)

MANS=mkelfImage.1
SPEC=mkelfImage-$(VERSION).spec
TARBALL=mkelfImage-$(VERSION).tar.gz
SRCS:=$(shell find ./AUTHORS ./COPYING ./Makefile ./Makefile.conf.in ./News \
	./configure.in ./configure \
	./mkelfImage.pl ./mkelfImage.spec.in \
	./elf32-i386 ./kunzip_src ./config \
	! -path '*CVS*' ! -name '*~' ! -name '.*' -type f -print )
SRCS+=$(SPEC)
PSRCS:=$(patsubst ./%,mkelfImage-$(VERSION)/%,$(SRCS))

SBIN_TARGETS=mkelfImage 
MAN1_TARGETS=mkelfImage.1
PKGDATADIR_TARGETS_I386=\
	 elf32-i386/convert_params.c \
	 elf32-i386/elf_boot.h \
	 elf32-i386/linuxbios_tables.h \
	 elf32-i386/elfImage.lds \
	 elf32-i386/head.S \
	 elf32-i386/uniform_boot.h

TARGETS:=$(SBIN_TARGETS) $(MAN1_TARGETS) $(PKGDATADIR_TARGETS_I386)

all: $(TARGETS)

clean:
	@rm -rf dist
	@rm -f config.log config.status config.cache 
	@rm -f $(SBIN_TARGETS) $(MAN1_TARGETS)
	@rm -f mkelfImage-$(VERSION) $(TARBALL)

dist-clean: clean
	@find . -type f -name '*~' | xargs rm -f
	@rm Makefile.conf

maintainer-clean: dist-clean
	@rm -f $(SPEC) configure


install: $(TARGETS)
	mkdir -p $(DESTDIR)/$(pkgdatadir)/elf32-i386/ $(DESTDIR)/$(sbindir) $(DESTDIR)/$(mandir)/man1
	cp -ar $(SBIN_TARGETS) $(DESTDIR)/$(sbindir)/
	cp -ar $(MAN1_TARGETS) $(DESTDIR)/$(mandir)/man1/
	cp -ar $(PKGDATADIR_TARGETS_I386) $(DESTDIR)/$(pkgdatadir)/elf32-i386/


%.1 : %.pl Makefile
	mkdir -p $(@D)
	pod2man --date="$(RELEASE_DATE)" --release="$(VERSION)" $*.pl > $@

mkelfImage:  mkelfImage.pl Makefile Makefile.conf
	mkdir -p $(@D)
	sed \
		-e 's,^#!/usr/bin/perl,#!$(PERL),' \
		-e 's,^my $$VERSION="";$$,my $$VERSION="$(VERSION)";,' \
		-e 's,^$$params{MYDATA}=".";$$,$$params{MYDATA}="$(pkgdatadir)";,' \
		-e 's,^$$params{OBJCOPY}="objcopy";$$,$$params{OBJCOPY}="$(OBJCOPY)";,' \
		-e 's,^$$params{LD}="ld";$$,$$params{LD}="$(LD)";,' \
		-e 's,^$$params{CC}="cc";$$,$$params{CC}="$(CC)";,' \
		mkelfImage.pl > $@
	chmod a+x $@

tarball: $(TARBALL)


$(TARBALL): $(SRCS)
	rm -f ./mkelfImage-$(VERSION)
	ln -s . ./mkelfImage-$(VERSION)
	tar -cf - $(PSRCS) | gzip -9 > $@

Makefile.conf: Makefile.conf.in configure
	./configure


configure: configure.in
	autoconf

$(SPEC): mkelfImage.spec.in Makefile
	sed \
		-e 's,^Version: $$,Version: $(VERSION),' \
		mkelfImage.spec.in > $@



.PHONY: echo install realinstall


echo:
	echo $(SRCS)
