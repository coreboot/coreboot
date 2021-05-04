ifeq ($(CONFIG_PLATFORM_USES_FSP2_0),y)
romstage-y += fsp_reset.c
ramstage-y += fsp_reset.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += fsp-acpi.c

subdirs-y += ./*

endif # CONFIG_PLATFORM_USES_FSP2_0
