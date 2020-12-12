## SPDX-License-Identifier: GPL-2.0-only

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include

ramstage-y += ramstage.c
ramstage-y += variants/$(VARIANT_DIR)/hda_verb.c
ramstage-y += variants/$(VARIANT_DIR)/gpio.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += variants/$(VARIANT_DIR)/gma-mainboard.ads
