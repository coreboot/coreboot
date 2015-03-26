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
## Foundation, Inc.
##
################################################################################

subdirs-y += lib/

subdirs-$(CONFIG_ARM64_USE_SECURE_MONITOR) += secmon/

armv8_flags = -march=armv8-a -I$(src)/arch/arm64/include/armv8/ -D__COREBOOT_ARM_ARCH__=8

armv8_asm_flags = $(armv8_flags)

################################################################################
## bootblock
################################################################################
ifeq ($(CONFIG_ARCH_BOOTBLOCK_ARMV8_64),y)

ifneq ($(CONFIG_ARM64_BOOTBLOCK_CUSTOM),y)
bootblock-y += bootblock.S
bootblock-y += bootblock_simple.c
endif
bootblock-y += cache.c
bootblock-y += cache_helpers.S
bootblock-y += cpu.S
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += exception.c

bootblock-c-ccopts += $(armv8_flags)
bootblock-S-ccopts += $(armv8_asm_flags)

endif

################################################################################
## verstage
################################################################################
ifeq ($(CONFIG_ARCH_VERSTAGE_ARMV8_64),y)

verstage-y += cache.c
verstage-y += cpu.S
verstage-y += cache_helpers.S
verstage-y += exception.c

libverstage-c-ccopts += $(armv8_flags)
libverstage-S-ccopts += $(armv8_asm_flags)
verstage-c-ccopts += $(armv8_flags)
verstage-S-ccopts += $(armv8_asm_flags)

endif

################################################################################
## romstage
################################################################################
ifeq ($(CONFIG_ARCH_ROMSTAGE_ARMV8_64),y)

romstage-y += cache.c
romstage-y += cache_helpers.S
romstage-y += cpu.S
romstage-y += exception.c

romstage-c-ccopts += $(armv8_flags)
romstage-S-ccopts += $(armv8_asm_flags)

rmodules_arm64-c-ccopts += $(armv8_flags)
rmodules_arm64-S-ccopts += $(armv8_asm_flags)

endif

################################################################################
## ramstage
################################################################################
ifeq ($(CONFIG_ARCH_RAMSTAGE_ARMV8_64),y)

ramstage-y += cache.c
ramstage-y += cache_helpers.S
ramstage-y += cpu.S
ramstage-y += exception.c
ramstage-y += mmu.c

ramstage-$(CONFIG_ARM64_USE_SECURE_MONITOR) += secmon_loader.c

ramstage-c-ccopts += $(armv8_flags)
ramstage-S-ccopts += $(armv8_asm_flags)

rmodules_arm64-c-ccopts += $(armv8_flags)
rmodules_arm64-S-ccopts += $(armv8_asm_flags)

endif
