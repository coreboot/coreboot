## SPDX-License-Identifier: GPL-2.0-only

romstage-y += chromeos.c
ramstage-y += chromeos.c
bootblock-y += gpio.c
romstage-y += gpio.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
bootblock-y += early_init.c
romstage-y += early_init.c

ramstage-$(CONFIG_DRIVERS_OPTION_CFR) += cfr.c
