## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c

romstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c
verstage-y += chromeos.c
ramstage-y += lan.c

smm-y += variants/$(VARIANT_DIR)/led.c

romstage-y += variants/$(VARIANT_DIR)/gpio.c

bootblock-y += variants/$(VARIANT_DIR)/led.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
ramstage-$(CONFIG_DRIVERS_OPTION_CFR) += cfr.c

subdirs-y += variants/$(VARIANT_DIR)
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include
