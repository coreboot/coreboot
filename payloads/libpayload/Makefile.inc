##
## This file is part of the libpayload project.
##
## Copyright (C) 2008 Advanced Micro Devices, Inc.
## Copyright (C) 2008 Uwe Hermann <uwe@hermann-uwe.de>
## Copyright (C) 2011 secunet Security Networks AG
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
## 1. Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer.
## 2. Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in the
##    documentation and/or other materials provided with the distribution.
## 3. The name of the author may not be used to endorse or promote products
##    derived from this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
## ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
## ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
## FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
## DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
## OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
## HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
## LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
## OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
## SUCH DAMAGE.
##

export KERNELVERSION      := 0.2.0

ARCHDIR-$(CONFIG_ARCH_ARMV7)   := armv7
ARCHDIR-$(CONFIG_ARCH_X86)     := x86
ARCHDIR-$(CONFIG_ARCH_POWERPC) := powerpc
DESTDIR ?= install

real-target: lib

classes-$(CONFIG_PCI) += libpci
classes-$(CONFIG_LIBC) += libc
classes-$(CONFIG_CURSES) += libcurses
classes-$(CONFIG_PDCURSES) += libmenu libform libpanel
classes-$(CONFIG_CBFS) += libcbfs
classes-$(CONFIG_LZMA) += liblzma
libraries := $(classes-y)
classes-y += head.o

subdirs-y := arch/$(ARCHDIR-y)
subdirs-y += crypto libc drivers libpci
subdirs-$(CONFIG_CURSES) += curses
subdirs-$(CONFIG_CBFS) += libcbfs
subdirs-$(CONFIG_LZMA) += liblzma

INCLUDES := -Iinclude -Iinclude/$(ARCHDIR-y) -I$(obj)
CFLAGS =  $(EXTRA_CFLAGS) $(INCLUDES) -Os -pipe -nostdinc
CFLAGS += -nostdlib -Wall -Wundef -Wstrict-prototypes -Wmissing-prototypes
CFLAGS += -Wwrite-strings -Wredundant-decls -Wno-trigraphs
CFLAGS += -Wstrict-aliasing -Wshadow -fno-builtin

$(obj)/libpayload-config.h: $(KCONFIG_AUTOHEADER)
	cmp $@ $< 2>/dev/null || cp $< $@

library-targets = $(addsuffix .a,$(addprefix $(obj)/,$(libraries))) $(obj)/libpayload.a
lib: $$(library-targets) $(obj)/head.o

extract_nth=$(word $(1), $(subst |, ,$(2)))

#######################################################################
# Add handler for special include files 
$(call add-special-class,includes)
includes-handler= \
		$(if $(wildcard $(1)$(call extract_nth,1,$(2))), \
			$(eval includes += $(1)$(2)))

$(obj)/libpayload.a: $(foreach class,$(libraries),$$($(class)-objs))
	printf "    AR         $(subst $(shell pwd)/,,$(@))\n"
	$(AR) rc $@ $^

$(obj)/%.a: $$(%-objs)
	printf "    AR         $(subst $(shell pwd)/,,$(@))\n"
	$(AR) rc $@ $^

$(obj)/head.o: $(obj)/arch/$(ARCHDIR-y)/head.head.o.o
	printf "    CP         $(subst $(shell pwd)/,,$(@))\n"
	cp $^ $@

install: real-target
	printf "    INSTALL    $(DESTDIR)/libpayload/lib\n"
	install -m 755 -d $(DESTDIR)/libpayload/lib
	for lib in $(library-targets); do \
		install -m 644 $$lib $(DESTDIR)/libpayload/lib/; \
	done
	install -m 644 arch/$(ARCHDIR-y)/libpayload.ldscript $(DESTDIR)/libpayload/lib/
	install -m 755 -d $(DESTDIR)/libpayload/lib/$(ARCHDIR-y)
	install -m 644 $(obj)/head.o $(DESTDIR)/libpayload/lib/$(ARCHDIR-y)
	printf "    INSTALL    $(DESTDIR)/libpayload/include\n"
	install -m 755 -d $(DESTDIR)/libpayload/include
	for file in `find include -name *.h -type f`; do \
		install -m 755 -d $(DESTDIR)/libpayload/`dirname $$file`; \
		install -m 644 $$file $(DESTDIR)/libpayload/$$file; \
	done
	install -m 644 $(obj)/libpayload-config.h $(DESTDIR)/libpayload/include
	$(foreach item,$(includes), \
		install -m 755 -d $(DESTDIR)/libpayload/include/$(call extract_nth,2,$(item)); \
		install -m 644 $(call extract_nth,1,$(item)) $(DESTDIR)/libpayload/include/$(call extract_nth,2,$(item)); )
	printf "    INSTALL    $(DESTDIR)/libpayload/bin\n"
	install -m 755 -d $(DESTDIR)/libpayload/bin
	install -m 755 bin/lpgcc $(DESTDIR)/libpayload/bin
	install -m 755 bin/lpas $(DESTDIR)/libpayload/bin
	install -m 644 bin/lp.functions $(DESTDIR)/libpayload/bin
	install -m 644 $(DOTCONFIG) $(DESTDIR)/libpayload/libpayload.config

clean-for-update-target:
	rm -f $(addsuffix .a,$(addprefix $(obj)/,$(libraries))) $(obj)/libpayload.a

clean-target:
prepare:
