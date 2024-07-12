## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_ROOT_COMPLEX),y)

ramstage-y += ioapic.c
ramstage-y += non_pci_resources.c
ramstage-y += root_complex.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_ROOT_COMPLEX
