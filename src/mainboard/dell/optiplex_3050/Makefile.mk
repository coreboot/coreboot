## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-y += sch5555_ec.c

romstage-y += romstage.c

ramstage-y += ramstage.c
ramstage-y += sch5555_ec.c
ramstage-y += hda_verb.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
