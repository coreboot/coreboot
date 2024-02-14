## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c

romstage-y += gpio.c
romstage-y += romstage_fsp_params.c

ramstage-y += mainboard.c

all-y += die.c
smm-y += die.c

all-y += board_beep.c
smm-y += board_beep.c
