## SPDX-License-Identifier: GPL-2.0-only

all-y += memlayout.ld
all-y += chromeos.c
all-y += reset.c

bootblock-y += bootblock.c

romstage-y += romstage.c

ramstage-y += boardid.c
ramstage-y += mainboard.c
ramstage-y += panel.c
ramstage-y += panel_tps65132s.c
ramstage-y += regulator.c

ramstage-$(CONFIG_BOARD_GOOGLE_SAPPHIRE) += panel_sapphire.c

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include

subdirs-y += variants/$(VARIANT_DIR)
