## SPDX-License-Identifier: GPL-2.0-only

subdirs-y += ./spd
subdirs-y += variants/baseboard
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include

subdirs-y += variants/$(VARIANT_DIR)
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include

bootblock-y += bootblock.c

ramstage-y += hda_verb.c
ramstage-y += mainboard.c
ramstage-y += ramstage.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += variants/$(VARIANT_DIR)/gma-mainboard.ads
