## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += gpio.c
romstage-y += gpio.c

bootblock-y += variants/$(VARIANT_DIR)/early_init.c
romstage-y += variants/$(VARIANT_DIR)/early_init.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += variants/$(VARIANT_DIR)/gma-mainboard.ads

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include
bootblock-y += early_init.c
romstage-y += early_init.c
