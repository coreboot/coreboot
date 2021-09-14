# SPDX-License-Identifier: GPL-2.0-only

ramstage-y += panel.c

panel-params-y :=

panel-params-$(CONFIG_MIPI_PANEL_AUO_B101UAN08_3) += panel-AUO_B101UAN08_3
panel-params-$(CONFIG_MIPI_PANEL_AUO_KD101N80_45NA) += panel-AUO_KD101N80_45NA
panel-params-$(CONFIG_MIPI_PANEL_AUO_NT51021D8P) += panel-AUO_NT51021D8P

panel-params-$(CONFIG_MIPI_PANEL_BOE_TV080WUM_NG0) += panel-BOE_TV080WUM_NG0
panel-params-$(CONFIG_MIPI_PANEL_BOE_TV101WUM_N53) += panel-BOE_TV101WUM_N53
panel-params-$(CONFIG_MIPI_PANEL_BOE_TV101WUM_NG0) += panel-BOE_TV101WUM_NG0
panel-params-$(CONFIG_MIPI_PANEL_BOE_TV101WUM_NL6) += panel-BOE_TV101WUM_NL6
panel-params-$(CONFIG_MIPI_PANEL_BOE_TV105WUM_NW0) += panel-BOE_TV105WUM_NW0
panel-params-$(CONFIG_MIPI_PANEL_BOE_TV110C9M_LL0) += panel-BOE_TV110C9M_LL0

panel-params-$(CONFIG_MIPI_PANEL_CMN_P097PFG_SSD2858) += panel-CMN_P097PFG_SSD2858

panel-params-$(CONFIG_MIPI_PANEL_INX_OTA7290D10P) += panel-INX_OTA7290D10P
panel-params-$(CONFIG_MIPI_PANEL_INX_P110ZZD_DF0) += panel-INX_P110ZZD_DF0

panel-params-$(CONFIG_MIPI_PANEL_STA_2081101QFH032011_53G) += panel-STA_2081101QFH032011_53G

panel-params-$(CONFIG_MIPI_PANEL_VIS_RM69299) += panel-VIS_RM69299

$(foreach params,$(panel-params-y), \
	$(eval cbfs-files-y += $(params)) \
	$(eval $(params)-file := $(params).c:struct) \
	$(eval $(params)-type := struct) \
	$(eval $(params)-compression := $(CBFS_COMPRESS_FLAG)) \
)
