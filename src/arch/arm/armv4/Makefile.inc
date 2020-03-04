################################################################################
##
## SPDX-License-Identifier: GPL-2.0-only
## This file is part of the coreboot project.
##
###############################################################################

armv4_flags = -marm -march=armv4t -I$(src)/arch/arm/include/armv4/ \
	-D__COREBOOT_ARM_ARCH__=4

###############################################################################
# bootblock
###############################################################################

ifeq ($(CONFIG_ARCH_BOOTBLOCK_ARMV4),y)

ifneq ($(CONFIG_BOOTBLOCK_CUSTOM),y)
bootblock-y += bootblock.S
endif

bootblock-y += cache.c

bootblock-generic-ccopts += $(armv4_flags)

endif # CONFIG_ARCH_BOOTBLOCK_ARMV4

################################################################################
## Verstage
################################################################################

ifeq ($(CONFIG_ARCH_VERSTAGE_ARMV4),y)
verstage-generic-ccopts += $(armv4_flags)

verstage-y += cache.c
endif # CONFIG_ARCH_VERSTAGE_ARMV4

###############################################################################
# romstage
###############################################################################

ifeq ($(CONFIG_ARCH_ROMSTAGE_ARMV4),y)


romstage-generic-ccopts += $(armv4_flags)
romstage-y += cache.c

rmodules_arm-generic-ccopts += $(armv4_flags)

endif # CONFIG_ARCH_ROMSTAGE_ARMV4

###############################################################################
# ramstage
###############################################################################

ifeq ($(CONFIG_ARCH_RAMSTAGE_ARMV4),y)

ramstage-generic-ccopts += $(armv4_flags)

endif # CONFIG_ARCH_RAMSTAGE_ARMV4
