/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 * Copyright (c) 2013, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <console/console.h>
#include <delay.h>
#include <device/i2c.h>
#include <stdint.h>
#include <stdlib.h>

#include "pmic.h"
#include "reset.h"

#define PAGE_ADDR(reg)		((reg >> 8) & 0xff)
#define PAGE_OFFSET(reg)	(reg & 0xff)

struct ti65913_init_reg {
	u16 reg;
	u8 val;
	u8 delay;
};

static struct ti65913_init_reg init_list[] = {
//TODO(twarren@nvidia.com): Add slams back to defaults
//	{TI65913_SMPS12_VOLTAGE, 0x38, 0},
//	{TI65913_SMPS12_CTRL, 0x01, 1},
//etc.
};

int pmic_read_reg(unsigned bus, uint16_t reg, uint8_t *data)
{
	if (i2c_readb(bus, PAGE_ADDR(reg), PAGE_OFFSET(reg), data)) {
		printk(BIOS_ERR, "%s: page = 0x%02X, reg = 0x%02X failed!\n",
			 __func__, PAGE_ADDR(reg), PAGE_OFFSET(reg));
		return -1;
	}
	return 0;
}

void pmic_write_reg(unsigned bus, uint16_t reg, uint8_t val, int delay)
{
	if (i2c_writeb(bus, PAGE_ADDR(reg), PAGE_OFFSET(reg), val)) {
		printk(BIOS_ERR, "%s: page = 0x%02X, reg = 0x%02X, "
			"value = 0x%02X failed!\n",
			__func__, PAGE_ADDR(reg), PAGE_OFFSET(reg), val);
		/* Reset the SoC on any PMIC write error */
		cpu_reset();
	} else {
		if (delay)
			udelay(500);
	}
}

static void pmic_slam_defaults(unsigned bus)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(init_list); i++) {
		struct ti65913_init_reg *reg = &init_list[i];
		pmic_write_reg(bus, reg->reg, reg->val, reg->delay);
	}
}

void pmic_init(unsigned bus)
{
	/* Restore PMIC POR defaults, in case kernel changed 'em */
	pmic_slam_defaults(bus);

	/* A44: Set VDD_CPU to 1.0V. */
	pmic_write_reg(bus, TI65913_SMPS12_VOLTAGE, 0x38, 0);
	pmic_write_reg(bus, TI65913_SMPS12_CTRL, 0x01, 1);

	printk(BIOS_DEBUG, "PMIC init done\n");
}
