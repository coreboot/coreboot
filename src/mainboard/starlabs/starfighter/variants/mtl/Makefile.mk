## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += gpio.c

romstage-y += board_id.c
romstage-y += romstage.c

ramstage-y += board_id.c
ramstage-y += devtree.c
ramstage-y += gpio.c
ramstage-y += hda_verb.c
ramstage-y += ramstage.c

$(call add_vbt_to_cbfs, vbt_native_res.bin, data_native_res.vbt)
$(call add_vbt_to_cbfs, vbt_qhd.bin, data_qhd.vbt)
