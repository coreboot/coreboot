## SPDX-License-Identifier: GPL-2.0-only


ifeq ($(CONFIG_DRIVERS_WIFI_GENERIC),y)

romstage-y += generic.c

ramstage-y += generic.c
ramstage-$(CONFIG_GENERATE_SMBIOS_TABLES) += smbios.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c
ramstage-$(CONFIG_USE_MTCL) += mtcl.c

CONFIG_WIFI_SAR_CBFS_FILEPATH := $(call strip_quotes,$(CONFIG_WIFI_SAR_CBFS_FILEPATH))

ifneq ($(CONFIG_WIFI_SAR_CBFS_FILEPATH),)

cbfs-files-$(CONFIG_USE_SAR) += wifi_sar_defaults.hex
wifi_sar_defaults.hex-file := $(CONFIG_WIFI_SAR_CBFS_FILEPATH)
wifi_sar_defaults.hex-type := raw

endif

CONFIG_WIFI_MTCL_CBFS_FILEPATH := $(call strip_quotes,$(CONFIG_WIFI_MTCL_CBFS_FILEPATH))

ifneq ($(CONFIG_WIFI_MTCL_CBFS_FILEPATH),)

cbfs-files-$(CONFIG_USE_MTCL) += wifi_mtcl.bin
wifi_mtcl.bin-file := $(CONFIG_WIFI_MTCL_CBFS_FILEPATH)
wifi_mtcl.bin-type := raw

endif

endif
