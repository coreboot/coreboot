## SPDX-License-Identifier: GPL-2.0-or-later

ramstage-y += gpio.c
ramstage-y += ramstage.c
ramstage-$(CONFIG_FW_CONFIG) += variant.c

smm-y += variant.c

$(call add_vbt_to_cbfs, vbt_drawman.bin, drawman-data.vbt)
