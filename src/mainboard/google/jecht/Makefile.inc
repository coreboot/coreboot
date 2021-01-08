## SPDX-License-Identifier: GPL-2.0-only

subdirs-y += spd
romstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c
verstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-y += lan.c

smm-y += led.c

romstage-y += variants/$(VARIANT_DIR)/pei_data.c
ramstage-y += variants/$(VARIANT_DIR)/pei_data.c

bootblock-y += led.c

bootblock-y += bootblock.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads

subdirs-y += variants/$(VARIANT_DIR)
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include

romstage-y += variants/$(VARIANT_DIR)/gpio.c
