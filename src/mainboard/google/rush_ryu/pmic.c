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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <delay.h>
#include <device/i2c.h>
#include <stdint.h>
#include <stdlib.h>
#include "pmic.h"
#include "reset.h"

/* A44/Ryu has a TI 65913 PMIC on bus 4 (PWR_I2C) */

enum {
	TI65913_I2C_ADDR = 0x58
};

struct ti65913_init_reg {
	u8 reg;
	u8 val;
	u8 delay;
};

static struct ti65913_init_reg init_list[] = {
//TODO(twarren@nvidia.com): Add slams back to defaults
//	{TI65913_SMPS12_CTRL, 0x01, 0},
//	{TI65913_SMPS12_VOLTAGE, 0x38, 0},
//etc.
};

void pmic_write_reg(unsigned bus, uint8_t reg, uint8_t val, int delay)
{
	if (i2c_writeb(bus, TI65913_I2C_ADDR, reg, val)) {
		printk(BIOS_ERR, "%s: reg = 0x%02X, value = 0x%02X failed!\n",
			__func__, reg, val);
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
	/* Don't need to set up VDD_CORE - already done - by EC ?? */

	/* Restore PMIC POR defaults, in case kernel changed 'em */
	pmic_slam_defaults(bus);

	/* A44: Set VDD_CPU to 1.0V. */
	pmic_write_reg(bus, TI65913_SMPS12_CTRL, 0x01, 1);
	pmic_write_reg(bus, TI65913_SMPS12_VOLTAGE, 0x38, 0);

	printk(BIOS_DEBUG, "PMIC init done\n");
}
