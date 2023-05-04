bootblock-y += gpio.c

ramstage-$(CONFIG_FW_CONFIG) += fw_config.c
ramstage-y += gpio.c
ramstage-y += tpm_tis.c

romstage-y += gpio.c
romstage-y += tpm_tis.c

verstage-y += tpm_tis.c

smm-y += smihandler.c
