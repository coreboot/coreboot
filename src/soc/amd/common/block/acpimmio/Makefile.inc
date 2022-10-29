## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO),y)

all-y += mmio_util.c
smm-y += mmio_util.c

all-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO_BIOSRAM) += biosram.c
smm-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO_BIOSRAM) += biosram.c

bootblock-y += print_reset_status.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_ACPIMMIO
