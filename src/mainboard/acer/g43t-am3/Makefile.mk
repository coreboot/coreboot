# SPDX-License-Identifier: GPL-2.0-only

ramstage-y += cstates.c
romstage-y += variants/$(VARIANT_DIR)/gpio.c

bootblock-y += variants/$(VARIANT_DIR)/early_init.c
romstage-y += variants/$(VARIANT_DIR)/early_init.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include
