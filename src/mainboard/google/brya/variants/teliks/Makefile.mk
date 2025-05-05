# SPDX-License-Identifier: GPL-2.0-only
bootblock-y += gpio.c

romstage-y += memory.c
romstage-y += gpio.c

ramstage-y += gpio.c

ramstage-y += variant.c

$(call add_vbt_to_cbfs, vbt-teliks_panel_11_inch.bin, data-teliks_panel_11_inch.vbt)
