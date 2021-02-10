ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_ACPI),y)

bootblock-y += acpi.c
verstage_x86-y += acpi.c
romstage-y += acpi.c
ramstage-y += acpi.c
postcar-y += acpi.c
smm-y += acpi.c

ramstage-y += pm_state.c
ramstage-y += tables.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_ACPI
