/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2017-2018 Patrick Rudolph <siro@das-labor.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "hostbridge.asl"
#include "peg.asl"
#include <southbridge/intel/common/rcba.h>

/* PCI Device Resource Consumption */
Device (PDRC)
{
	Name (_HID, EISAID("PNP0C02"))
	Name (_UID, 1)

	Name (PDRS, ResourceTemplate() {
		Memory32Fixed(ReadWrite, DEFAULT_RCBA, 0x00004000)
		// Filled by _CRS
		Memory32Fixed(ReadWrite, 0, 0x00008000, MCHB)
		Memory32Fixed(ReadWrite, 0, 0x00001000, DMIB)
		Memory32Fixed(ReadWrite, 0, 0x00001000, EGPB)
		Memory32Fixed(ReadWrite, 0, 0x04000000, PCIX)
		Memory32Fixed(ReadWrite, 0xfed20000, 0x00020000) // Misc ICH
		Memory32Fixed(ReadWrite, 0xfed40000, 0x00005000) // TPM TIS
		Memory32Fixed(ReadWrite, 0xfed45000, 0x0004b000) // Misc ICH

#if CONFIG(CHROMEOS_RAMOOPS)
		Memory32Fixed(ReadWrite, CONFIG_CHROMEOS_RAMOOPS_RAM_START,
					 CONFIG_CHROMEOS_RAMOOPS_RAM_SIZE)
#endif

		/* Required for SandyBridge sighting 3715511 */
		Memory32Fixed(ReadWrite, 0x20000000, 0x00200000)
		Memory32Fixed(ReadWrite, 0x40000000, 0x00200000)
	})

	// Current Resource Settings
	Method (_CRS, 0, Serialized)
	{
		CreateDwordField (PDRS, ^MCHB._BAS, MBR0)
		MBR0 = \_SB.PCI0.MCHC.MHBR << 15

		CreateDwordField (PDRS, ^DMIB._BAS, DBR0)
		DBR0 = \_SB.PCI0.MCHC.DMBR << 12

		CreateDwordField (PDRS, ^EGPB._BAS, EBR0)
		EBR0 = \_SB.PCI0.MCHC.EPBR << 12

		CreateDwordField (PDRS, ^PCIX._BAS, XBR0)
		XBR0 = \_SB.PCI0.MCHC.PXBR << 26

		CreateDwordField (PDRS, ^PCIX._LEN, XSZ0)
		XSZ0 = 0x10000000 >> \_SB.PCI0.MCHC.PXSZ

		Return(PDRS)
	}
}

// Integrated graphics 0:2.0
#include <drivers/intel/gma/acpi/pch.asl>
