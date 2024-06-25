## SPDX-License-Identifier: GPL-2.0-only

romstage-y += board_id.c
romstage-y += gpio.c

ramstage-y += board_id.c
ramstage-y += ramstage.c

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/
