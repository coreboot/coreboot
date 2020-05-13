## SPDX-License-Identifier: GPL-2.0-only

smm-y += smihandler.c
romstage-y += gpio.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
