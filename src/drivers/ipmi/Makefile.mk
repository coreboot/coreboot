## SPDX-License-Identifier: GPL-2.0-only

ramstage-$(CONFIG_IPMI_KCS) += ipmi_if.c
ramstage-$(CONFIG_IPMI_KCS) += ipmi_kcs.c
ramstage-$(CONFIG_IPMI_KCS) += ipmi_kcs_ops.c
ramstage-$(CONFIG_IPMI_KCS) += ipmi_ops.c
ramstage-$(CONFIG_IPMI_KCS) += ipmi_fru.c
ramstage-$(CONFIG_DRIVERS_IPMI_SUPERMICRO_OEM) += supermicro_oem.c
romstage-$(CONFIG_IPMI_KCS_ROMSTAGE) += ipmi_if.c
romstage-$(CONFIG_IPMI_KCS_ROMSTAGE) += ipmi_ops_premem.c
romstage-$(CONFIG_IPMI_KCS_ROMSTAGE) += ipmi_kcs.c
romstage-$(CONFIG_IPMI_KCS_ROMSTAGE) += ipmi_ops.c
smm-$(CONFIG_SOC_RAS_BMC_SEL) += ipmi_kcs.c

ramstage-$(CONFIG_IPMI_BT) += ipmi_if.c
ramstage-$(CONFIG_IPMI_BT) += ipmi_bt.c
ramstage-$(CONFIG_IPMI_BT) += ipmi_bt_ops.c
ramstage-$(CONFIG_IPMI_BT) += ipmi_ops.c
ramstage-$(CONFIG_IPMI_BT) += ipmi_fru.c
romstage-$(CONFIG_IPMI_BT_ROMSTAGE) += ipmi_if.c
romstage-$(CONFIG_IPMI_BT_ROMSTAGE) += ipmi_ops_premem.c
romstage-$(CONFIG_IPMI_BT_ROMSTAGE) += ipmi_bt.c
romstage-$(CONFIG_IPMI_BT_ROMSTAGE) += ipmi_ops.c
