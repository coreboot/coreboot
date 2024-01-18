## SPDX-License-Identifier: GPL-2.0-only

ramstage-y += ec.c

romstage-y += chromeos.c
ramstage-y += chromeos.c

smm-y += ec.c

SRC_ROOT = $(src)/mainboard/google/stout
bootblock-y += gpio.c
romstage-y += gpio.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
bootblock-y += early_init.c
romstage-y += early_init.c
