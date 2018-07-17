#
# This file is part of the coreboot project.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#

ramstage-$(CONFIG_DRIVERS_INTEL_WIFI) += wifi.c

cbfs-files-$(CONFIG_WIFI_SAR_CBFS)  += wifi_sar_defaults.hex
wifi_sar_defaults.hex-file := $(call strip_quotes,$(CONFIG_WIFI_SAR_CBFS_FILEPATH))
wifi_sar_defaults.hex-type := raw
