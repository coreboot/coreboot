bootblock-y += gpio.c

romstage-y += gpio.c

ramstage-y += gpio.c
ramstage-y += variant.c

smm-y += variant.c

$(call add_vbt_to_cbfs, vbt_garg_hdmi.bin, garg-hdmi-data.vbt)
$(call add_vbt_to_cbfs, vbt_garfour.bin, garfour-data.vbt)
$(call add_vbt_to_cbfs, vbt_garfour_hdmi.bin, garfour-hdmi-data.vbt)
