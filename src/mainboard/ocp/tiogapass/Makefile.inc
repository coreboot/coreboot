## SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += bootblock.c
romstage-y += ipmi.c
ramstage-y += ramstage.c ipmi.c

all-y += console.c

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include
