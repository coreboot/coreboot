
ifeq ($(CONFIG_DRIVERS_WIFI_GENERIC),y)

romstage-y += generic.c

ramstage-y += generic.c
ramstage-$(CONFIG_GENERATE_SMBIOS_TABLES) += smbios.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c

CONFIG_WIFI_SAR_CBFS_FILEPATH := $(call strip_quotes,$(CONFIG_WIFI_SAR_CBFS_FILEPATH))

ifneq ($(CONFIG_WIFI_SAR_CBFS_FILEPATH),)

cbfs-files-$(CONFIG_USE_SAR) += wifi_sar_defaults.hex
wifi_sar_defaults.hex-file := $(CONFIG_WIFI_SAR_CBFS_FILEPATH)
wifi_sar_defaults.hex-type := raw

endif

endif
