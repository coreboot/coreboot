# SPDX-License-Identifier: GPL-2.0-only

bootblock-$(CONFIG_VPD) += vpd_decode.c vpd.c
verstage-$(CONFIG_VPD) += vpd_decode.c vpd.c
romstage-$(CONFIG_VPD) += vpd_decode.c vpd.c
postcar-$(CONFIG_VPD) += vpd_decode.c vpd.c
ramstage-$(CONFIG_VPD) += vpd_decode.c vpd.c
ramstage-$(CONFIG_SMBIOS_SERIAL_FROM_VPD) += vpd_serial.c
