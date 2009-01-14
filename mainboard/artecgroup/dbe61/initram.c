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
#define DIMM_DBE61C ((u8) 0xA0)
#define DIMM_EMPTY  ((u8) 0xA2)
#define DIMM_DBE61A ((u8) 0xA4)

struct spd_entry {
	u8 address;
	u8 data;
};

/* Save space by using a short list of SPD values used by Geode LX Memory init */

/* Fake SPD for DBE61C - 256MB. Same memory chip, and therefore same SPD entries, as for DBE62. */
/* Micron MT46V32M16 */
static const struct spd_entry spd_table_dbe61c[] = {
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
};

/* Fake SPD for DBE61A - 128MB */
/* Micron MT46V16M16 */
static const struct spd_entry spd_table_dbe61a[] = {
	{SPD_MEMORY_TYPE, 7},
	{SPD_NUM_ROWS, 13},
	{SPD_tRFC, 0x85},
	{SPD_ACCEPTABLE_CAS_LATENCIES, 0x10},
	{SPD_DENSITY_OF_EACH_ROW_ON_MODULE, 0x20},
	{SPD_tRAS, 0x35},
	{SPD_MIN_CYCLE_TIME_AT_CAS_MAX, 0x7}, /*0x <= 7},*/
	{SPD_MIN_RAS_TO_CAS_DELAY, 0x58},
	{SPD_tRRD, 0x3c},
	{SPD_tRP, 0x58},
	{SPD_PRIMARY_SDRAM_WIDTH, 8},
	{SPD_NUM_BANKS_PER_SDRAM, 0x4},
	{SPD_NUM_COLUMNS, 0x9}, /* 4kB */
	{SPD_NUM_DIMM_BANKS, 0x1},
	{SPD_REFRESH, 0x82},
	{SPD_SDRAM_CYCLE_TIME_2ND, 0x0},
	{SPD_SDRAM_CYCLE_TIME_3RD, 0x0},
};

/**
 * Given an SMBUS device, and an address in that device, return the value of SPD
 * for that device. In this mainboard, the only one that can return is DIMM_DBE61A
 * and DIMM_DBE61C.
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
	if (device == DIMM_DBE61C) {
		for (i = 0; i < ARRAY_SIZE(spd_table_dbe61c); i++) {
			if (spd_table_dbe61c[i].address == address) {
				ret = spd_table_dbe61c[i].data;
				break;
			}
		}

		if (i == ARRAY_SIZE(spd_table_dbe61c))
			printk(BIOS_DEBUG, " addr %02x does not exist in DBE61C SPD table",
				address);
	} else if (device == DIMM_DBE61A) {
		for (i = 0; i < ARRAY_SIZE(spd_table_dbe61a); i++) {
			if (spd_table_dbe61a[i].address == address) {
				ret = spd_table_dbe61a[i].data;
				break;
			}
		}

		if (i == ARRAY_SIZE(spd_table_dbe61a))
			printk(BIOS_DEBUG, " addr %02x does not exist in DBE61A SPD table",
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

static void initialize_ram(u8 dimm0, u8 dimm1)
{
	cpu_reg_init(0, dimm0, dimm1, DRAM_UNTERMINATED);
	printk(BIOS_DEBUG, "done cpu reg init\n");

	sdram_set_registers();
	printk(BIOS_DEBUG, "done sdram set registers\n");

	sdram_set_spd_registers(dimm0, dimm1);
	printk(BIOS_DEBUG, "done sdram set spd registers\n");

	sdram_enable(dimm0, dimm1);
	printk(BIOS_DEBUG, "done sdram enable\n");
}

/** 
  * main for initram for the Artec Group ThinCan DBE61. It might seem that
  * you could somehow do these functions in, e.g., the cpu code, but the
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

	printk(BIOS_INFO, "Trying 256MB RAM\n");
	initialize_ram(DIMM_DBE61C, DIMM_EMPTY);
	/* FIXME: Would like to check around 188MB and 88MB, but that high always
	   fails, even if everything works fine for a memtest86+ payload */
	if (ram_check(596*1024, 608*1024) == 0) {
		printk(BIOS_INFO, "DRAM 256MB configuration OK\n");
	} else {
		printk(BIOS_INFO, "Trying 128MB RAM\n");
		initialize_ram(DIMM_DBE61A, DIMM_EMPTY);

		if (ram_check(596*1024, 608*1024) == 0)
			printk(BIOS_INFO, "DRAM 128MB configuration OK\n");
		else
			printk(BIOS_INFO, "DRAM configuration failed\n");
	}

	/* Check low memory */
	/*ram_check(0x00000000, 640*1024); */

	printk(BIOS_DEBUG, "stage1 returns\n");
	return 0;
}
