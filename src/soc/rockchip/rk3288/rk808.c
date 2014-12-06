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

#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <device/i2c.h>
#include <soc/rk808.h>
#include <stdint.h>
#include <stdlib.h>

#define RK808_ADDR	0x1b

#define DCDC_EN		0x23
#define LDO_EN		0x24
#define BUCK1SEL	0x2f
#define BUCK4SEL	0x38
#define LDO_ONSEL(i)	(0x39 + 2 * i)
#define LDO_SLPSEL(i)	(0x3a + 2 * i)

static void rk808_clrsetbits(uint8_t bus, uint8_t reg, uint8_t clr, uint8_t set)
{
	uint8_t value;

	if (i2c_readb(bus, RK808_ADDR, reg, &value) ||
	    i2c_writeb(bus, RK808_ADDR, reg, (value & ~clr) | set))
		printk(BIOS_ERR, "ERROR: Cannot set Rk808[%#x]!\n", reg);
}

void rk808_configure_switch(uint8_t bus, int sw, int enabled)
{
	assert(sw == 1 || sw == 2);
	rk808_clrsetbits(bus, DCDC_EN, 1 << (sw + 4), !!enabled << (sw + 4));
}

void rk808_configure_ldo(uint8_t bus, int ldo, int millivolts)
{
	uint8_t vsel;

	if (!millivolts) {
		rk808_clrsetbits(bus, LDO_EN, 1 << (ldo - 1), 0);
		return;
	}

	switch (ldo) {
	case 1:
	case 2:
	case 4:
	case 5:
	case 8:
		vsel = div_round_up(millivolts, 100) - 18;
		assert(vsel <= 0x10);
		break;
	case 3:
	case 6:
	case 7:
		vsel = div_round_up(millivolts, 100) - 8;
		assert(vsel <= 0x11);
		break;
	default:
		die("Unknown LDO index!");
	}

	rk808_clrsetbits(bus, LDO_ONSEL(ldo), 0x1f, vsel);
	rk808_clrsetbits(bus, LDO_EN, 0, 1 << (ldo - 1));
}

void rk808_configure_buck(uint8_t bus, int buck, int millivolts)
{
	uint8_t vsel;
	uint8_t buck_reg;

	switch (buck) {
	case 1:
	case 2:
		/*base on 725mv, use 25mv step */
		vsel = (div_round_up(millivolts, 25) - 29) * 2 + 1;
		assert(vsel <= 0x3f);
		buck_reg = BUCK1SEL + 4 * (buck - 1);
		break;
	case 4:
		vsel = div_round_up(millivolts, 100) - 18;
		assert(vsel <= 0xf);
		buck_reg = BUCK4SEL;
		break;
	default:
		die("fault buck index!");
	}
	rk808_clrsetbits(bus, buck_reg, 0x3f, vsel);
	rk808_clrsetbits(bus, DCDC_EN, 0, 1 << (buck - 1));
}
