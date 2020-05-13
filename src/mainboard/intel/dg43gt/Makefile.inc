# SPDX-License-Identifier: GPL-2.0-only

ramstage-y += cstates.c
romstage-y += gpio.c

bootblock-y += early_init.c
romstage-y += early_init.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
