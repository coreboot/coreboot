## SPDX-License-Identifier: GPL-2.0-only

romstage-y += romstage.c
romstage-y += gpio.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
ramstage-y += hda_verb.c
