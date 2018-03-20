/*
 * Memory information
 *
 * Copyright (C) 2014, Intel Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _MEMORY_INFO_H_
#define _MEMORY_INFO_H_

#include <stdint.h>
#include <compiler.h>

#define DIMM_INFO_SERIAL_SIZE		5
#define DIMM_INFO_PART_NUMBER_SIZE	19
#define DIMM_INFO_TOTAL			8	/* Maximum num of dimm is 8 */

/**
 * If this table is filled and put in CBMEM,
 * then these info in CBMEM will be used to generate smbios type 17 table
 *
 * Values are specified according to the JEDEC SPD Standard.
 */
struct dimm_info {
	/*
	 * Size of the module in MiB.
	 */
	uint32_t dimm_size;
	/*
	 * SMBIOS (not SPD) device type.
	 *
	 * See the smbios.h smbios_memory_device_type enum.
	 */
	uint16_t ddr_type;
	uint16_t ddr_frequency;
	uint8_t rank_per_dimm;
	uint8_t channel_num;
	uint8_t dimm_num;
	uint8_t bank_locator;
	/*
	 * The last byte is '\0' for the end of string.
	 *
	 * Even though the SPD spec defines this field as a byte array the value
	 * is passed directly to SMBIOS as a string, and thus must be printable
	 * ASCII.
	 */
	uint8_t serial[DIMM_INFO_SERIAL_SIZE];
	/*
	 * The last byte is '\0' for the end of string
	 *
	 * Must contain only printable ASCII.
	 */
	uint8_t module_part_number[DIMM_INFO_PART_NUMBER_SIZE];
	/*
	 * SPD Manufacturer ID
	 */
	uint16_t mod_id;
	/*
	 * SPD Module Type.
	 *
	 * See spd.h for valid values.
	 *
	 * e.g., SPD_RDIMM, SPD_SODIMM, SPD_MICRO_DIMM
	 */
	uint8_t mod_type;
	/*
	 * SPD bus width.
	 *
	 * Bits 0 - 2 encode the primary bus width:
	 *   0b000 = 8 bit width
	 *   0b001 = 16 bit width
	 *   0b010 = 32 bit width
	 *   0b011 = 64 bit width
	 *
	 * Bits 3 - 4 encode the extension bits (ECC):
	 *   0b00 = 0 extension bits
	 *   0b01 = 8 bit of ECC
	 *
	 * e.g.,
	 *   64 bit bus with 8 bits of ECC (72 bits total): 0b1011
	 *   64 bit bus with 0 bits of ECC (64 bits total): 0b0011
	 *
	 * See the smbios.h smbios_memory_bus_width enum.
	 */
	uint8_t bus_width;
} __packed;

struct memory_info {
	uint8_t dimm_cnt;
	struct dimm_info dimm[DIMM_INFO_TOTAL];
} __packed;

#endif
