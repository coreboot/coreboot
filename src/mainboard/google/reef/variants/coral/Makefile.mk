## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += gpio.c

ramstage-y += mainboard.c
ramstage-y += gpio.c

smm-y += gpio.c

$(call add_vbt_to_cbfs, vbt-astronaut.bin, astronaut-data.vbt)
$(call add_vbt_to_cbfs, vbt-babytiger.bin, babytiger-data.vbt)
$(call add_vbt_to_cbfs, vbt-babymega.bin, babymega-data.vbt)
$(call add_vbt_to_cbfs, vbt-epaulette.bin, epaulette-data.vbt)
$(call add_vbt_to_cbfs, vbt-nasher.bin, nasher-data.vbt)
$(call add_vbt_to_cbfs, vbt-rabbid_rugged.bin, rabbid_rugged-data.vbt)
$(call add_vbt_to_cbfs, vbt-santa.bin, santa-data.vbt)
