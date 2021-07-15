## SPDX-License-Identifier: GPL-2.0-only

all-y += chromeos.c
ifneq ($(CONFIG_BOARD_GOOGLE_SENOR),y)
all-y += boardid.c
all-y += reset.c
endif
bootblock-y += bootblock.c

romstage-y += romstage.c

ramstage-y += mainboard.c
