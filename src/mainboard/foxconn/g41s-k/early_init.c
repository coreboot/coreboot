/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Damien Zammit <damien@zamaudio.com>
 * Copyright (C) 2017 Arthur Heymans <arthur@aheymans.xyz>
 * Copyright (C) 2017 Samuel Holland <samuel@sholland.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <bootblock_common.h>
#include <northbridge/intel/x4x/x4x.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8720f/it8720f.h>

#define SERIAL_DEV PNP_DEV(0x2e, IT8720F_SP1)
#define GPIO_DEV PNP_DEV(0x2e, IT8720F_GPIO)

void bootblock_mainboard_early_init(void)
{
	/* Set up GPIOs on Super I/O. */
	ite_reg_write(GPIO_DEV, 0x25, 0x01);
	ite_reg_write(GPIO_DEV, 0x26, 0x04);
	ite_reg_write(GPIO_DEV, 0x27, 0x00);
	ite_reg_write(GPIO_DEV, 0x28, 0x40);
	ite_reg_write(GPIO_DEV, 0x29, 0x01);
	ite_reg_write(GPIO_DEV, 0x73, 0x00);
	ite_reg_write(GPIO_DEV, 0x74, 0x00);
	ite_reg_write(GPIO_DEV, 0xb1, 0x04);
	ite_reg_write(GPIO_DEV, 0xb8, 0x20);
	ite_reg_write(GPIO_DEV, 0xbb, 0x01);
	ite_reg_write(GPIO_DEV, 0xc0, 0x00);
	ite_reg_write(GPIO_DEV, 0xc3, 0x01);
	ite_reg_write(GPIO_DEV, 0xcb, 0x01);
	ite_reg_write(GPIO_DEV, 0xf5, 0x28);
	ite_reg_write(GPIO_DEV, 0xf6, 0x12);
	ite_enable_3vsbsw(GPIO_DEV);

	/* Set up IRQ routing. */
	RCBA16(D31IR) = 0x0132;
	RCBA16(D30IR) = 0x3241;
	RCBA16(D29IR) = 0x0237;

	RCBA32(FD) |= FD_INTLAN;

	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}

void mb_get_spd_map(u8 spd_map[4])
{
	spd_map[0] = 0x50;
	if (CONFIG(BOARD_FOXCONN_G41M))
		spd_map[2] = 0x52;
}
