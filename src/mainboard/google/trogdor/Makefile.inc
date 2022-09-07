## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += boardid.c
bootblock-y += chromeos.c
bootblock-y += bootblock.c

ifneq ($(CONFIG_BOARD_GOOGLE_BUBS),y)
verstage-y += reset.c
endif
verstage-y += boardid.c
verstage-y += chromeos.c

romstage-y += romstage.c
romstage-y += boardid.c
romstage-y += chromeos.c
ifneq ($(CONFIG_BOARD_GOOGLE_BUBS),y)
romstage-y += reset.c
endif

ramstage-y += mainboard.c
ifneq ($(CONFIG_BOARD_GOOGLE_BUBS),y)
ramstage-y += reset.c
endif
ramstage-y += chromeos.c
ramstage-y += boardid.c
