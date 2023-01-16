romstage-$(CONFIG_OCP_VPD) += vpd_util.c
ramstage-$(CONFIG_OCP_VPD) += vpd_util.c
ifeq ($(CONFIG_VPD),y)
all-$(CONFIG_CONSOLE_OVERRIDE_LOGLEVEL) += loglevel_vpd.c
endif
