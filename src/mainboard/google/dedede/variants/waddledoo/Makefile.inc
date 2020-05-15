## SPDX-License-Identifier: GPL-2.0-or-later

SPD_SOURCES = empty					#0b0000
SPD_SOURCES += SPD_LPDDR4X_200b_8Gb_4267_DDP_1x16	#0b0001

romstage-y += memory.c

ramstage-y += variant.c
