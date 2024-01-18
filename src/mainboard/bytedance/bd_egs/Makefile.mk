## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c gpio.c
romstage-y += romstage.c
ramstage-y += ramstage.c gpio.c
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include
