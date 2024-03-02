# SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += early_init.c
bootblock-y += gpio.c
bootblock-y += led.c

romstage-y += early_init.c
romstage-y += gpio.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads

smm-y += led.c
