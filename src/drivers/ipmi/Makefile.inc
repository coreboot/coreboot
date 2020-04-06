ramstage-$(CONFIG_IPMI_KCS) += ipmi_kcs.c
ramstage-$(CONFIG_IPMI_KCS) += ipmi_kcs_ops.c
ramstage-$(CONFIG_IPMI_KCS) += ipmi_ops.c
ramstage-$(CONFIG_IPMI_KCS) += ipmi_fru.c
romstage-$(CONFIG_IPMI_KCS_ROMSTAGE) += ipmi_kcs_ops_premem.c
romstage-$(CONFIG_IPMI_KCS_ROMSTAGE) += ipmi_kcs.c
romstage-$(CONFIG_IPMI_KCS_ROMSTAGE) += ipmi_ops.c
