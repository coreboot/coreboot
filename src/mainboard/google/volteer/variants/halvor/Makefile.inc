# SPDX-License-Identifier: GPL-2.0-only

## Memory Options                                      # DRAM ID  # Part Num
SPD_SOURCES = SPD_LPDDR4X_556b_1R_32Gb_8GbD_QDP_4267   # b0000    # H9HKNNNCRMBVAR-NEH
SPD_SOURCES += SPD_LPDDR4X_556b_1R_64Gb_16GbD_QDP_4267 # b0001    # MT53E1G64D4SQ-046 WT:A

bootblock-y += gpio.c

ramstage-y += gpio.c
