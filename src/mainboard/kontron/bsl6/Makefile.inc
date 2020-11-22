## SPDX-License-Identifier: GPL-2.0-only

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include

bootblock-y += bootblock.c

romstage-y += variants/$(VARIANT_DIR)/romstage.c

ramstage-y += ramstage.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += variants/$(VARIANT_DIR)/gma-mainboard.ads
