## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-y += variants/$(VARIANT_DIR)/led.c

romstage-$(CONFIG_CHROMEOS) += chromeos.c
romstage-y += variants/$(VARIANT_DIR)/gpio.c

ramstage-$(CONFIG_DRIVERS_OPTION_CFR) += cfr.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
ramstage-y += lan.c

verstage-y += chromeos.c

smm-y += variants/$(VARIANT_DIR)/led.c

subdirs-y += variants/$(VARIANT_DIR)
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include
