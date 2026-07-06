## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += gpio.c

romstage-y += romstage.c

ramstage-$(CONFIG_DRIVERS_OPTION_CFR) += cfr.c
ramstage-y += card_reader.c
ramstage-y += devtree.c
ramstage-y += gpio.c
ramstage-y += hda_verb.c
ramstage-y += mainboard_ssdt.c
ramstage-y += ramstage.c
