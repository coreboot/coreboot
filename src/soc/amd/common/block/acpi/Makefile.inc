ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_ACPI),y)

bootblock-y += acpi.c
verstage_x86-y += acpi.c
romstage-y += acpi.c
ramstage-y += acpi.c
postcar-y += acpi.c
smm-y += acpi.c

ramstage-y += pm_state.c
ramstage-y += tables.c
ramstage-$(CONFIG_ACPI_BERT) += bert.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPI_ALIB) += alib.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPI_CPPC) += cppc.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPI_GPIO) += gpio.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPI_IVRS) += ivrs.c

romstage-y += elog.c
ramstage-y += elog.c
smm-y += elog.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_ACPI
