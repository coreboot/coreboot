bootblock-y += gpio.c
romstage-y += gpio.c
ramstage-$(CONFIG_FW_CONFIG) += fw_config.c
ramstage-$(CONFIG_FW_CONFIG) += variant.c
ramstage-y += gpio.c
ramstage-y += ramstage.c
