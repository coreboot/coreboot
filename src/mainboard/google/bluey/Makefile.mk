## SPDX-License-Identifier: GPL-2.0-only

all-y += boardid.c
all-y += chromeos.c
ifneq ($(CONFIG_MISSING_BOARD_RESET),y)
all-y += reset.c
endif

bootblock-y += bootblock.c

romstage-y += romstage.c

ramstage-y += mainboard.c
