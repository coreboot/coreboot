/*
 * spd.h: Definitions for Serial Presence Detect (SPD) data
 *		  stored on SDRAM modules
 *
 * Copyright (C) 2005 Digital Design Corporation
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __SPD_H_DEFINED
#define __SPD_H_DEFINED

// Byte numbers
#define SPD_MEMORY_TYPE							 2
#define SPD_NUM_ROWS							 3
#define SPD_NUM_COLUMNS							 4
#define SPD_NUM_DIMM_BANKS						 5
#define SPD_MODULE_DATA_WIDTH_LSB				 6
#define SPD_MODULE_DATA_WIDTH_MSB				 7
#define SPD_MODULE_VOLTAGE						 8
#define SPD_MIN_CYCLE_TIME_AT_CAS_MAX			 9
#define SPD_DIMM_CONFIG_TYPE					11
#define	SPD_REFRESH								12
#define SPD_PRIMARY_DRAM_WIDTH					13
#define SPD_SUPPORTED_BURST_LENGTHS				16
#define SPD_NUM_BANKS_PER_DRAM					17
#define SPD_ACCEPTABLE_CAS_LATENCIES			18
#define SPD_MODULE_ATTRIBUTES					21
#define SPD_MIN_CYCLE_TIME_AT_CAS_REDUCED_05	23
#define SPD_MIN_CYCLE_TIME_AT_CAS_REDUCED_10	25
#define SPD_MIN_ROW_PRECHARGE_TIME				27
#define SPD_MIN_RAS_TO_CAS_DELAY				29
#define SPD_MIN_ACTIVE_TO_PRECHARGE_DELAY		30
#define SPD_ADDRESS_CMD_HOLD					33


// SPD_MEMORY_TYPE values
#define MEMORY_TYPE_SDRAM_DDR	7

// SPD_MODULE_VOLTAGE values
#define SPD_VOLTAGE_SSTL2		4

// SPD_DIMM_CONFIG_TYPE values
#define ERROR_SCHEME_NONE		0
#define ERROR_SCHEME_PARITY		1
#define ERROR_SCHEME_ECC		2

// SPD_ACCEPTABLE_CAS_LATENCIES values
#define SPD_CAS_LATENCY_1_0		0x01
#define SPD_CAS_LATENCY_1_5		0x02
#define SPD_CAS_LATENCY_2_0		0x04
#define SPD_CAS_LATENCY_2_5		0x08
#define SPD_CAS_LATENCY_3_0		0x10
#define SPD_CAS_LATENCY_3_5		0x20
#define SPD_CAS_LATENCY_4_0		0x40

// SPD_SUPPORTED_BURST_LENGTHS values
#define SPD_BURST_LENGTH_1		1
#define SPD_BURST_LENGTH_2		2
#define SPD_BURST_LENGTH_4		4
#define SPD_BURST_LENGTH_8		8
#define SPD_BURST_LENGTH_PAGE	(1<<7)


// SPD_MODULE_ATTRIBUTES values
#define MODULE_BUFFERED			1
#define MODULE_REGISTERED		2

#endif 	// __SPD_H_DEFINED
