# SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += bootblock.c
bootblock-y += gpio.c

romstage-y += eeprom.c

ramstage-y += gpio.c
ramstage-y += ramstage.c
ramstage-y += mainboard.c
ramstage-y += eeprom.c
ramstage-y += smbios.c

$(call add_vbt_to_cbfs, vbt-avalanche.bin, avalanche-data.vbt)
