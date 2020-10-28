
ifeq ($(CONFIG_DRIVERS_WIFI_GENERIC),y)

ramstage-y += generic.c
ramstage-$(CONFIG_GENERATE_SMBIOS_TABLES) += smbios.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c

cbfs-files-$(CONFIG_WIFI_SAR_CBFS)  += wifi_sar_defaults.hex
wifi_sar_defaults.hex-file := $(call strip_quotes,$(CONFIG_WIFI_SAR_CBFS_FILEPATH))
wifi_sar_defaults.hex-type := raw

endif
