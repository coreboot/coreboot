/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <stdint.h>
#include <console/console.h>
#include "southbridge/amd/cimx/sb700/smbus.h" /*SMBUS_IO_BASE*/
#include "w83795.h"

static u32 w83795_set_bank(u8 bank)
{
	return do_smbus_write_byte(SMBUS_IO_BASE, W83795_DEV, W83795_REG_BANKSEL, bank);
}

static u8 w83795_read(u16 reg)
{
	u32 ret;

	ret = w83795_set_bank(reg >> 8);
	if (ret < 0) {
		printk(BIOS_DEBUG, "read faild to set bank %x\n", reg >> 8);
		return -1;
	}

	ret = do_smbus_read_byte(SMBUS_IO_BASE, W83795_DEV, reg & 0xff);
	return ret;
}

static u8 w83795_write(u16 reg, u8 value)
{
	u32 err;

	err = w83795_set_bank(reg >> 8);
	if (err < 0) {
		printk(BIOS_DEBUG, "write faild to set bank %x\n", reg >> 8);
		return -1;
	}

	err = do_smbus_write_byte(SMBUS_IO_BASE, W83795_DEV, reg & 0xff, value);
	return err;
}

#if 0
static void w83795_set_speed(void)
{

}

static void w83795_set_ttti(void)//KR it works
{
	u32 i;
	for (i = 0; i < 6; i++) {
		//w83795_write(W83795_REG_TTTI(i), 0xa);//10 degree, default 40
		//w83795_write(W83795_REG_CTFS(i), 0x20);//32 degree, default 80
	}
}
#endif

static void w83795_set_tfmr(w83795_fan_mode_t mode)
{
	u8 val;
	u8 i;

	if ((mode == SMART_FAN_MODE) || (mode == THERMAL_CRUISE_MODE)) {
		val = 0xFF;
	} else {
		val = 0x00;
	}

	for (i = 0; i < 6; i++)
		w83795_write(W83795_REG_TFMR(i), val);
}

static u32 w83795_set_fan_mode(w83795_fan_mode_t mode)
{
	if (mode == SPEED_CRUISE_MODE) {
		w83795_write(W83795_REG_FCMS1, 0xFF);
		printk(BIOS_INFO, "W83795G/ADG work in Speed Cruise Mode\n");
	}  else {
		w83795_write(W83795_REG_FCMS1, 0x00);
		if (mode == THERMAL_CRUISE_MODE) {
			w83795_write(W83795_REG_FCMS2, 0x00);
			printk(BIOS_INFO, "W83795G/ADG work in Thermal Cruise Mode\n");
		} else if (mode == SMART_FAN_MODE) {
			w83795_write(W83795_REG_FCMS2, 0x3F);
			printk(BIOS_INFO, "W83795G/ADG work in Smart Fan Mode\n");
		} else {
			printk(BIOS_INFO, "W83795G/ADG work in Manual Mode\n");
			return -1;
		}
	}

	return 0;
}

static void w83795_set_tss(void)
{
	u8 val;

	val = 0x00;
	w83795_write(W83795_REG_TSS(0), val); /* Temp1, 2 */
	w83795_write(W83795_REG_TSS(1), val); /* Temp3, 4 */
	w83795_write(W83795_REG_TSS(2), val); /* Temp5, 6 */
}

static void w83795_set_fan(w83795_fan_mode_t mode)
{
	u8 i;

	/* select temperature sensor (TSS)*/
	w83795_set_tss();

	/* select Temperature to Fan mapping Relationships (TFMR)*/
	w83795_set_tfmr(mode);

	/* set fan output controlled mode (FCMS)*/
	w83795_set_fan_mode(mode);

	/* Set Critical Temperature to Full Speed all fan (CTFS) */
	for (i = 0; i < 6; i++) {
		w83795_write(W83795_REG_CTFS(i), 0x50); /* default 80 celsius degree */
	}

	if (mode == THERMAL_CRUISE_MODE) {
		/* Set Target Temperature of Temperature Inputs (TTTI) */
		for (i = 0; i < 6; i++) {
			w83795_write(W83795_REG_TTTI(i), 0x28); /* default 40 celsius degree */
		}
	} else if (mode == SMART_FAN_MODE) {
		/* Set the Relative Register-at SMART FAN IV Control Mode Table */
		//SFIV TODO
	}

	/* Set Hystersis of Temperature (HT) */
}

void w83795_init(w83795_fan_mode_t mode)
{
	u8 i;
	u8 val;

	if (do_smbus_read_byte(SMBUS_IO_BASE, W83795_DEV, 0x00) < 0) {
		printk(BIOS_INFO, "W83795G/ADG Nuvoton H/W Monitor not found\n");
		return;
	}
	val = w83795_read(W83795_REG_CONFIG);
	if ((val & W83795_REG_CONFIG_CONFIG48) == 0)
		printk(BIOS_INFO, "Found 64 pin W83795G Nuvoton H/W Monitor\n");
	else if ((val & W83795_REG_CONFIG_CONFIG48) == 1)
		printk(BIOS_INFO, "Found 48 pin W83795ADG Nuvoton H/W Monitor\n");

	/* Reset */
	val |= W83795_REG_CONFIG_INIT;
	w83795_write(W83795_REG_CONFIG, val);

	/* enable monitoring operations */
	val = w83795_read(W83795_REG_CONFIG);
	val |= W83795_REG_CONFIG_START;
	w83795_write(W83795_REG_CONFIG, val);

	w83795_set_fan(mode);

	printk(BIOS_INFO, "Fan   CTFS(celsius)  TTTI(celsius)\n");
	for (i = 0; i < 6; i++) {
		val = w83795_read(W83795_REG_CTFS(i));
		printk(BIOS_INFO, " %x     %d", i, val);
		val = w83795_read(W83795_REG_TTTI(i));
		printk(BIOS_INFO, "             %d\n", val);
	}
}

