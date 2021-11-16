## SPDX-License-Identifier: GPL-2.0-or-later

ramstage-$(CONFIG_FW_CONFIG) += variant.c
ramstage-y += gpio.c

$(call add_vbt_to_cbfs, vbt_magister.bin, magister-data.vbt)
