/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
 * Copyright (c) 2013-2015, NVIDIA CORPORATION.  All rights reserved.
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

#include <boardid.h>
#include <console/console.h>
#include <delay.h>
#include <device/i2c.h>
#include <stdint.h>
#include <stdlib.h>

#include "pmic.h"
#include "reset.h"

enum {
	MAX77620_I2C_ADDR = 0x3c,
	MAX77621_CPU_I2C_ADDR = 0x1B,
	MAX77621_GPU_I2C_ADDR = 0x1C,
};

struct max77620_init_reg {
	u8 reg;
	u8 val;
	u8 delay;
};

static struct max77620_init_reg init_list[] = {
	/* TODO */
};

static void pmic_write_reg(unsigned bus, uint8_t chip, uint8_t reg, uint8_t val,
			   int delay)
{
	if (i2c_writeb(bus, chip, reg, val)) {
		printk(BIOS_ERR, "%s: reg = 0x%02X, value = 0x%02X failed!\n",
			__func__, reg, val);
		/* Reset the SoC on any PMIC write error */
		cpu_reset();
	} else {
		if (delay)
			udelay(500);
	}
}

void pmic_write_reg_77620(unsigned bus, uint8_t reg, uint8_t val,
					int delay)
{
	pmic_write_reg(bus, MAX77620_I2C_ADDR, reg, val, delay);
}

static inline void pmic_write_reg_77621(unsigned bus, uint8_t reg, uint8_t val,
					int delay)
{
	pmic_write_reg(bus, MAX77621_CPU_I2C_ADDR, reg, val, delay);
}

static void pmic_slam_defaults(unsigned bus)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(init_list); i++) {
		struct max77620_init_reg *reg = &init_list[i];
		pmic_write_reg_77620(bus, reg->reg, reg->val, reg->delay);
	}
}

void pmic_init(unsigned bus)
{
	/* Restore PMIC POR defaults, in case kernel changed 'em */
	pmic_slam_defaults(bus);

	/* Setup/Enable GPIO5 - VDD_CPU_REG_EN */
	pmic_write_reg_77620(bus, MAX77620_GPIO5_REG, 0x09, 1);

	/* Setup/Enable GPIO1 - VDD_HDMI_5V0_BST_EN -- ??? */
	pmic_write_reg_77620(bus, MAX77620_GPIO1_REG, 0x09, 1);

	/* GPIO 0,1,5,6,7 = GPIO, 2,3,4 = alt mode */
	pmic_write_reg_77620(bus, MAX77620_AME_GPIO, 0x1c, 1);

	/* Disable SD1 Remote Sense, Set SD1 for LPDDR4 to 1.125v? */
	pmic_write_reg_77620(bus, MAX77620_CNFG2SD_REG, 0x04, 1);

	pmic_write_reg_77620(bus, MAX77620_SD1_REG, 0x2a, 1);

	/* Max77621 VREG for CPU needs to be set to 0.85V as per SysEng */

	/* Max77621 CPU VREG, register 0, 0.85V = 0x27(0.60625 + (39*6.25mV)) */
	pmic_write_reg_77621(bus, MAX77621_VOUT_REG, 0x27, 1);
	pmic_write_reg_77621(bus, MAX77621_VOUT_REG, 0xa7, 1);

	/* Max77621 CPU VREG DVC, register 1, 0.85V = 0x27 */
	pmic_write_reg_77621(bus, MAX77621_VOUT_DVC_REG, 0x27, 1);
	pmic_write_reg_77621(bus, MAX77621_VOUT_DVC_REG, 0xa7, 1);

	printk(BIOS_DEBUG, "PMIC init done\n");
}
