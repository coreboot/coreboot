## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c

romstage-y += hsio.c

ramstage-y += ramstage.c
ramstage-y += hsio.c

CPPFLAGS_common += -Isrc/mainboard/$(MAINBOARDDIR)/
