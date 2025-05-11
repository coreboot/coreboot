## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-$(CONFIG_CHROMEOS) += chromeos.c

verstage-$(CONFIG_CHROMEOS) += chromeos.c

romstage-$(CONFIG_CHROMEOS) += chromeos.c

ramstage-$(CONFIG_DRIVERS_OPTION_CFR) += cfr.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-$(CONFIG_EC_GOOGLE_CHROMEEC) += ec.c
ramstage-y += mainboard.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += variants/$(VARIANT_DIR)/gma-mainboard.ads

subdirs-y += variants/baseboard
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include

subdirs-y += variants/$(VARIANT_DIR)
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include
