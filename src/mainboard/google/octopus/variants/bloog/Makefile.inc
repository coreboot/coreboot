bootblock-y += gpio.c

romstage-y += gpio.c

ramstage-y += gpio.c
ramstage-y += variant.c

$(call add_vbt_to_cbfs, vbt_blooguard.bin, blooguard-data.vbt)
