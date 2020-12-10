ifeq ($(CONFIG_PLATFORM_USES_FSP2_0),y)
romstage-y += fsp_reset.c
ramstage-y += fsp_reset.c
endif # CONFIG_PLATFORM_USES_FSP2_0
