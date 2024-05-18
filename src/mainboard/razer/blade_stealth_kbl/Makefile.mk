## SPDX-License-Identifier: GPL-2.0-only

subdirs-y += spd

ramstage-y += ramstage.c
ramstage-y += variants/$(VARIANT_DIR)/hda_verb.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include
