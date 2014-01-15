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

armv7_flags = -march=armv7-a -mthumb -mthumb-interwork \
	-I$(src)/arch/arm/include/armv7/ -D__COREBOOT_ARM_ARCH__=7

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

CFLAGS_bootblock += $(armv7_flags)
CPPFLAGS_bootblock += $(armv7_flags)

endif # CONFIG_ARCH_BOOTBLOCK_ARMV7

###############################################################################
# romstage
###############################################################################

ifeq ($(CONFIG_ARCH_ROMSTAGE_ARMV7),y)

romstage-y += cache.c
romstage-y += cpu.S
romstage-y += exception.c
romstage-y += exception_asm.S
romstage-y += mmu.c

CFLAGS_romstage += $(armv7_flags)
CPPFLAGS_romstage += $(armv7_flags)

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

CFLAGS_ramstage += $(armv7_flags)
CPPFLAGS_ramstage += $(armv7_flags)

endif # CONFIG_ARCH_RAMSTAGE_ARMV7
