/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_SPD_H
#define MAINBOARD_SPD_H

#define SPD_LEN			256

#define SPD_DRAM_TYPE		2
#define SPD_DRAM_DDR3		0x0b
#define SPD_DRAM_LPDDR3		0xf1
#define SPD_DENSITY_BANKS	4
#define SPD_ADDRESSING		5
#define SPD_ORGANIZATION	7
#define SPD_BUS_DEV_WIDTH	8
#define SPD_PART_OFF		128
#define SPD_PART_LEN		18

/* Auron board memory configuration GPIOs */
#define SPD_GPIO_BIT0		13
#define SPD_GPIO_BIT1		9
#define SPD_GPIO_BIT2		47

struct pei_data;
void mainboard_fill_spd_data(struct pei_data *pei_data);

#endif
