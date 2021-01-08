## SPDX-License-Identifier: GPL-2.0-only

subdirs-y += spd

bootblock-y += gpio.c
romstage-y += gpio.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
bootblock-y += early_init.c
romstage-y += early_init.c
