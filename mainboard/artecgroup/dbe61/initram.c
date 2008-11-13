/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#define _MAINOBJECT

#include <types.h>
#include <lib.h>
#include <console.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <msr.h>
#include <io.h>
#include <amd_geodelx.h>
#include <northbridge/amd/geodelx/raminit.h>
#include <spd.h>

#define MANUALCONF 0		/* Do automatic strapped PLL config */
#define PLLMSRHI 0x00001490	/* manual settings for the PLL */
#define PLLMSRLO 0x02000030
#define DIMM0 ((u8) 0xA0)
#define DIMM1 ((u8) 0xA2)

struct spd_entry {
	u8 address;
	u8 data;
};

/* Save space by using a short list of SPD values used by Geode LX Memory init */
/* Fake SPD for DBE61C - 256MB. Same memory chip, and therefore same SPD entries, as for DBE62. */
/* Micron MT46V32M16 */
static const struct spd_entry spd_table[] = {
	{SPD_MEMORY_TYPE, 7},
	{SPD_NUM_ROWS, 13},
	{SPD_tRFC, 0x4b},
	{SPD_ACCEPTABLE_CAS_LATENCIES, 0x10},
	{SPD_DENSITY_OF_EACH_ROW_ON_MODULE, 0x40},
	{SPD_tRAS, 0x2d},
	{SPD_MIN_CYCLE_TIME_AT_CAS_MAX, 0x7}, /*0x <= 7},*/
	{SPD_MIN_RAS_TO_CAS_DELAY, 0x58},
	{SPD_tRRD, 0x3c},
	{SPD_tRP, 0x58},
	{SPD_PRIMARY_SDRAM_WIDTH, 8},
	{SPD_NUM_BANKS_PER_SDRAM, 0x4},
	{SPD_NUM_COLUMNS, 0xa}, /* 8kB */
	{SPD_NUM_DIMM_BANKS, 0x1},
	{SPD_REFRESH, 0x82},
	{SPD_SDRAM_CYCLE_TIME_2ND, 0x0},
	{SPD_SDRAM_CYCLE_TIME_3RD, 0x0},
	{SPD_tRCD, 15},
};

#if 0
/* Fake SPD for DBE61A - 128MB. Not working yet */
/* Micron MT46V16M16 */
static const struct spd_entry spd_table_dbe61a[] = {
	{SPD_ACCEPTABLE_CAS_LATENCIES, 0x10},
	{SPD_BANK_DENSITY, 0x40},
	{SPD_DEVICE_ATTRIBUTES_GENERAL, 0xff},
	{SPD_MEMORY_TYPE, 7},
	{SPD_MIN_CYCLE_TIME_AT_CAS_MAX, 10}, /* A guess for the tRAC value */
	{SPD_MODULE_ATTRIBUTES, 0xff}, /* FIXME later when we figure out. */
	{SPD_NUM_BANKS_PER_SDRAM, 4},
	{SPD_PRIMARY_SDRAM_WIDTH, 8},
	{SPD_NUM_DIMM_BANKS, 1}, /* ALIX1.C is 1 bank. */
	{SPD_NUM_COLUMNS, 0xa},
	{SPD_NUM_ROWS, 3},
	{SPD_REFRESH, 0x3a},
	{SPD_SDRAM_CYCLE_TIME_2ND, 60},
	{SPD_SDRAM_CYCLE_TIME_3RD, 75},
	{SPD_tRAS, 40},
	{SPD_tRCD, 15},
	{SPD_tRFC, 70},
	{SPD_tRP, 15},
	{SPD_tRRD, 10},
};
#endif

/**
 * Given an SMBUS device, and an address in that device, return the value of SPD
 * for that device. In this mainboard, the only one that can return is DIMM0. 
 * @param device The device number
 * @param address The address in SPD rom to return the value of
 * @returns The value
 */ 
u8 spd_read_byte(u16 device, u8 address)
{
	int i;
	/* returns 0xFF on any failures */
	u8 ret = 0xff;

	printk(BIOS_DEBUG, "spd_read_byte dev %04x", device);
	if (device == DIMM0) {
		for (i = 0; i < ARRAY_SIZE(spd_table); i++) {
			if (spd_table[i].address == address) {
				ret = spd_table[i].data;
                                break;
			}
		}

		if (i == ARRAY_SIZE(spd_table))
			printk(BIOS_DEBUG, " addr %02x does not exist in SPD table",
				address);
	}

	printk(BIOS_DEBUG, " addr %02x returns %02x\n", address, ret);
	return ret;
}

/**
  * Placeholder in case we ever need it. Since this file is a
  * template for other motherboards, we want this here and we want the
  * call in the right place.
  */

static void mb_gpio_init(void)
{
	/* Early mainboard specific GPIO setup */
}

/** 
  * main for initram for the PC Engines Alix 1C.  It might seem that you
  * could somehow do these functions in, e.g., the cpu code, but the
  * order of operations and what those operations are is VERY strongly
  * mainboard dependent. It's best to leave it in the mainboard code.
  */
int main(void)
{
	printk(BIOS_DEBUG, "Hi there from stage1\n");
	post_code(POST_START_OF_MAIN);

	system_preinit();
	printk(BIOS_DEBUG, "done preinit\n");

	mb_gpio_init();
	printk(BIOS_DEBUG, "done gpio init\n");

	pll_reset(MANUALCONF, PLLMSRHI, PLLMSRLO);
	printk(BIOS_DEBUG, "done pll reset\n");

	cpu_reg_init(0, DIMM0, DIMM1, DRAM_UNTERMINATED);
	printk(BIOS_DEBUG, "done cpu reg init\n");

	sdram_set_registers();
	printk(BIOS_DEBUG, "done sdram set registers\n");

	sdram_set_spd_registers(DIMM0, DIMM1);
	printk(BIOS_DEBUG, "done sdram set spd registers\n");

	sdram_enable(DIMM0, DIMM1);
	printk(BIOS_DEBUG, "done sdram enable\n");

	/* Check low memory */
	/*ram_check(0x00000000, 640*1024); */

	printk(BIOS_DEBUG, "stage1 returns\n");
	return 0;
}
