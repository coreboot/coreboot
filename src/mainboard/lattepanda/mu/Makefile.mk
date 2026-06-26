## SPDX-License-Identifier: GPL-2.0-only

subdirs-y += spd
subdirs-y += variants/baseboard
subdirs-y += $(addprefix variants/,$(VARIANT_DIR))

bootblock-y += bootblock.c

romstage-y += romstage_fsp_params.c
romstage-y += memory.c

ramstage-y += mainboard.c
ramstage-y += ramstage.c

smm-y += smihandler.c

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include
