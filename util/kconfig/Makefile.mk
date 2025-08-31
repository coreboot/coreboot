# SPDX-License-Identifier: GPL-2.0-or-later

# Early coreboot specific configuration
KBUILD_KCONFIG ?= src/Kconfig

export LC_ALL=C
export LANG=C

DEFCONFIG ?= defconfig

# Include original Makefile, with just enough edits to work for us
$(objk)/Makefile.real: $(dir $(lastword $(MAKEFILE_LIST)))Makefile
	mkdir -p $(objk)
	sed \
		-e "s,\$$(obj),\$$(objk),g" \
		-e "s,\$$(src),\$$(srck),g" \
		-e "s,^help:,help_kconfig help::," \
		-e "s,^%.config:,__disable__&," \
		-e "s,^savedefconfig:,__disable__&," \
		-e "s,\$$(srctree)/arch/\$$(SRCARCH)/configs/\$$(KBUILD_DEFCONFIG),\$$(KBUILD_DEFCONFIG)," \
		-e "s,--defconfig=arch/\$$(SRCARCH)/configs/\$$(KBUILD_DEFCONFIG),--defconfig=\$$(KBUILD_DEFCONFIG)," \
		-e "/^unexport CONFIG_$$/d" \
		-e "s/if_changed,moc/cmd_moc/g" \
		$< > $@.tmp
	mv $@.tmp $@

kecho := echo

cmd = $(cmd_$(1))
cmd_conf_cfg = $< $(addprefix $(obj)/$*conf-, cflags libs bin); touch $(obj)/$*conf-bin

###
# Read a file, replacing newlines with spaces
#
# Make 4.2 or later can read a file by using its builtin function.
#
# Imported from Linux upstream:
# commit 6768fa4bcb6c1618248f135d04b9287ba2724ae0
# Author: Masahiro Yamada <masahiroy@kernel.org>
ifneq ($(filter-out 3.% 4.0 4.1, $(MAKE_VERSION)),)
read-file = $(subst $(newline),$(space),$(file < $1))
else
read-file = $(shell cat $1 2>/dev/null)
endif

-include $(objk)/Makefile.real
unexport KCONFIG_DEFCONFIG_LIST

# Fill in Linux kconfig build rules to work

savedefconfig: $(objk)/conf
	cp $(DOTCONFIG) $(DEFCONFIG)
	chmod +w $(DEFCONFIG)
	$< --savedefconfig=$(DEFCONFIG) $(KBUILD_KCONFIG)

FORCE:

filechk=$< > $@

$(objk)/%.o: $(srck)/%.c
	$(HOSTCC) -I $(srck) -I $(objk) -c $(strip $(HOSTCFLAGS_$(notdir $@))) -o $@ $<

$(objk)/%.o: $(srck)/%.cc
	$(HOSTCXX) -I $(srck) -I $(objk) -c $(strip $(HOSTCXXFLAGS_$(notdir $@))) -o $@ $<

$(objk)/%.o: $(objk)/%.c
	$(HOSTCC) -I $(srck) -I $(objk) -c -o $@ $<

$(objk)/qconf-moc.o: $(objk)/qconf-moc.cc
	$(HOSTCXX) -I $(srck) -I $(objk) -c $(strip $(HOSTCXXFLAGS_$(notdir $@))) -o $@ $<

define hostprogs_template
# $1 entry in hostprogs
$(objk)/$(1): $$(foreach _o,$$($(1)-objs) $$($(1)-cxxobjs),$(objk)/$$(_o)) | $(wildcard $(objk)/$(1)-cfg)
	$$(HOSTCXX) -o $$@ $$^ $$(HOSTLDLIBS_$(1))
endef

$(foreach prog,$(hostprogs),$(eval $(call hostprogs_template,$(prog))))

# This might be a bit of a chicken & egg problem, using a kconfig flag when
# building kconfig, but if you're messing with the parser you probably know
# what you're doing: make CONFIG_UTIL_GENPARSER=y
ifeq ($(CONFIG_UTIL_GENPARSER),y)
$(objk)/%.tab.c $(objk)/%.tab.h: $(srck)/%.y
	bison -t -l --defines -b $(objk)/$* $<

$(objk)/%.lex.c: $(srck)/%.l
	flex -L -o$@ $<

else # !CONFIG_UTIL_GENPARSER

$(objk)/parser.tab.c: | $(objk)/parser.tab.h

$(objk)/%: $(srck)/%_shipped
	cp $< $@

endif

# Support mingw by shipping our own regex implementation
_OS=$(shell uname -s |cut -c-7)
regex-objs=
ifeq ($(_OS),MINGW32)
	regex-objs=regex.o
endif
$(objk)/regex.o: $(srck)/regex.c
	$(HOSTCC) $(HOSTCFLAGS) $(HOST_EXTRACFLAGS) -DHAVE_STRING_H -c -o $@ $<

conf-objs += $(regex-objs)
mconf-objs += $(regex-objs)

# Provide tool to convert kconfig output into Ada format
$(objk)/toada: $(objk)/toada.o
	$(HOSTCC) $(HOSTCFLAGS) $(HOST_EXTRACFLAGS) -o $@ $^
$(objk)/toada.o: $(srck)/toada.c
	$(HOSTCC) $(HOSTCFLAGS) $(HOST_EXTRACFLAGS) -c -o $@ $<
