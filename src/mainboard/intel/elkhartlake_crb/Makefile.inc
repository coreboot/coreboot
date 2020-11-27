## SPDX-License-Identifier: GPL-2.0-only

subdirs-y += spd

bootblock-y += bootblock.c

romstage-y += romstage_fsp_params.c
romstage-y += board_id.c

ramstage-y += mainboard.c
ramstage-y += board_id.c

smm-y += smihandler.c

subdirs-y += variants/baseboard
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include

subdirs-y += variants/$(VARIANT_DIR)
