/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019-2020 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* Global Variables */

Name (\PICM, 0)		// IOAPIC/8259

/*
 * Global ACPI memory region. This region is used for passing information
 * between coreboot (aka "the system bios"), ACPI, and the SMI handler.
 * Since we don't know where this will end up in memory at ACPI compile time,
 * we have to fix it up in coreboot's ACPI creation phase.
 */

External (NVSA)

OperationRegion (GNVS, SystemMemory, NVSA, 0x2000)
Field (GNVS, ByteAcc, NoLock, Preserve)
{
	/* Miscellaneous */
	OSYS,	16,	// 0x00 - Operating System
	SMIF,	8,	// 0x02 - SMI function
	PCNT,	8,	// 0x03 - Processor Count
	PPCM,	8,	// 0x04 - Max PPC State
	TLVL,	8,	// 0x05 - Throttle Level Limit
	LIDS,	8,	// 0x06 - LID State
	PWRS,	8,	// 0x07 - AC Power State
	CBMC,	32,	// 0x08 - 0x0b AC Power State
	PM1I,	64,	// 0x0c - 0x13 PM1 wake status bit
	GPEI,	64,	// 0x14 - 0x17 GPE wake status bit
	DPTE,	8,	// 0x1c - Enable DPTF
	NHLA,	64,	// 0x1d - 0x24 NHLT Address
	NHLL,	32,	// 0x25 - 0x28 NHLT Length
	CID1,	16,	// 0x29 - 0x2a Wifi Country Identifier
	U2WE,	16,	// 0x2b - 0x2c USB2 Wake Enable Bitmap
	U3WE,	16,	// 0x2d - 0x2e USB3 Wake Enable Bitmap
	UIOR,	8,	// 0x2f - UART debug controller init on S3 resume
	E4GM,	8,	// 0x30 - Enable above 4GB MMIO Resource
	A4GB,	64,	// 0x31 - 0x38 Base of above 4GB MMIO Resource
	A4GS,	64,	// 0x39 - 0x40 Length of above 4GB MMIO Resource

	/* ChromeOS specific */
	Offset (0x100),
	#include <vendorcode/google/chromeos/acpi/gnvs.asl>
}
