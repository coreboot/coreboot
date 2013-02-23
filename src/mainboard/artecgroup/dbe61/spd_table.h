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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <spd.h>

struct spd_entry {
	unsigned int address;
	unsigned int data;
	};

/* Save space by using a short list of SPD values used by Geode LX Memory init */
/* 128MB */
const struct spd_entry spd_table [] =
{
{SPD_MEMORY_TYPE,                     0x07}, /* (Fundamental) memory type */
{SPD_NUM_ROWS,                        0x0D}, /* Number of row address bits */
{SPD_NUM_COLUMNS,                     0x09}, /* Number of column address bits */
{SPD_NUM_DIMM_BANKS,                  0x01}, /* Number of module rows (banks) */
{SPD_MIN_CYCLE_TIME_AT_CAS_MAX,       0x50}, /* SDRAM cycle time (highest CAS latency), RAS access time (tRAC) */
{SPD_REFRESH,                         0x82}, /* Refresh rate/type */
{SPD_PRIMARY_SDRAM_WIDTH,             0x08}, /* SDRAM width (primary SDRAM) */
{SPD_NUM_BANKS_PER_SDRAM,             0x04}, /* SDRAM device attributes, number of banks on SDRAM device */
{SPD_ACCEPTABLE_CAS_LATENCIES,        0x1C}, /* SDRAM device attributes, CAS latency */
{SPD_MODULE_ATTRIBUTES,               0x20}, /* SDRAM module attributes */
{SPD_DEVICE_ATTRIBUTES_GENERAL,       0xC0}, /* SDRAM device attributes, general */
{SPD_SDRAM_CYCLE_TIME_2ND,            0x60}, /* SDRAM cycle time (2nd highest CAS latency) */
{SPD_SDRAM_CYCLE_TIME_3RD,            0x75}, /* SDRAM cycle time (3rd highest CAS latency) */
{SPD_MIN_ROW_PRECHARGE_TIME,          0x3C}, /* Minimum row precharge time (Trp) */
{SPD_MIN_ROWACTIVE_TO_ROWACTIVE,      0x28}, /* Minimum row active to row active (Trrd) */
{SPD_MIN_RAS_TO_CAS_DELAY,            0x3C}, /* Minimum RAS to CAS delay (Trcd) */
{SPD_MIN_ACTIVE_TO_PRECHARGE_DELAY,   0x28}, /* Minimum RAS pulse width (Tras) */
{SPD_DENSITY_OF_EACH_ROW_ON_MODULE,   0x20}, /* Density of each row on module */
{SPD_CMD_SIGNAL_INPUT_HOLD_TIME,      0x60}, /* Command and address signal input hold time */
{SPD_tRC,                             0x37}, /* SDRAM Device Minimum Active to Active/Auto Refresh Time (tRC) */
{SPD_tRFC,                            0x46}  /* SDRAM Device Minimum Auto Refresh to Active/Auto Refresh (tRFC) */
};
