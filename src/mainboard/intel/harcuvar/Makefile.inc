## SPDX-License-Identifier: GPL-2.0-only

subdirs-y += spd

romstage-y += boardid.c
romstage-y += hsio.c

ramstage-y += ramstage.c
ramstage-y += boardid.c
ramstage-y += hsio.c

CPPFLAGS_common += -Isrc/mainboard/$(MAINBOARDDIR)/
