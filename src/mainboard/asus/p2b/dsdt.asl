/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Tobias Diedrich <ranma+coreboot@tdiedrich.de>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "southbridge/intel/i82371eb/i82371eb.h"

DefinitionBlock ("DSDT.aml", "DSDT", 2, "CORE  ", "COREBOOT", 1)
{
	/*
	 * Intel 82371EB (PIIX4E) datasheet, section 7.2.3, page 142
	 *
	 * 0: soft off/suspend to disk					S5
	 * 1: suspend to ram						S3
	 * 2: powered on suspend, context lost				S2
	 *    Note: 'context lost' means the CPU restarts at the reset
	 *          vector
	 * 3: powered on suspend, CPU context lost			S1
	 *    Note: Looks like 'CPU context lost' does _not_ mean the
	 *          CPU restarts at the reset vector. Most likely only
	 *          caches are lost, so both 0x3 and 0x4 map to acpi S1
	 * 4: powered on suspend, context maintained			S1
	 * 5: working (clock control)					S0
	 * 6: reserved
	 * 7: reserved
	 */
	Name (\_S0, Package () { 0x05, 0x05, 0x00, 0x00 })
	Name (\_S1, Package () { 0x03, 0x03, 0x00, 0x00 })
	Name (\_S5, Package () { 0x00, 0x00, 0x00, 0x00 })

	OperationRegion (SIO1, SystemIO, Add(DEFAULT_PMBASE, GPO0), 2)
	Field (SIO1, ByteAcc, NoLock, Preserve)
	{
		FANP,	1, /* CPU/case fan power */
		Offset (0x01),
		PLED,	1,
	}

	Method (\_PTS, 1, NotSerialized)
	{
		/* Disable fan, blink power led */
		Store (Zero, FANP)
		Store (Zero, PLED)
	}

	Method (\_WAK, 1, NotSerialized)
	{
		/* Re-enable fan, stop power led blinking */
		Store (One, FANP)
		Store (One, PLED)
		/* wake OK */
		Return(Package(0x02){0x00, 0x00})
	}

	/* Root of the bus hierarchy */
	Scope (\_SB)
	{
		/* Top PCI device */
		Device (PCI0)
		{
			Name (_HID, EisaId ("PNP0A03"))
			Name (_ADR, 0x00)
			Name (_UID, 0x00)
			Name (_BBN, 0x00)

			/* PCI Routing Table */
			Name (_PRT, Package () {
				Package (0x04) { 0x0001FFFF, 0, LNKA, 0 },
				Package (0x04) { 0x0001FFFF, 1, LNKB, 0 },
				Package (0x04) { 0x0001FFFF, 2, LNKC, 0 },
				Package (0x04) { 0x0001FFFF, 3, LNKD, 0 },

				Package (0x04) { 0x0004FFFF, 0, LNKA, 0 },
				Package (0x04) { 0x0004FFFF, 1, LNKB, 0 },
				Package (0x04) { 0x0004FFFF, 2, LNKC, 0 },
				Package (0x04) { 0x0004FFFF, 3, LNKD, 0 },

				Package (0x04) { 0x0009FFFF, 0, LNKD, 0 },
				Package (0x04) { 0x0009FFFF, 1, LNKA, 0 },
				Package (0x04) { 0x0009FFFF, 2, LNKB, 0 },
				Package (0x04) { 0x0009FFFF, 3, LNKC, 0 },

				Package (0x04) { 0x000AFFFF, 0, LNKC, 0 },
				Package (0x04) { 0x000AFFFF, 1, LNKD, 0 },
				Package (0x04) { 0x000AFFFF, 2, LNKA, 0 },
				Package (0x04) { 0x000AFFFF, 3, LNKB, 0 },

				Package (0x04) { 0x000BFFFF, 0, LNKB, 0 },
				Package (0x04) { 0x000BFFFF, 1, LNKC, 0 },
				Package (0x04) { 0x000BFFFF, 2, LNKD, 0 },
				Package (0x04) { 0x000BFFFF, 3, LNKA, 0 },

				Package (0x04) { 0x000CFFFF, 0, LNKA, 0 },
				Package (0x04) { 0x000CFFFF, 1, LNKB, 0 },
				Package (0x04) { 0x000CFFFF, 2, LNKC, 0 },
				Package (0x04) { 0x000CFFFF, 3, LNKD, 0 },

			})

#include "northbridge/intel/i440bx/acpi/sb_pci0_crs.asl"
#include "southbridge/intel/i82371eb/acpi/isabridge.asl"
		}
#include "southbridge/intel/i82371eb/acpi/pirq.asl"
	}
}
