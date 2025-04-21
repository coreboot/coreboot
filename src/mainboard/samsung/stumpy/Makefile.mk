## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += early_init.c
bootblock-y += gpio.c

romstage-$(CONFIG_CHROMEOS) += chromeos.c
romstage-y += early_init.c
romstage-y += gpio.c

ramstage-$(CONFIG_DRIVERS_OPTION_CFR) += cfr.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
