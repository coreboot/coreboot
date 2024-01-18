## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_INTEL_COMMON_BLOCK_PMC),y)
bootblock-y += pmclib.c
romstage-y += pmclib.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_PMC_DISCOVERABLE) += pmc.c
ramstage-y += pmclib.c
smm-y += pmclib.c
verstage-y += pmclib.c
postcar-y += pmclib.c
ramstage-$(CONFIG_PMC_IPC_ACPI_INTERFACE) += pmc_ipc.c
endif
