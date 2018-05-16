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
################################################################################

subdirs-y += lib/

ifeq ($(CONFIG_ARCH_ARMV8_EXTENSION),0)
march = armv8-a
else
march = armv8.$(CONFIG_ARCH_ARMV8_EXTENSION)-a
endif

armv8_flags = -march=$(march) -I$(src)/arch/arm64/include/armv8/ -D__COREBOOT_ARM_ARCH__=8

################################################################################
## bootblock
################################################################################
ifeq ($(CONFIG_ARCH_BOOTBLOCK_ARMV8_64),y)

ifneq ($(CONFIG_BOOTBLOCK_CUSTOM),y)
decompressor-y += bootblock.S
ifneq ($(CONFIG_COMPRESS_BOOTBLOCK),y)
bootblock-y += bootblock.S
endif
endif
decompressor-y += cpu.S
bootblock-y += cpu.S
decompressor-y += cache.c
bootblock-y += cache.c
decompressor-y += mmu.c
bootblock-y += mmu.c

bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += exception.c

decompressor-generic-ccopts += $(armv8_flags)
bootblock-generic-ccopts += $(armv8_flags)

# Required to access unaligned timestamp struct members before MMU is active
# (TODO: Maybe use explicit unaligned accesses in timestamp code instead, or
# evaluate redesigning timestamp data structures to avoid misaligned members.)
decompressor-c-ccopts += -mstrict-align
bootblock-c-ccopts += -mstrict-align

endif

################################################################################
## verstage
################################################################################
ifeq ($(CONFIG_ARCH_VERSTAGE_ARMV8_64),y)

verstage-y += cache.c
verstage-y += cpu.S
verstage-y += exception.c

verstage-generic-ccopts += $(armv8_flags)

endif

################################################################################
## romstage
################################################################################
ifeq ($(CONFIG_ARCH_ROMSTAGE_ARMV8_64),y)

romstage-y += cache.c
romstage-y += cpu.S
romstage-y += exception.c
romstage-y += mmu.c

romstage-generic-ccopts += $(armv8_flags)

rmodules_arm64-generic-ccopts += $(armv8_flags)

endif

################################################################################
## ramstage
################################################################################
ifeq ($(CONFIG_ARCH_RAMSTAGE_ARMV8_64),y)

ramstage-y += cache.c
ramstage-y += cpu.S
ramstage-y += exception.c
ramstage-y += mmu.c

ramstage-generic-ccopts += $(armv8_flags)

rmodules_arm64-generic-ccopts += $(armv8_flags)

endif
