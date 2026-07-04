## SPDX-License-Identifier: GPL-2.0-or-later

ramstage-$(CONFIG_EC_GOOGLE_CHROMEEC) += ec.c
smm-$(CONFIG_EC_GOOGLE_CHROMEEC) += smihandler.c

verstage-$(CONFIG_VBOOT) += bootmode.c
romstage-$(CONFIG_VBOOT) += bootmode.c
ramstage-$(CONFIG_VBOOT) += bootmode.c

ramstage-$(CONFIG_DRIVERS_OPTION_CFR) += cfr.c
