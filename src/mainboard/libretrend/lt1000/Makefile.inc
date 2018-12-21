## SPDX-License-Identifier: GPL-2.0-or-later
## This file is part of the coreboot project.

bootblock-y += bootblock.c

ramstage-y += ramstage.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
