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
## Foundation, Inc.
##
###############################################################################

armv4_flags = -marm -march=armv4t -I$(src)/arch/arm/include/armv4/ \
	-D__COREBOOT_ARM_ARCH__=4

###############################################################################
# bootblock
###############################################################################

ifeq ($(CONFIG_ARCH_BOOTBLOCK_ARMV4),y)

ifneq ($(CONFIG_ARM_BOOTBLOCK_CUSTOM),y)
bootblock-y += bootblock.S
bootblock-y += bootblock_simple.c
endif

bootblock-y += cache.c

bootblock-c-ccopts += $(armv4_flags)
bootblock-S-ccopts += $(armv4_flags)

endif # CONFIG_ARCH_BOOTBLOCK_ARMV4

################################################################################
## Verstage
################################################################################

ifeq ($(CONFIG_ARCH_VERSTAGE_ARMV4),y)
libverstage-c-ccopts += $(armv4_flags)
libverstage-S-ccopts += $(armv4_flags)
verstage-c-ccopts += $(armv4_flags)
verstage-S-ccopts += $(armv4_flags)

verstage-y += cache.c
endif # CONFIG_ARCH_VERSTAGE_ARMV4

###############################################################################
# romstage
###############################################################################

ifeq ($(CONFIG_ARCH_ROMSTAGE_ARMV4),y)


romstage-c-ccopts += $(armv4_flags)
romstage-S-ccopts += $(armv4_flags)
romstage-y += cache.c

rmodules_arm-c-ccopts += $(armv4_flags)
rmodules_arm-S-ccopts += $(armv4_flags)

endif # CONFIG_ARCH_ROMSTAGE_ARMV4

###############################################################################
# ramstage
###############################################################################

ifeq ($(CONFIG_ARCH_RAMSTAGE_ARMV4),y)

ramstage-c-ccopts += $(armv4_flags)
ramstage-S-ccopts += $(armv4_flags)

endif # CONFIG_ARCH_RAMSTAGE_ARMV4
