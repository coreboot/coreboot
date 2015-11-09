/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <arch/io.h>
#include <boardid.h>

/**
 *Bettong uses 3 GPIO(5-7) pins to identify board.
 *The GPIO ports are mapped to MMIO space.
 *The GPIO value and board version are mapped as follow:
 *GPIO5 GPIO6 GPIO7 Version
 *  0     0     0      A
 *  0     0     1      B
 *  ......
 *  1     1     1      H
 */
uint8_t board_id(void)
{
	void *gpiommioaddr;
	u8  value = 0;
	u8  boardrev = 0;
	char boardid;

	gpiommioaddr = (void *)0xfed80000ul + 0x1500;
	value = read8(gpiommioaddr + (7 << 2) + 2); /* agpio7: board_id2 */
	boardrev = value & 1;
	value = read8(gpiommioaddr + (6 << 2) + 2); /* agpio6: board_id1 */
	boardrev |= (value & 1) << 1;
	value = read8(gpiommioaddr + (5 << 2) + 2); /* agpio5: board_id0 */
	boardrev |= (value & 1) << 2;

	boardid = 'A' + boardrev;

	return boardid;
}
