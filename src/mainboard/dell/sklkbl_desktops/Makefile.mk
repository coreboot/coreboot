## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-y += variants/$(VARIANT_DIR)/early_gpio.c
bootblock-y += sch5555_ec.c

romstage-y += variants/$(VARIANT_DIR)/romstage.c

ramstage-y += ramstage.c
ramstage-y += variants/$(VARIANT_DIR)/gpio.c
ramstage-y += sch5555_ec.c
ramstage-y += variants/$(VARIANT_DIR)/hda_verb.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += variants/$(VARIANT_DIR)/gma-mainboard.ads

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include
