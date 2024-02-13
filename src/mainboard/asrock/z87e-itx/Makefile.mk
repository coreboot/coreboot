## SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += bootblock.c
romstage-y += gpio.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
