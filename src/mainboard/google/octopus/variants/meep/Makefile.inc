bootblock-y += gpio.c

romstage-y += gpio.c

ramstage-y += gpio.c
ramstage-y += variant.c

$(call add_vbt_to_cbfs, vbt_dorp_hdmi.bin, dorp-hdmi-data.vbt)
$(call add_vbt_to_cbfs, vbt_vortininja.bin, vortininja-data.vbt)
