# SPDX-License-Identifier: GPL-2.0-only

bootblock-y += gpio.c

romstage-y += memory.c

ramstage-y += gpio.c

ramstage-$(CONFIG_FW_CONFIG) += variant.c

$(call add_vbt_to_cbfs, vbt-oled.bin, data-oled.vbt)
