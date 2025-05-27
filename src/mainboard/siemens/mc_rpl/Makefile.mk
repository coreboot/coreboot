## SPDX-License-Identifier: GPL-2.0-only

subdirs-y += spd

bootblock-y += bootblock.c

romstage-y += romstage_fsp_params.c
romstage-y += board_id.c
romstage-y += memory.c
ifeq ($(CONFIG_BOARD_INTEL_ADLRVP_RPL_EXT_EC),y)
romstage-y += memory_rpl.c
endif

ramstage-y += mainboard.c
ramstage-y += board_id.c
ramstage-y += ramstage.c

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include

subdirs-y += variants/$(VARIANT_DIR)
