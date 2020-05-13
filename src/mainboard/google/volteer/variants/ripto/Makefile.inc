## SPDX-License-Identifier: GPL-2.0-only

SPD_SOURCES = SPD_LPDDR4X_200b_8bank_1Rx16_16Gb_DDP_4267   # 0b0000

bootblock-y += gpio.c

ramstage-y += gpio.c
