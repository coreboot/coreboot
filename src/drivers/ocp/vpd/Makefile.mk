## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_OCP_VPD),y)
romstage-y += vpd_util.c
ramstage-y += vpd_util.c
ifeq ($(CONFIG_XEON_SP_COMMON_BASE),y)
romstage-$(CONFIG_SOC_INTEL_HAS_CXL) += xeonsp_cxl.c
ramstage-$(CONFIG_SOC_INTEL_HAS_CXL) += xeonsp_cxl.c
endif
endif

ramstage-$(CONFIG_LINUXPAYLOAD_CMDLINE_VPD_OVERWRITE) += vpd_cmdline.c
ifeq ($(CONFIG_VPD),y)
all-$(CONFIG_CONSOLE_OVERRIDE_LOGLEVEL) += loglevel_vpd.c
endif
