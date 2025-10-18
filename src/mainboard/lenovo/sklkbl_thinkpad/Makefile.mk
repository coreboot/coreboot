## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c

romstage-y += variants/$(VARIANT_DIR)/memory_init_params.c

ramstage-$(CONFIG_DRIVERS_OPTION_CFR) += cfr.c
ramstage-$(CONFIG_VARIANT_HAS_DGPU) += dgpu.c
ramstage-y += variants/$(VARIANT_DIR)/gpio.c
ramstage-y += variants/$(VARIANT_DIR)/hda_verb.c
ramstage-y += ramstage.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += variants/$(VARIANT_DIR)/gma-mainboard.ads

subdirs-y += variants/$(VARIANT_DIR)

ifeq ($(CONFIG_VARIANT_HAS_DGPU),y)
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include
endif
