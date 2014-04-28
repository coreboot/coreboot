################################################################################
##
## This file is part of the coreboot project.
##
## Copyright (C) 2014 The ChromiumOS Authors
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
################################################################################

armv8_flags = -march=armv8-a -I$(src)/arch/arm64/include/armv8/ -D__COREBOOT_ARM_ARCH__=8

armv8_asm_flags = $(armv8_flags)

################################################################################
## bootblock
################################################################################
ifeq ($(CONFIG_ARCH_BOOTBLOCK_ARM_V8_64),y)

ifneq ($(CONFIG_ARM_BOOTBLOCK_CUSTOM),y)
bootblock-y += bootblock.S
endif
bootblock-$(CONFIG_ARM_BOOTBLOCK_SIMPLE) += bootblock_simple.c
bootblock-$(CONFIG_ARM_BOOTBLOCK_NORMAL) += bootblock_normal.c
bootblock-y += cache.c
bootblock-y += cpu.S
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += exception.c
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += exception_asm.S

bootblock-c-ccopts += $(armv8_flags)
bootblock-S-ccopts += $(armv8_asm_flags)

endif

################################################################################
## romstage
################################################################################
ifeq ($(CONFIG_ARCH_ROMSTAGE_ARM_V8_64),y)

romstage-y += cache.c
romstage-y += cpu.S
romstage-y += exception.c
romstage-y += exception_asm.S

romstage-c-ccopts += $(armv8_flags)
romstage-S-ccopts += $(armv8_asm_flags)

endif

################################################################################
## ramstage
################################################################################
ifeq ($(CONFIG_ARCH_RAMSTAGE_ARM_V8_64),y)

ramstage-y += cache.c
ramstage-y += cpu.S
ramstage-y += exception.c
ramstage-y += exception_asm.S

ramstage-c-ccopts += $(armv8_flags)
ramstage-S-ccopts += $(armv8_asm_flags)

endif
