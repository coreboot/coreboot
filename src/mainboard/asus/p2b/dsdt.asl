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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "southbridge/intel/i82371eb/i82371eb.h"

DefinitionBlock ("DSDT.aml", "DSDT", 2, "CORE  ", "COREBOOT", 1)
{
	/* Define the main processor.*/
	Scope (\_PR)
	{
		/* Looks like the P_CNT field can't be a name or method (except
		 * builtins like Add()) and has to be hardcoded or generated
		 * into SSDT */
		Processor (CPU0, 0x01, Add(DEFAULT_PMBASE, PCNTRL), 0x06) {}
	}

	/* For now only define 2 power states:
	 *  - S0 which is fully on
	 *  - S5 which is soft off
	 * Any others would involve declaring the wake up methods.
	 */

	/* intel i82371eb (piix4e) datasheet, section 7.2.3, page 142 */
	/*
	000b / 0x0: soft off/suspend to disk (soff/std)			s5
	001b / 0x1: suspend to ram (str)				s3
	010b / 0x2: powered on suspend, context lost (poscl)		s1
	011b / 0x3: powered on suspend, cpu context lost (posccl)	s2
	100b / 0x4: powered on suspend, context maintained (pos)	s4
	101b / 0x5: working (clock control)				s0
	110b / 0x6: reserved
	111b / 0x7: reserved
	*/
	Name (\_S0, Package () { 0x05, 0x05, 0x00, 0x00 })
	Name (\_S1, Package () { 0x03, 0x03, 0x00, 0x00 })
	Name (\_S5, Package () { 0x00, 0x00, 0x00, 0x00 })

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
