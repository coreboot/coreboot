##
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

ifneq ($(NOCOMPILE),1)
GIT:=$(shell git -C "$(top)" rev-parse --git-dir 1>/dev/null 2>&1 \
	&& command -v git)
else
GIT:=
endif

export KERNELVERSION      := 0.2.0

ARCHDIR-$(CONFIG_LP_ARCH_ARM)     := arm
ARCHDIR-$(CONFIG_LP_ARCH_ARM64)   := arm64
ARCHDIR-$(CONFIG_LP_ARCH_X86)     := x86
ARCHDIR-$(CONFIG_LP_ARCH_MOCK)    := mock
DESTDIR ?= install

real-target: lib

classes-$(CONFIG_LP_PCI) += libpci
classes-$(CONFIG_LP_LIBC) += libc
classes-$(CONFIG_LP_CURSES) += libcurses
classes-$(CONFIG_LP_PDCURSES) += libmenu libform libpanel
classes-$(CONFIG_LP_CBFS) += libcbfs
classes-$(CONFIG_LP_LZMA) += liblzma
classes-$(CONFIG_LP_LZ4) += liblz4
classes-$(CONFIG_LP_REMOTEGDB) += libgdb
classes-$(CONFIG_LP_VBOOT_LIB) += vboot_fw
classes-$(CONFIG_LP_VBOOT_LIB) += tlcl
libraries := $(classes-y)

subdirs-y := arch/$(ARCHDIR-y)
subdirs-y += crypto libc drivers libpci gdb
subdirs-$(CONFIG_LP_CURSES) += curses
subdirs-$(CONFIG_LP_CBFS) += libcbfs
subdirs-$(CONFIG_LP_LZMA) += liblzma
subdirs-$(CONFIG_LP_LZ4) += liblz4
subdirs-$(CONFIG_LP_VBOOT_LIB) += vboot

INCLUDES := -Iinclude -Iinclude/$(ARCHDIR-y) -I$(obj)
INCLUDES += -include include/kconfig.h
INCLUDES += -include $(coreboottop)/src/commonlib/bsd/include/commonlib/bsd/compiler.h
INCLUDES += -I$(coreboottop)/src/commonlib/bsd/include
ifeq ($(CONFIG_LP_GPL),y)
INCLUDES += -I$(coreboottop)/src/commonlib/include
endif
INCLUDES += -I$(VBOOT_SOURCE)/firmware/include

CFLAGS += $(INCLUDES) -Os -pipe -nostdinc -ggdb3
CFLAGS += -nostdlib -fno-builtin -ffreestanding -fomit-frame-pointer
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -Wall -Wundef -Wstrict-prototypes -Wmissing-prototypes -Wvla
CFLAGS += -Wwrite-strings -Wredundant-decls -Wno-trigraphs -Wimplicit-fallthrough
CFLAGS += -Wstrict-aliasing -Wshadow -Werror

ifeq ($(CONFIG_LP_LTO),y)
CFLAGS += -flto
endif

# Some of the commonlib cbfs headers include vboot headers, so initialize the
# submodule in case we are building a payload outside the main coreboot build
forgetthis:=$(if $(GIT),$(shell git submodule update --init ../../3rdparty/vboot $(quiet_errors)))

$(obj)/libpayload.config: $(DOTCONFIG)
	cp $< $@

$(obj)/libpayload-config.h: $(KCONFIG_AUTOHEADER) $(obj)/libpayload.config
	cmp $@ $< 2>/dev/null || cp $< $@

library-targets = $(addsuffix .a,$(addprefix $(obj)/,$(libraries))) $(obj)/libpayload.a
lib: $$(library-targets) $(obj)/libpayload.ldscript

extract_nth=$(word $(1), $(subst |, ,$(2)))

#######################################################################
# Add handler for special include files
$(call add-special-class,includes)
includes-handler= \
		$(if $(wildcard $(1)$(call extract_nth,1,$(2))), \
			$(eval includes += $(1)$(2)))

$(obj)/libpayload.a: $(foreach class,$(libraries),$$($(class)-objs))
	printf "    AR         $(subst $(CURDIR)/,,$(@))\n"
	printf "create $@\n$(foreach objc,$(filter-out %.a,$^),addmod $(objc)\n)$(foreach lib,$(filter %.a,$^),addlib $(lib)\n)save\nend\n" | $(AR) -M

$(obj)/%.a: $$(%-objs)
	printf "    AR         $(subst $(CURDIR)/,,$(@))\n"
	printf "create $@\n$(foreach objc,$(filter-out %.a,$^),addmod $(objc)\n)$(foreach lib,$(filter %.a,$^),addlib $(lib)\n)save\nend\n" | $(AR) -M

$(obj)/libpayload.ldscript: arch/$(ARCHDIR-y)/libpayload.ldscript $(obj)/libpayload-config.h
	@printf "  LDSCRIPT  $@\n"
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -E -P -x assembler-with-cpp -undef -o $@ $<

