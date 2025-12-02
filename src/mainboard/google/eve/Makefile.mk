## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-$(CONFIG_CHROMEOS) += chromeos.c

verstage-$(CONFIG_CHROMEOS) += chromeos.c

romstage-$(CONFIG_CHROMEOS) += chromeos.c

ramstage-$(CONFIG_DRIVERS_OPTION_CFR) += cfr.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-$(CONFIG_EC_GOOGLE_CHROMEEC) += ec.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
ramstage-y += mainboard.c

subdirs-y += spd
