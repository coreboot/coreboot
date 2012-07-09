include Makefile.conf
VERSION:=2.7
RELEASE_DATE:=27 March 2006
PACKAGE:=mkelfImage

# You can specify DESTDIR on the command line to do a add
# a prefix to the install so it doesn't really happen
# Useful for building binary packages
DESTDIR =
CWD:=$(shell pwd)
OBJDIR:=objdir

CPPFLAGS=-DVERSION='"$(VERSION)"' -DRELEASE_DATE='"$(RELEASE_DATE)"' -I include
HOST_CPPFLAGS=$(CPPFLAGS)
I386_CPPFLAGS=$(CPPFLAGS) -I arch/i386/include
IA64_CPPFLAGS=$(CPPFLAGS) -I arch/ia64/include

GENERATED_SRCS:=\
	./configure \
	./linux-i386/convert.bin.c\
	./linux-ia64/convert.bin.c

SPEC=$(OBJDIR)/mkelfImage-$(VERSION).spec
TARBALL=$(OBJDIR)/mkelfImage-$(VERSION).tar.gz
SRCS:=$(shell find ./AUTHORS ./COPYING ./Makefile ./Makefile.conf.in ./News \
	./configure.ac \
	./mkelfImage.spec.in \
	./config \
	./kunzip_src \
	./main ./util ./include ./arch \
	./linux-i386 \
	./linux-ia64 \
	! -path '*CVS*' ! -name '*~' ! -name '.*' -type f -print )
SRCS+=$(GENERATED_SRCS)
PSRCS:=$(patsubst ./%,mkelfImage-$(VERSION)/%,$(SRCS))
PSRCS+=./mkelfImage-$(VERSION).spec

SBIN_TARGETS=$(OBJDIR)/sbin/mkelfImage
MAN8_TARGETS=$(OBJDIR)/man/man8/mkelfImage.8

TARGETS:=$(SBIN_TARGETS) $(MAN8_TARGETS)

DEPS:= Makefile Makefile.conf
all: $(TARGETS)

include main/Makefile
include util/Makefile
include linux-i386/Makefile
include linux-ia64/Makefile


clean::
	@$(RM) -rf objdir
	@$(RM) -rf rpm
	@$(RM) -f config.log config.status config.cache
	@$(RM) -f $(SBIN_TARGETS) $(MAN1_TARGETS)
	@$(RM) -f mkelfImage-$(VERSION) $(TARBALL)

dist-clean:: clean
	@$(FIND) . -type f -name '*~' | xargs $(RM) -f
	@$(RM) Makefile.conf

maintainer-clean:: dist-clean
	@$(RM) -f $(SPEC) $(GENERATED_SRCS)


install: $(TARGETS)
	$(MKDIR) -p $(DESTDIR)/$(sbindir) $(DESTDIR)/$(mandir)/man8
	$(CP) -ar $(SBIN_TARGETS) $(DESTDIR)/$(sbindir)/
	$(CP) -ar $(MAN8_TARGETS) $(DESTDIR)/$(mandir)/man8/


#%.1 : %.pl Makefile
#	mkdir -p $(@D)
#	pod2man --date="$(RELEASE_DATE)" --release="$(VERSION)" $*.pl > $@


tarball: $(TARBALL)


$(TARBALL): $(SRCS) $(SPEC)
	$(MKDIR) -p $(OBJDIR)
	$(RM) -f $(OBJDIR)/mkelfImage-$(VERSION)
	$(LN) -s .. $(OBJDIR)/mkelfImage-$(VERSION)
	(cd $(OBJDIR); $(TAR) -cf - $(PSRCS) | gzip -9) > $@

rpm: $(TARBALL)
	$(MKDIR) -p $(OBJDIR)/RPM $(OBJDIR)/SRPM $(OBJDIR)/BUILD $(OBJDIR)/SPECS $(OBJDIR)/TMP
	unset MAKEFLAGS MAKELEVEL; \
	$(RPM) -ta \
		--define '_rpmdir $(CWD)/$(OBJDIR)/RPM' \
		--define '_srcrpmdir $(CWD)/$(OBJDIR)/SRPM' \
		--define '_builddir $(CWD)/$(OBJDIR)/BUILD' \
		--define '_specdir $(CWD)/$(OBJDIR)/SPECS' \
		--define '_tmppath $(CWD)/$(OBJDIR)/TMP' \
		$(TARBALL)

Makefile.conf: Makefile.conf.in configure
	/bin/sh ./configure


configure: configure.ac
	autoconf
	$(RM) -rf autom4te.cache

$(SPEC): mkelfImage.spec.in Makefile
	$(SED) -e 's,^Version: $$,Version: $(VERSION),' $< > $@


.PHONY: echo install realinstall


echo:
	echo $(SRCS)
