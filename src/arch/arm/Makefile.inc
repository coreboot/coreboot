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
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
##
###############################################################################
# Take care of subdirectories
###############################################################################
subdirs-y += libgcc/
subdirs-y += armv4/ armv7/

###############################################################################
# ARM specific options
###############################################################################

ifeq ($(CONFIG_ARCH_ROMSTAGE_ARM),y)
CBFSTOOL_PRE1_OPTS = -m arm -b $(CONFIG_BOOTBLOCK_ROM_OFFSET) -H $(CONFIG_CBFS_HEADER_ROM_OFFSET) -o $(CONFIG_CBFS_ROM_OFFSET)
CBFSTOOL_PRE_OPTS = -b 0
endif

ifeq ($(CONFIG_ARCH_ARM),y)
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

bootblock-y += id.S
$(obj)/arch/arm/id.bootblock.o: $(obj)/build.h

bootblock-y += boot.c
bootblock-y += stages.c
bootblock-y += eabi_compat.c
bootblock-y += memset.S
bootblock-y += memcpy.S
bootblock-y += memmove.S
bootblock-y += div0.c
bootblock-y += clock.c

$(objcbfs)/bootblock.debug: $(src)/arch/arm/bootblock.ld $(obj)/ldoptions $$(bootblock-objs) $$(VERSTAGE_LIB)
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_bootblock) --gc-sections -static -o $@ -L$(obj) --start-group $(bootblock-objs) --end-group -T $(src)/arch/arm/bootblock.ld

endif # CONFIG_ARCH_BOOTBLOCK_ARM

###############################################################################
# verification stage
###############################################################################

$(objcbfs)/verstage.debug: $$(verstage-objs) $(src)/arch/arm/verstage.ld $(obj)/ldoptions $$(VB2_LIB)
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_verstage) --gc-sections -static -o $@ -L$(obj) --start-group $(verstage-objs) --end-group -T $(src)/arch/arm/verstage.ld

verstage-y += div0.c
verstage-y += eabi_compat.c
verstage-y += memset.S
verstage-y += memcpy.S
verstage-y += memmove.S
verstage-y += stages.c

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

VBOOT_STUB_DEPS += $(obj)/arch/arm/eabi_compat.rmodules_arm.o

$(objcbfs)/romstage.debug: $$(romstage-objs) $(src)/arch/arm/romstage.ld $(obj)/ldoptions
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_romstage) -nostdlib --gc-sections -static -o $@ -L$(obj) --start-group $(romstage-objs) --end-group -T $(src)/arch/arm/romstage.ld

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

$(objcbfs)/ramstage.debug: $$(ramstage-objs) $(src)/arch/arm/ramstage.ld $(obj)/ldoptions
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(LD_ramstage) -nostdlib --gc-sections -o $@ -L$(obj) --start-group $(ramstage-objs) --end-group -T $(src)/arch/arm/ramstage.ld

$(objgenerated)/ramstage.o: $(stages_o) $$(ramstage-objs)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(LD_ramstage) -nostdlib --gc-sections -r -o $@ --start-group $(ramstage-objs) --end-group

endif # CONFIG_ARCH_RAMSTAGE_ARM
