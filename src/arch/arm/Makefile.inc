################################################################################
##
## This file is part of the coreboot project.
##
## Copyright (C) 2012-2013 The ChromiumOS Authors
## Copyright (C) 2012 Alexandru Gagniuc <mr.nuke.me@gmail.com>
## Copyright (C) 2009-2010 coresystems GmbH
## Copyright (C) 2009 Ronald G. Minnich
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

###############################################################################
# ARM specific options
###############################################################################

ifeq ($(CONFIG_ARCH_RAMSTAGE_ARM),y)
check-ramstage-overlap-regions += postram_cbfs_cache stack ttb
endif

ifeq ($(CONFIG_ARCH_ARM),y)
subdirs-y += libgcc/
subdirs-y += armv4/ armv7/

stages_c = $(src)/arch/arm/stages.c
stages_o = $(obj)/arch/arm/stages.o

$(stages_o): $(stages_c) $(obj)/config.h
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC_arm) -I. $(CPPFLAGS_arm) -c -o $@ $< -marm

endif # CONFIG_ARCH_ARM

###############################################################################
# bootblock
###############################################################################

ifeq ($(CONFIG_ARCH_BOOTBLOCK_ARM),y)

decompressor-y += id.S
bootblock-y += id.S
$(call src-to-obj,decompressor,$(dir)/id.S): $(obj)/build.h
$(call src-to-obj,bootblock,$(dir)/id.S): $(obj)/build.h

decompressor-y += boot.c
bootblock-y += boot.c
decompressor-y += div0.c
bootblock-y += div0.c
decompressor-y += eabi_compat.c
bootblock-y += eabi_compat.c
decompressor-y += memset.S
bootblock-y += memset.S
decompressor-y += memcpy.S
bootblock-y += memcpy.S
decompressor-y += memmove.S
bootblock-y += memmove.S

bootblock-y += clock.c
bootblock-y += stages.c

$(objcbfs)/bootblock.debug: $$(bootblock-objs)
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_bootblock) $(LDFLAGS_bootblock) -o $@ -L$(obj) -T $(call src-to-obj,bootblock,src/mainboard/$(MAINBOARDDIR)/memlayout.ld) --whole-archive --start-group $(filter-out %.ld,$(bootblock-objs)) --end-group

$(objcbfs)/decompressor.debug: $$(decompressor-objs)
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_bootblock) $(LDFLAGS_bootblock) -o $@ -L$(obj) -T $(call src-to-obj,decompressor,src/mainboard/$(MAINBOARDDIR)/memlayout.ld) --whole-archive --start-group $(filter-out %.ld,$(decompressor-objs)) --end-group

endif # CONFIG_ARCH_BOOTBLOCK_ARM

###############################################################################
# verification stage
###############################################################################

ifeq ($(CONFIG_ARCH_VERSTAGE_ARM),y)

$(objcbfs)/verstage.debug: $$(verstage-objs)
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_verstage) $(LDFLAGS_verstage) -o $@ -L$(obj) -T $(call src-to-obj,verstage,src/mainboard/$(MAINBOARDDIR)/memlayout.ld) --whole-archive --start-group $(filter-out %.ld,$(verstage-objs)) --end-group

verstage-y += boot.c
verstage-y += div0.c
verstage-y += eabi_compat.c
verstage-y += memset.S
verstage-y += memcpy.S
verstage-y += memmove.S
verstage-y += stages.c

endif # CONFIG_ARCH_VERSTAGE_ARM

###############################################################################
# romstage
###############################################################################

ifeq ($(CONFIG_ARCH_ROMSTAGE_ARM),y)

romstage-y += boot.c
romstage-y += stages.c
romstage-y += div0.c
romstage-y += eabi_compat.c
romstage-y += memset.S
romstage-y += memcpy.S
romstage-y += memmove.S
romstage-y += clock.c
rmodules_arm-y += memset.S
rmodules_arm-y += memcpy.S
rmodules_arm-y += memmove.S
rmodules_arm-y += eabi_compat.c

$(objcbfs)/romstage.debug: $$(romstage-objs)
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_romstage) $(LDFLAGS_romstage) -o $@ -L$(obj) -T $(call src-to-obj,romstage,src/mainboard/$(MAINBOARDDIR)/memlayout.ld) --whole-archive --start-group $(filter-out %.ld,$(romstage-objs)) --end-group

endif # CONFIG_ARCH_ROMSTAGE_ARM

###############################################################################
# ramstage
###############################################################################

ifeq ($(CONFIG_ARCH_RAMSTAGE_ARM),y)

ramstage-y += stages.c
ramstage-y += div0.c
ramstage-$(CONFIG_COOP_MULTITASKING) += cpu.c
ramstage-y += eabi_compat.c
ramstage-y += boot.c
ramstage-y += tables.c
ramstage-y += memset.S
ramstage-y += memcpy.S
ramstage-y += memmove.S
ramstage-y += clock.c

rmodules_arm-y += memset.S
rmodules_arm-y += memcpy.S
rmodules_arm-y += memmove.S
rmodules_arm-y += eabi_compat.c
ramstage-srcs += $(wildcard src/mainboard/$(MAINBOARDDIR)/mainboard.c)

$(objcbfs)/ramstage.debug: $$(ramstage-objs)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(LD_ramstage) $(LDFLAGS_ramstage) -o $@ -L$(obj) -T $(call src-to-obj,ramstage,src/mainboard/$(MAINBOARDDIR)/memlayout.ld) --whole-archive --start-group $(filter-out %.ld,$(ramstage-objs)) --end-group

endif # CONFIG_ARCH_RAMSTAGE_ARM
