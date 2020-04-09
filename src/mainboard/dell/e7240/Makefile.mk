## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
romstage-y += gpio.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
