/*
 * ifdtool - dump Intel Firmware Descriptor information
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <stdint.h>
#define IFDTOOL_VERSION "1.1"

enum spi_frequency {
	SPI_FREQUENCY_20MHZ = 0,
	SPI_FREQUENCY_33MHZ = 1,
	SPI_FREQUENCY_50MHZ = 4,
};

enum component_density {
	COMPONENT_DENSITY_512KB = 0,
	COMPONENT_DENSITY_1MB   = 1,
	COMPONENT_DENSITY_2MB   = 2,
	COMPONENT_DENSITY_4MB   = 3,
	COMPONENT_DENSITY_8MB   = 4,
	COMPONENT_DENSITY_16MB  = 5,
};

// flash descriptor
typedef struct {
	uint32_t flvalsig;
	uint32_t flmap0;
	uint32_t flmap1;
	uint32_t flmap2;
	uint8_t  reserved[0xefc - 0x20];
	uint32_t flumap1;
} __attribute__((packed)) fdbar_t;

// regions
typedef struct {
	uint32_t flreg0;
	uint32_t flreg1;
	uint32_t flreg2;
	uint32_t flreg3;
	uint32_t flreg4;
} __attribute__((packed)) frba_t;

// component section
typedef struct {
	uint32_t flcomp;
	uint32_t flill;
	uint32_t flpb;
} __attribute__((packed)) fcba_t;

// pch strap
typedef struct {
	uint32_t pchstrp0;
	uint32_t pchstrp1;
	uint32_t pchstrp2;
	uint32_t pchstrp3;
	uint32_t pchstrp4;
	uint32_t pchstrp5;
	uint32_t pchstrp6;
	uint32_t pchstrp7;
	uint32_t pchstrp8;
	uint32_t pchstrp9;
	uint32_t pchstrp10;
	uint32_t pchstrp11;
	uint32_t pchstrp12;
	uint32_t pchstrp13;
	uint32_t pchstrp14;
	uint32_t pchstrp15;
	uint32_t pchstrp16;
	uint32_t pchstrp17;
} __attribute__((packed)) fpsba_t;

// master
typedef struct {
	uint32_t flmstr1;
	uint32_t flmstr2;
	uint32_t flmstr3;
} __attribute__((packed)) fmba_t;

// processor strap
typedef struct {
	uint32_t data[8];
} __attribute__((packed)) fmsba_t;

// ME VSCC
typedef struct {
	uint32_t jid;
	uint32_t vscc;
} vscc_t;

typedef struct {
	// Actual number of entries specified in vtl
	vscc_t entry[8];
} vtba_t;

typedef struct {
	int base, limit, size;
} region_t;
