ramstage-$(CONFIG_IPMI_OCP) += ipmi_ocp.c ipmi_sel.c
ifeq ($(CONFIG_IPMI_OCP),y)
romstage-$(CONFIG_IPMI_KCS_ROMSTAGE) += ipmi_ocp_romstage.c
smm-$(CONFIG_IPMI_BMC_SEL) += ipmi_sel.c
endif
