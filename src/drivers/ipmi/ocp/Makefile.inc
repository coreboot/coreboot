ramstage-$(CONFIG_IPMI_OCP) += ipmi_ocp.c
ifeq ($(CONFIG_IPMI_OCP),y)
romstage-$(CONFIG_IPMI_KCS_ROMSTAGE) += ipmi_ocp_romstage.c
endif
