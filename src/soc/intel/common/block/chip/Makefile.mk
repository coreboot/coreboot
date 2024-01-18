## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_INTEL_COMMON_BLOCK_CHIP_CONFIG),y)

bootblock-y += chip.c
romstage-y += chip.c
verstage-y += chip.c
ramstage-y += chip.c
smm-y += chip.c
postcar-y += chip.c

endif
