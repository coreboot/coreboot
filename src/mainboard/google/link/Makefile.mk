## SPDX-License-Identifier: GPL-2.0-only

ramstage-y += ec.c

romstage-y += chromeos.c
ramstage-y += chromeos.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads

# Order of names in SPD_SOURCES is important!
SPD_SOURCES = elpida_4Gb_1600_x16
SPD_SOURCES += samsung_4Gb_1600_1.35v_x16
SPD_SOURCES += micron_4Gb_1600_1.35v_x16

bootblock-y += gpio.c
romstage-y += gpio.c
bootblock-y += early_init.c
romstage-y += early_init.c
