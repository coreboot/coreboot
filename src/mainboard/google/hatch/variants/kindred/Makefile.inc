## SPDX-License-Identifier: GPL-2.0-only

SPD_SOURCES = 4G_2400   # 0b000
SPD_SOURCES += empty_ddr4 # 0b001
SPD_SOURCES += 8G_2400    # 0b010
SPD_SOURCES += 8G_2666    # 0b011
SPD_SOURCES += 16G_2400   # 0b100
SPD_SOURCES += 16G_2666   # 0b101
SPD_SOURCES += 8G_3200    # 0b110
SPD_SOURCES += 16G_3200   # 0b111

bootblock-y += gpio.c
ramstage-y += gpio.c
ramstage-y += variant.c

$(call add_vbt_to_cbfs, vbt-kled.bin, kled-data.vbt)
