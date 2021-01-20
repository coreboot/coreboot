## SPDX-License-Identifier: GPL-2.0-only

SPD_SOURCES = LP_8G_2133	# 0b000
SPD_SOURCES += LP_16G_2133	# 0b001
SPD_SOURCES += LP_4G_2133	# 0b010

romstage-y += memory.c

bootblock-y += gpio.c

ramstage-y += gpio.c
ramstage-y += ramstage.c
ramstage-y += variant.c
