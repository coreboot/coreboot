panel-params-y :=
panel-params-$(CONFIG_BOARD_GOOGLE_KRANE) += panel-AUO_KD101N80_45NA
panel-params-$(CONFIG_BOARD_GOOGLE_KRANE) += panel-BOE_TV101WUM_NL6
panel-params-$(CONFIG_BOARD_GOOGLE_KODAMA) += panel-BOE_TV101WUM_N53
panel-params-$(CONFIG_BOARD_GOOGLE_KUKUI) += panel-CMN_P097PFG_SSD2858

$(foreach params,$(panel-params-y), \
	$(eval cbfs-files-y += $(params)) \
	$(eval $(params)-file := $(params).c:struct) \
	$(eval $(params)-type := struct) \
	$(eval $(params)-compression := $(CBFS_COMPRESS_FLAG)) \
)
