bootblock-y += gpio.c
romstage-y += gpio.c
ramstage-y += gpio.c
ramstage-y += variant.c
ramstage-$(CONFIG_FW_CONFIG) += fw_config.c
