/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef MAINBOARD_SPD_H
#define MAINBOARD_SPD_H

#define SPD_LEN			256

#define SPD_DRAM_TYPE		2
#define  SPD_DRAM_DDR3		0x0b
#define  SPD_DRAM_LPDDR3	0xf1
#define SPD_DENSITY_BANKS	4
#define SPD_ADDRESSING		5
#define SPD_ORGANIZATION	7
#define SPD_BUS_DEV_WIDTH	8
#define SPD_PART_OFF		128
#define  SPD_PART_LEN		18
#define SPD_MANU_OFF		148

int mainboard_get_spd_index(void);
uintptr_t mainboard_get_spd_data(void);

#endif
