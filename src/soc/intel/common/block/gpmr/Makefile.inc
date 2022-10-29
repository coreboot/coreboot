## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_INTEL_COMMON_BLOCK_GPMR), y)

bootblock-y += gpmr.c
romstage-y += gpmr.c
ramstage-y += gpmr.c

endif
