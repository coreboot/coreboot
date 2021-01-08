## SPDX-License-Identifier: GPL-2.0-only

romstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-$(CONFIG_EC_GOOGLE_CHROMEEC) += ec.c
ramstage-y += irqroute.c
ramstage-y += w25q64.c

ramstage-y += variants/$(VARIANT_DIR)/gpio.c

ramstage-$(CONFIG_BOARD_GOOGLE_NINJA) += variants/$(VARIANT_DIR)/lan.c
ramstage-$(CONFIG_BOARD_GOOGLE_SUMO) += variants/$(VARIANT_DIR)/lan.c

subdirs-y += variants/$(VARIANT_DIR)

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include
