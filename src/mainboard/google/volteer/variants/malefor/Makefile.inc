# SPDX-License-Identifier: GPL-2.0-only

## Memory Options					  # DRAM ID  # Part Num
SPD_SOURCES = SPD_LPDDR4X_200b_8bank_1Rx16_16Gb_DDP_4267  # 0b0000   # MT53E512M32D2NP-046

romstage-y += memory.c

bootblock-y += gpio.c

ramstage-y += gpio.c
