## SPDX-License-Identifier: GPL-2.0-only

subdirs-y += spd

bootblock-y += bootblock_mainboard.c

bootblock-$(CONFIG_CHROMEOS) += chromeos.c
verstage-$(CONFIG_CHROMEOS) += chromeos.c
romstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c

ramstage-$(CONFIG_EC_GOOGLE_CHROMEEC) += ec.c

ramstage-y += mainboard.c
ramstage-y += ramstage.c
