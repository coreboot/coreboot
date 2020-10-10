ifeq ($(CONFIG_SOC_INTEL_COMMON_BLOCK_PMC),y)
bootblock-y += pmclib.c
romstage-y += pmclib.c
ramstage-y += pmc.c
ramstage-y += pmclib.c pmc_ipc.c
smm-y += pmclib.c
verstage-y += pmclib.c
postcar-y += pmclib.c
ramstage-$(CONFIG_PMC_IPC_ACPI_INTERFACE) += pmc_ipc.c
endif
