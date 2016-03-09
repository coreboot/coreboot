/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
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

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x02,		// DSDT revision: ACPI v2.0
	"COREv4",	// OEM id
	"COREBOOT",	// OEM table id
	0x20110725	// OEM revision
)
{
	#include "acpi/platform.asl"

	Name(_S0, Package() { 0x00, 0x00, 0x00, 0x00 })
	Name(_S5, Package() { 0x07, 0x00, 0x00, 0x00 })

	Scope (\_SB)
	{
		Device (PCI0)
		{
			#include <acpi/southcluster.asl>
			#include <acpi/pcie1.asl>
		}

		Name (PRUN, Package() {
			Package() { 0x0008FFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
			Package() { 0x0008FFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
			Package() { 0x0008FFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
			Package() { 0x0008FFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },

			Package() { 0x0009FFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
			Package() { 0x0009FFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
			Package() { 0x0009FFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
			Package() { 0x0009FFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },

			Package() { 0x000AFFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
			Package() { 0x000AFFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
			Package() { 0x000AFFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
			Package() { 0x000AFFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },

			Package() { 0x000BFFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
			Package() { 0x000BFFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
			Package() { 0x000BFFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
			Package() { 0x000BFFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },

			Package() { 0x000CFFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
			Package() { 0x000CFFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
			Package() { 0x000CFFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
			Package() { 0x000CFFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },

			Package() { 0x000DFFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
			Package() { 0x000DFFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
			Package() { 0x000DFFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
			Package() { 0x000DFFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },

			Package() { 0x000EFFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
			Package() { 0x000EFFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
			Package() { 0x000EFFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
			Package() { 0x000EFFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },

			Package() { 0x000FFFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
			Package() { 0x000FFFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
			Package() { 0x000FFFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
			Package() { 0x000FFFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },

			Package() { 0x0010FFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
			Package() { 0x0010FFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
			Package() { 0x0010FFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
			Package() { 0x0010FFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },

			Package() { 0x0011FFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
			Package() { 0x0011FFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
			Package() { 0x0011FFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
			Package() { 0x0011FFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },

			Package() { 0x0012FFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
			Package() { 0x0012FFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
			Package() { 0x0012FFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
			Package() { 0x0012FFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },

			Package() { 0x0013FFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
			Package() { 0x0013FFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
			Package() { 0x0013FFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
			Package() { 0x0013FFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },

			Package() { 0x0014FFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
			Package() { 0x0014FFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
			Package() { 0x0014FFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
			Package() { 0x0014FFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },

			Package() { 0x0016FFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
			Package() { 0x0016FFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
			Package() { 0x0016FFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
			Package() { 0x0016FFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },

			Package() { 0x0017FFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
			Package() { 0x0017FFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
			Package() { 0x0017FFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
			Package() { 0x0017FFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },

			Package() { 0x0018FFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
			Package() { 0x0018FFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
			Package() { 0x0018FFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
			Package() { 0x0018FFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },

			Package() { 0x0019FFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
			Package() { 0x0019FFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
			Package() { 0x0019FFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
			Package() { 0x0019FFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },

			Package() { 0x001CFFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
			Package() { 0x001CFFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
			Package() { 0x001CFFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
			Package() { 0x001CFFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },

			Package() { 0x001DFFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
			Package() { 0x001DFFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
			Package() { 0x001DFFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
			Package() { 0x001DFFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },

			Package() { 0x001EFFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
			Package() { 0x001EFFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
			Package() { 0x001EFFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
			Package() { 0x001EFFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },

			Package() { 0x001FFFFF, 0, \_SB.PCI0.LPC0.LNKA, 0 },
			Package() { 0x001FFFFF, 1, \_SB.PCI0.LPC0.LNKB, 0 },
			Package() { 0x001FFFFF, 2, \_SB.PCI0.LPC0.LNKC, 0 },
			Package() { 0x001FFFFF, 3, \_SB.PCI0.LPC0.LNKD, 0 },
		})

		Name (ARUN, Package() {
			Package() { 0x0008FFFF, 0, 0, 16 },
			Package() { 0x0008FFFF, 1, 0, 17 },
			Package() { 0x0008FFFF, 2, 0, 18 },
			Package() { 0x0008FFFF, 3, 0, 19 },

			Package() { 0x0009FFFF, 0, 0, 16 },
			Package() { 0x0009FFFF, 1, 0, 17 },
			Package() { 0x0009FFFF, 2, 0, 18 },
			Package() { 0x0009FFFF, 3, 0, 19 },

			Package() { 0x000AFFFF, 0, 0, 16 },
			Package() { 0x000AFFFF, 1, 0, 17 },
			Package() { 0x000AFFFF, 2, 0, 18 },
			Package() { 0x000AFFFF, 3, 0, 19 },

			Package() { 0x000BFFFF, 0, 0, 16 },
			Package() { 0x000BFFFF, 1, 0, 17 },
			Package() { 0x000BFFFF, 2, 0, 18 },
			Package() { 0x000BFFFF, 3, 0, 19 },

			Package() { 0x000CFFFF, 0, 0, 16 },
			Package() { 0x000CFFFF, 1, 0, 17 },
			Package() { 0x000CFFFF, 2, 0, 18 },
			Package() { 0x000CFFFF, 3, 0, 19 },

			Package() { 0x000DFFFF, 0, 0, 16 },
			Package() { 0x000DFFFF, 1, 0, 17 },
			Package() { 0x000DFFFF, 2, 0, 18 },
			Package() { 0x000DFFFF, 3, 0, 19 },

			Package() { 0x000EFFFF, 0, 0, 16 },
			Package() { 0x000EFFFF, 1, 0, 17 },
			Package() { 0x000EFFFF, 2, 0, 18 },
			Package() { 0x000EFFFF, 3, 0, 19 },

			Package() { 0x000FFFFF, 0, 0, 16 },
			Package() { 0x000FFFFF, 1, 0, 17 },
			Package() { 0x000FFFFF, 2, 0, 18 },
			Package() { 0x000FFFFF, 3, 0, 19 },

			Package() { 0x0010FFFF, 0, 0, 16 },
			Package() { 0x0010FFFF, 1, 0, 17 },
			Package() { 0x0010FFFF, 2, 0, 18 },
			Package() { 0x0010FFFF, 3, 0, 19 },

			Package() { 0x0011FFFF, 0, 0, 16 },
			Package() { 0x0011FFFF, 1, 0, 17 },
			Package() { 0x0011FFFF, 2, 0, 18 },
			Package() { 0x0011FFFF, 3, 0, 19 },

			Package() { 0x0012FFFF, 0, 0, 16 },
			Package() { 0x0012FFFF, 1, 0, 17 },
			Package() { 0x0012FFFF, 2, 0, 18 },
			Package() { 0x0012FFFF, 3, 0, 19 },

			Package() { 0x0013FFFF, 0, 0, 16 },
			Package() { 0x0013FFFF, 1, 0, 17 },
			Package() { 0x0013FFFF, 2, 0, 18 },
			Package() { 0x0013FFFF, 3, 0, 19 },

			Package() { 0x0014FFFF, 0, 0, 16 },
			Package() { 0x0014FFFF, 1, 0, 17 },
			Package() { 0x0014FFFF, 2, 0, 18 },
			Package() { 0x0014FFFF, 3, 0, 19 },

			Package() { 0x0016FFFF, 0, 0, 16 },
			Package() { 0x0016FFFF, 1, 0, 17 },
			Package() { 0x0016FFFF, 2, 0, 18 },
			Package() { 0x0016FFFF, 3, 0, 19 },

			Package() { 0x0017FFFF, 0, 0, 16 },
			Package() { 0x0017FFFF, 1, 0, 17 },
			Package() { 0x0017FFFF, 2, 0, 18 },
			Package() { 0x0017FFFF, 3, 0, 19 },

			Package() { 0x0018FFFF, 0, 0, 16 },
			Package() { 0x0018FFFF, 1, 0, 17 },
			Package() { 0x0018FFFF, 2, 0, 18 },
			Package() { 0x0018FFFF, 3, 0, 19 },

			Package() { 0x0019FFFF, 0, 0, 16 },
			Package() { 0x0019FFFF, 1, 0, 17 },
			Package() { 0x0019FFFF, 2, 0, 18 },
			Package() { 0x0019FFFF, 3, 0, 19 },

			Package() { 0x001CFFFF, 0, 0, 16 },
			Package() { 0x001CFFFF, 1, 0, 17 },
			Package() { 0x001CFFFF, 2, 0, 18 },
			Package() { 0x001CFFFF, 3, 0, 19 },

			Package() { 0x001DFFFF, 0, 0, 16 },
			Package() { 0x001DFFFF, 1, 0, 17 },
			Package() { 0x001DFFFF, 2, 0, 18 },
			Package() { 0x001DFFFF, 3, 0, 19 },

			Package() { 0x001EFFFF, 0, 0, 16 },
			Package() { 0x001EFFFF, 1, 0, 17 },
			Package() { 0x001EFFFF, 2, 0, 18 },
			Package() { 0x001EFFFF, 3, 0, 19 },

			Package() { 0x001FFFFF, 0, 0, 16 },
			Package() { 0x001FFFFF, 1, 0, 17 },
			Package() { 0x001FFFFF, 2, 0, 18 },
			Package() { 0x001FFFFF, 3, 0, 19 },
		})

		Device (UNC0)
		{
			Name (_HID, EisaId ("PNP0A03"))
			Name (_UID, 0x3F)
			Method (_BBN, 0, NotSerialized)
			{
				Return (0xff)
			}

			Name (_ADR, 0x00)
			Method (_STA, 0, NotSerialized)
			{
				Return (0xf)
			}

			Name (_CRS, ResourceTemplate ()
			{
				WordBusNumber (ResourceProducer, MinFixed, MaxFixed, PosDecode,
					0x0000,             // Granularity
					0x00FF,             // Range Minimum
					0x00FF,             // Range Maximum
					0x0000,             // Translation Offset
					0x0001,             // Length
					,, )
			})

			Method (_PRT, 0, NotSerialized)
			{
				If (LEqual (PICM, Zero))
				{
					Return (PRUN)
				}

				Return (ARUN)
			}
		}
	}

	#include "acpi/mainboard.asl"
}
