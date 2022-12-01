## SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += board_id.c
romstage-y += board_id.c
ramstage-y += board_id.c

CPPFLAGS_common += -I$(src)/ec/intel
