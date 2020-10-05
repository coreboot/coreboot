ramstage-$(CONFIG_DRIVERS_WIFI_GENERIC) += generic.c

cbfs-files-$(CONFIG_WIFI_SAR_CBFS)  += wifi_sar_defaults.hex
wifi_sar_defaults.hex-file := $(call strip_quotes,$(CONFIG_WIFI_SAR_CBFS_FILEPATH))
wifi_sar_defaults.hex-type := raw
