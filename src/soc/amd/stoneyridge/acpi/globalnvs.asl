/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
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
 */

/*
 * NOTE: The layout of the GNVS structure below must match the layout in
 * soc/amd/stoneyridge/include/soc/nvs.h !!!
 *
 */

External (NVSA)

OperationRegion (GNVS, SystemMemory, NVSA, 0x1000)
Field (GNVS, ByteAcc, NoLock, Preserve)
{
	/* Miscellaneous */
	Offset (0x00),
	PCNT,	8,	// 0x00 - Processor Count
	PPCM,	8,	// 0x01 - Max PPC State
	LIDS,	8,	// 0x02 - LID State
	PWRS,	8,	// 0x03 - AC Power State
	DPTE,	8,	// 0x04 - Enable DPTF
	CBMC,	32,	// 0x05 - 0x08 - coreboot Memory Console
	PM1I,	64,	// 0x09 - 0x10 - System Wake Source - PM1 Index
	GPEI,	64,	// 0x11 - 0x18 - GPE Wake Source
	NHLA,	64,	// 0x19 - 0x20 - NHLT Address
	NHLL,	32,	// 0x21 - 0x24 - NHLT Length
	PRT0,	32,	// 0x25 - 0x28 - PERST_0 Address
	SCDP,	8,	// 0x29 - SD_CD GPIO portid
	SCDO,	8,	// 0x2A - GPIO pad offset relative to the community
	TMPS,	8,	// 0x2B - Temperature Sensor ID
	TLVL,	8,	// 0x2C - Throttle Level Limit
	FLVL,	8,	// 0x2D - Current FAN Level
	TCRT,	8,	// 0x2E - Critical Threshold
	TPSV,	8,	// 0x2F - Passive Threshold
	TMAX,	8,	// 0x30 - CPU Tj_max
	Offset (0x34),  // 0x34 - AOAC Device Enables
	, 5,
	IC0E,	1,	//        I2C0, 5
	IC1E,	1,	//        I2C1, 6
	IC2E,	1,	//        I2C2, 7
	IC3E,	1,	//        I2C3, 8
	, 2,
	UT0E,	1,	//        UART0, 11
	UT1E,	1,	//        UART1, 12
	, 2,
	ST_E,	1,	//        SATA, 15
	, 2,
	EHCE,	1,	//        EHCI, 18
	, 4,
	XHCE,	1,	//        XCHI, 23
	SD_E,	1,	//        SD, 24
	, 2,
	ESPI,	1,	//        ESPI, 27
	, 4,
	FW00,	16,	// 0x38 - xHCI FW ROM addr, boot RAM
	FW02,	16,	// 0x3A - xHCI FW ROM addr, Instruction RAM
	FW01,	32,	// 0x3C - xHCI FW RAM addr, boot RAM
	FW03,	32,	// 0x40 - xHCI FW RAM addr, Instruction RAM
	EH10,	32,	// 0x44 - EHCI BAR
	/* ChromeOS stuff (0x100 -> 0xfff, size 0xeff) */
	Offset (0x100),
	#include <vendorcode/google/chromeos/acpi/gnvs.asl>
}
