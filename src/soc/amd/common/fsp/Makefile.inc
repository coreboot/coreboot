ifeq ($(CONFIG_PLATFORM_USES_FSP2_0),y)
romstage-y += fsp_reset.c
ramstage-y += fsp_reset.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += fsp-acpi.c
ramstage-$(CONFIG_SOC_AMD_COMMON_FSP_DMI_TABLES) += dmi.c

subdirs-y += ./*

endif # CONFIG_PLATFORM_USES_FSP2_0
