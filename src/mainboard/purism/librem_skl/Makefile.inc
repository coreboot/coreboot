##
## This file is part of the coreboot project.
##
##
## SPDX-License-Identifier: GPL-2.0-only

ramstage-y += ramstage.c
ramstage-y += hda_verb.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
