/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Rockchip Inc.
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
#include <device/i2c.h>
#include <stdint.h>
#include <stdlib.h>
#include "pmic.h"

#define RK808_ADDR	0x1b
#define LOD1EN		(1 << 0)
#define LOD2EN		(1 << 1)
#define LOD3EN		(1 << 2)
#define LOD4EN		(1 << 3)
#define LOD5EN		(1 << 4)
#define LOD6EN		(1 << 5)
#define LOD7EN		(1 << 6)
#define LOD8EN		(1 << 7)

#define LDO_BASE18V	18
#define LDO_BASE08V	8
#define LDOVSEL(mV, base) (mV/100 - base)

struct rk808_reg {
	 u8 reg;
	 u8 val;
};

enum {
	 LDO_EN = 0x24,
	 LDO1_ONSEL = 0x3B,
	 LDO1_SLPSEL,
	 LDO2_ONSEL,
	 LDO2_SLPSEL,
	 LDO3_ONSEL,
	 LDO3_SLPSEL,
	 LDO4_ONSEL,
	 LDO4_SLPSEL,
	 LDO5_ONSEL,
	 LDO5_SLPSEL,
	 LDO6_ONSEL,
	 LDO6_SLPSEL,
	 LDO7_ONSEL,
	 LDO7_SLPSEL,
	 LDO8_ONSEL,
	 LDO8_SLPSEL,
};

static struct rk808_reg ldo_initlist[] = {
	 {LDO4_ONSEL, LDOVSEL(1800, LDO_BASE18V)}, /*vcc18_lcd*/
	 {LDO5_ONSEL, LDOVSEL(1800, LDO_BASE18V)}, /*vcc18_codec*/
	 {LDO6_ONSEL, LDOVSEL(1000, LDO_BASE08V)}, /*vcc10_lcd*/
	 {LDO8_ONSEL, LDOVSEL(3300, LDO_BASE18V)}, /*vccio_sd*/
};

void pmic_init(unsigned int bus)
{
	uint8_t read_reg;
	int i;
	for (i = 0; i < ARRAY_SIZE(ldo_initlist); i++) {
		struct rk808_reg *reg = &ldo_initlist[i];
		i2c_writeb(bus, RK808_ADDR, reg->reg, reg->val);
	}

	/*enable ldo4,ldo5,ldo6,ldo8*/
	i2c_readb(bus, RK808_ADDR, LDO_EN, &read_reg);
	i2c_writeb(bus, RK808_ADDR, LDO_EN, read_reg | LOD8EN | LOD6EN
						     | LOD5EN | LOD4EN);
}