install: real-target
	printf "    INSTALL    $(DESTDIR)/libpayload/lib\n"
	install -m 755 -d $(DESTDIR)/libpayload/lib
	install -m 644 $(library-targets) $(DESTDIR)/libpayload/lib/
	install -m 644 $(obj)/libpayload.ldscript $(DESTDIR)/libpayload/lib/
	install -m 755 -d $(DESTDIR)/libpayload/lib/$(ARCHDIR-y)
	printf "    INSTALL    $(DESTDIR)/libpayload/include\n"
	install -m 755 -d $(DESTDIR)/libpayload/include
	find include -type d -exec install -m755 -d $(DESTDIR)/libpayload/{} \;
	find include -type f -exec install -m644 {} $(DESTDIR)/libpayload/{} \;
	cd $(coreboottop)/src/commonlib/bsd && find include -type d -exec install -m755 -d $(abspath $(DESTDIR))/libpayload/{} \;
	cd $(coreboottop)/src/commonlib/bsd && find include -type f -exec install -m644 {} $(abspath $(DESTDIR))/libpayload/{} \;
ifeq ($(CONFIG_LP_GPL),y)
	cd $(coreboottop)/src/commonlib && find include -type d -exec install -m755 -d $(abspath $(DESTDIR))/libpayload/{} \;
	cd $(coreboottop)/src/commonlib && find include -type f -exec install -m644 {} $(abspath $(DESTDIR))/libpayload/{} \;
endif
	install -m 644 $(obj)/libpayload-config.h $(DESTDIR)/libpayload/include
	$(foreach item,$(includes), \
		install -m 755 -d $(DESTDIR)/libpayload/include/$(call extract_nth,2,$(item)); \
		install -m 644 $(call extract_nth,1,$(item)) $(DESTDIR)/libpayload/include/$(call extract_nth,2,$(item)); )
	printf "    INSTALL    $(DESTDIR)/libpayload/vboot\n"
	install -m 755 -d $(DESTDIR)/libpayload/vboot
	for file in `find $(VBOOT_SOURCE)/firmware/include \
			  $(VBOOT_SOURCE)/firmware/2lib/include \
			  -iname '*.h' -type f \
			  | sed 's,$(VBOOT_SOURCE)/firmware/,,'`; do \
		install -m 755 -d $(DESTDIR)/libpayload/vboot/$$(dirname $$file); \
		install -m 644 $(VBOOT_SOURCE)/firmware/$$file $(DESTDIR)/libpayload/vboot/$$file ; \
	done
	printf "    INSTALL    $(DESTDIR)/libpayload/bin\n"
	install -m 755 -d $(DESTDIR)/libpayload/bin
	install -m 755 bin/lpgcc $(DESTDIR)/libpayload/bin
	install -m 755 bin/lpas $(DESTDIR)/libpayload/bin
	install -m 644 bin/lp.functions $(DESTDIR)/libpayload/bin
	install -m 644 $(DOTCONFIG) $(DESTDIR)/libpayload/libpayload.config
	install -m 755 $(xcompile) $(DESTDIR)/libpayload/libpayload.xcompile

clean-for-update-target:
	rm -f $(addsuffix .a,$(addprefix $(obj)/,$(libraries))) $(obj)/libpayload.a

clean-target:
prepare:

junit.xml:
	echo '<?xml version="1.0" encoding="utf-8"?><testsuite>' > $@.tmp
	for i in $(filter-out %.old %.unit-tests,$(wildcard configs/*)); do \
		$(MAKE) clean; \
		echo "Building libpayload for $$i"; \
		cp "$$i" junit_config; \
		$(MAKE) olddefconfig DOTCONFIG=junit_config V=$(V) Q=$(Q) 2>/dev/null >/dev/null; \
		echo "<testcase classname='libpayload' name='$$i'>" >> $@.tmp; \
		$(MAKE) V=$(V) Q=$(Q) CONFIG_LP_CCACHE=$(CONFIG_LP_CCACHE) DOTCONFIG=junit_config >> $@.tmp.2 2>&1 && type="system-out" || type="failure"; \
		if [ $$type = "failure" ]; then \
			echo "<failure type='buildFailed'>" >> $@.tmp; \
		else \
			echo "<$$type>" >> $@.tmp; \
		fi; \
		echo '<![CDATA[' >> $@.tmp; \
		cat $@.tmp.2 >> $@.tmp; \
		echo "]]></$$type>" >>$@.tmp; \
		rm -f $@.tmp.2; \
		echo "</testcase>" >> $@.tmp; \
	done
	echo "</testsuite>" >> $@.tmp
	echo "libpayload build complete, test results in $@"
	mv $@.tmp $@

test-configs:
	for config in $(filter-out %.old %.unit-tests,$(wildcard configs/*)); do \
		$(MAKE) clean; \
		cp "$$config" test_config; \
		echo "*** Making libpayload config $$config ***"; \
		$(MAKE) olddefconfig DOTCONFIG=test_config V=$(V) Q=$(Q) ; \
		$(MAKE) V=$(V) Q=$(Q) CONFIG_LP_CCACHE=$(CONFIG_LP_CCACHE) DOTCONFIG=test_config; \
	done
