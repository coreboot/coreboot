# SPDX-License-Identifier: GPL-2.0-only

bootblock-y += early_init.c

romstage-y += gpio.c
romstage-y += early_init.c

ramstage-y += cstates.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
