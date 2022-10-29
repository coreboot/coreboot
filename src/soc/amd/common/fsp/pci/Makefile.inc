## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_COMMON_FSP_PCI),y)

ramstage-y += pci_routing_info.c

endif # CONFIG_SOC_AMD_COMMON_FSP_PCI
