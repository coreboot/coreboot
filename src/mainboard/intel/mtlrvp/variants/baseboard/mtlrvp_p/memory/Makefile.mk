## SPDX-License-Identifier: GPL-2.0-or-later

romstage-y += memory.c

SPD_SOURCES =
SPD_SOURCES += spd/lp5/set-0/spd-empty.hex	# ID = 0(0b0000)
SPD_SOURCES += spd/lp5/set-0/spd-empty.hex	# ID = 1(0b0001)
SPD_SOURCES += spd/lp5/set-0/spd-4.hex		# ID = 2(0b0010)  Parts = MT62F2G32D8DR-031 WT:B
