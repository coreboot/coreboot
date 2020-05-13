# SPDX-License-Identifier: GPL-2.0-only

ramstage-$(CONFIG_DRIVERS_INTEL_WIFI) += wifi.c

cbfs-files-$(CONFIG_WIFI_SAR_CBFS)  += wifi_sar_defaults.hex
wifi_sar_defaults.hex-file := $(call strip_quotes,$(CONFIG_WIFI_SAR_CBFS_FILEPATH))
wifi_sar_defaults.hex-type := raw
