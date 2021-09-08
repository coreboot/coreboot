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
		$< > $@.tmp
	mv $@.tmp $@

kecho := echo

-include $(objk)/Makefile.real
unexport KCONFIG_DEFCONFIG_LIST

# Fill in Linux kconfig build rules to work

oldconfig: KCONFIG_STRICT=

savedefconfig: $(objk)/conf
	cp $(DOTCONFIG) $(DEFCONFIG)
	$< --savedefconfig=$(DEFCONFIG) $(KBUILD_KCONFIG)

FORCE:

filechk=$< > $@

$(objk)/%.o: $(srck)/%.c
	$(HOSTCC) -I $(srck) -I $(objk) -c $(HOSTCFLAGS_$(notdir $@)) -o $@ $<

$(objk)/%.o: $(srck)/%.cc
	$(HOSTCXX) -I $(srck) -I $(objk) -c $(HOSTCXXFLAGS_$(notdir $@)) -o $@ $<

$(objk)/%.o: $(objk)/%.c
	$(HOSTCC) -I $(srck) -I $(objk) -c -o $@ $<

$(objk)/%.moc: $(srck)/%.h | $(objk)/qconf-cfg
	$(call cmd_moc)

define hostprogs_template
# $1 entry in hostprogs
$(objk)/$(1): $$(foreach _o,$$($(1)-objs) $$($(1)-cxxobjs),$(objk)/$$(_o)) | $(wildcard $(objk)/$(1)-cfg)
	$$(HOSTCXX) -o $$@ $$^ $$(HOSTLDLIBS_$(1))
endef

$(foreach prog,$(hostprogs),$(eval $(call hostprogs_template,$(prog))))

$(objk)/%.tab.c $(objk)/%.tab.h: $(srck)/%.y
	bison -t -l --defines -b $(objk)/$* $<

$(objk)/%.lex.c: $(srck)/%.l
	flex -L -o$@ $<

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
