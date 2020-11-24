## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c

ramstage-y += mainboard.c

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include

subdirs-y += variants/$(VARIANT_DIR)
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include
