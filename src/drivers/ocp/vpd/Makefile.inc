## SPDX-License-Identifier: GPL-2.0-only

romstage-$(CONFIG_OCP_VPD) += vpd_util.c
ramstage-$(CONFIG_OCP_VPD) += vpd_util.c
ramstage-$(CONFIG_LINUXPAYLOAD_CMDLINE_VPD_OVERWRITE) += vpd_cmdline.c
ifeq ($(CONFIG_VPD),y)
all-$(CONFIG_CONSOLE_OVERRIDE_LOGLEVEL) += loglevel_vpd.c
endif
