## SPDX-License-Identifier: GPL-2.0-only

ramstage-$(CONFIG_EC_GOOGLE_CHROMEEC) += ec.c

romstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c
bootblock-$(CONFIG_CHROMEOS) += chromeos.c

smm-y += smihandler.c

romstage-$(CONFIG_HAVE_SPD_IN_CBFS) += spd.c

romstage-y += variants/$(VARIANT_DIR)/pei_data.c
ramstage-y += variants/$(VARIANT_DIR)/pei_data.c

ifneq ($(CONFIG_BOARD_GOOGLE_BUDDY),y)
romstage-y += variants/$(VARIANT_DIR)/variant.c
endif
ramstage-y += variants/$(VARIANT_DIR)/variant.c

subdirs-y += variants/$(VARIANT_DIR)
subdirs-y += variants/$(VARIANT_DIR)/spd

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include

romstage-y += variants/$(VARIANT_DIR)/gpio.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
