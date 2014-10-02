################################################################################
##
## This file is part of the coreboot project.
##
## Copyright (C) 2013 The ChromiumOS Authors
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

armv7_flags = -march=armv7-a -mthumb \
	-I$(src)/arch/arm/include/armv7/ -D__COREBOOT_ARM_ARCH__=7
armv7_asm_flags = $(armv7_flags) -Wa,-mthumb -Wa,-mimplicit-it=always \
	-Wa,-mno-warn-deprecated

###############################################################################
# bootblock
###############################################################################

ifeq ($(CONFIG_ARCH_BOOTBLOCK_ARMV7),y)

ifneq ($(CONFIG_ARM_BOOTBLOCK_CUSTOM),y)
bootblock-y += bootblock.S
bootblock-$(CONFIG_BOOTBLOCK_SIMPLE) += bootblock_simple.c
endif

bootblock-y += cache.c
bootblock-y += cpu.S
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += exception.c
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += exception_asm.S
bootblock-y += mmu.c

bootblock-c-ccopts += $(armv7_flags)
bootblock-S-ccopts += $(armv7_asm_flags)

endif # CONFIG_ARCH_BOOTBLOCK_ARMV7

################################################################################
## verification stage
################################################################################

ifeq ($(CONFIG_ARCH_VERSTAGE_ARM_V7),y)

verstage-c-ccopts += $(armv7_flags)
verstage-S-ccopts += $(armv7_asm_flags)
verstage-y += cache.c
verstage-y += cpu.S
verstage-y += exception.c
verstage-y += exception_asm.S
verstage-y += mmu.c

endif # CONFIG_ARCH_VERSTAGE_ARMV7

###############################################################################
# romstage
###############################################################################

ifeq ($(CONFIG_ARCH_ROMSTAGE_ARMV7),y)

romstage-y += cache.c
romstage-y += cpu.S
romstage-y += exception.c
romstage-y += exception_asm.S
romstage-y += mmu.c
romstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c

romstage-c-ccopts += $(armv7_flags)
romstage-S-ccopts += $(armv7_asm_flags)

rmodules_arm-c-ccopts += $(armv7_flags)
rmodules_arm-S-ccopts += $(armv7_asm_flags)

endif # CONFIG_ARCH_ROMSTAGE_ARMV7

###############################################################################
# ramstage
###############################################################################

ifeq ($(CONFIG_ARCH_RAMSTAGE_ARMV7),y)

ramstage-y += cache.c
ramstage-y += cpu.S
ramstage-y += exception.c
ramstage-y += exception_asm.S
ramstage-y += mmu.c
ramstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c

ramstage-c-ccopts += $(armv7_flags)
ramstage-S-ccopts += $(armv7_asm_flags)

# All rmodule code is armv7 if ramstage is armv7.
rmodules_arm-c-ccopts += $(armv7_flags)
rmodules_arm-S-ccopts += $(armv7_asm_flags)
endif # CONFIG_ARCH_RAMSTAGE_ARMV7
