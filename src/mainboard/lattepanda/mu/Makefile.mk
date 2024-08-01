## SPDX-License-Identifier: GPL-2.0-only

subdirs-y += spd

bootblock-y += bootblock.c
bootblock-y += early_gpio.c

romstage-y += romstage_fsp_params.c
romstage-y += memory.c

ramstage-y += mainboard.c
ramstage-y += gpio.c
ramstage-y += ramstage.c

smm-y += smihandler.c

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include
