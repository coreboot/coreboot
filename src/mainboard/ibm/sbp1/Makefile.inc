## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
romstage-y += romstage.c
ramstage-y += ramstage.c
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include
