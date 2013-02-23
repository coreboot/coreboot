/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Joseph Smith <joe@settoplinux.org>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <spd.h>

#if CONFIG_ONBOARD_MEMORY_64MB

#define DENSITY 0x10

#elif CONFIG_ONBOARD_MEMORY_128MB

#define DENSITY 0x20

#endif

struct spd_entry {
	unsigned int address;
	unsigned int data;
};

/*
 * The onboard 64MB PC133 memory does not have an SPD EEPROM so the values
 * have to be set manually, the onboard memory is located in socket1 (0x51).
 */
const struct spd_entry spd_table [] = {
	{SPD_MEMORY_TYPE,                     0x04}, /* (Fundamental) memory type */
	{SPD_NUM_COLUMNS,                     0x09}, /* Number of column address bits */
	{SPD_NUM_DIMM_BANKS,                  0x01}, /* Number of module rows (banks) */
	{SPD_MODULE_DATA_WIDTH_LSB,           0x40}, /* Module data width (LSB) */
	{SPD_MIN_CYCLE_TIME_AT_CAS_MAX,       0x75}, /* SDRAM cycle time (highest CAS latency), RAS access time (tRAC) */
	{SPD_ACCESS_TIME_FROM_CLOCK,          0x54}, /* SDRAM access time from clock (highest CAS latency), CAS access time (Tac, tCAC) */
	{SPD_DENSITY_OF_EACH_ROW_ON_MODULE,   DENSITY}, /* Density of each row on module */
};
