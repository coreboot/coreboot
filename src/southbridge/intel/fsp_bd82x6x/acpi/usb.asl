/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

/* Intel Cougar Point USB support */

// EHCI Controller 0:1d.0

Device (EHC1)
{
	Name(_ADR, 0x001d0000)

	Name (_PRW, Package(){ 13, 4 }) // Power Resources for Wake

	// Leave USB ports on for to allow Wake from USB

	Method(_S3D,0)	// Highest D State in S3 State
	{
		Return (2)
	}

	Method(_S4D,0)	// Highest D State in S4 State
	{
		Return (2)
	}

	Device (HUB7)
	{
		Name (_ADR, 0x00000000)

		// How many are there?
		Device (PRT1) { Name (_ADR, 1) } // USB Port 0
		Device (PRT2) { Name (_ADR, 2) } // USB Port 1
		Device (PRT3) { Name (_ADR, 3) } // USB Port 2
		Device (PRT4) { Name (_ADR, 4) } // USB Port 3
		Device (PRT5) { Name (_ADR, 5) } // USB Port 4
		Device (PRT6) { Name (_ADR, 6) } // USB Port 5
	}
}

// EHCI #2 Controller 0:1a.0

Device (EHC2)
{
	Name(_ADR, 0x001a0000)

	Name (_PRW, Package(){ 13, 4 }) // Power Resources for Wake

	// Leave USB ports on for to allow Wake from USB

	Method(_S3D,0)	// Highest D State in S3 State
	{
		Return (2)
	}

	Method(_S4D,0)	// Highest D State in S4 State
	{
		Return (2)
	}

	Device (HUB7)
	{
		Name (_ADR, 0x00000000)

		// How many are there?
		Device (PRT1) { Name (_ADR, 1) } // USB Port 0
		Device (PRT2) { Name (_ADR, 2) } // USB Port 1
		Device (PRT3) { Name (_ADR, 3) } // USB Port 2
		Device (PRT4) { Name (_ADR, 4) } // USB Port 3
		Device (PRT5) { Name (_ADR, 5) } // USB Port 4
		Device (PRT6) { Name (_ADR, 6) } // USB Port 5
	}
}

