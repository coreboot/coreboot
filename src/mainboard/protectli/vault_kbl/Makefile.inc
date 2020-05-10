## SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += bootblock.c

ramstage-y += ramstage.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
