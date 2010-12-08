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

/* Intel i82801G USB support */

// USB Controller 0:1d.0

Device (USB1)
{
	Name(_ADR, 0x001d0000)

	OperationRegion(U01P, PCI_Config, 0, 256)
	Field(U01P, DWordAcc, NoLock, Preserve)
	{
		Offset(0xc4),
		U1WE, 2		// USB Wake Enable
	}

	Name (_PRW, Package(){ 3, 4 }) // Power Resources for Wake

	Method (_PSW, 1)	// Power State Wake method
	{
		// USB Controller can wake OS from Sleep State
		If (Arg0) {
			Store (3, U1WE)
		} Else {
			Store (0, U1WE)
		}
	}

	// Leave USB ports on for to allow Wake from USB

	Method(_S3D,0)	// Highest D State in S3 State
	{
		Return (2)
	}

	Method(_S4D,0)	// Highest D State in S4 State
	{
		Return (2)
	}
}


// USB Controller 0:1d.1

Device (USB2)
{
	Name(_ADR, 0x001d0001)

	OperationRegion(U02P, PCI_Config, 0, 256)
	Field(U02P, DWordAcc, NoLock, Preserve)
	{
		Offset(0xc4),
		U2WE, 2		// USB Wake Enable
	}

	Name (_PRW, Package(){ 3, 4 }) // Power Resources for Wake

	Method (_PSW, 1)	// Power State Wake method
	{
		// USB Controller can wake OS from Sleep State
		If (Arg0) {
			Store (3, U2WE)
		} Else {
			Store (0, U2WE)
		}
	}

	// Leave USB ports on for to allow Wake from USB

	Method(_S3D,0)	// Highest D State in S3 State
	{
		Return (2)
	}

	Method(_S4D,0)	// Highest D State in S4 State
	{
		Return (2)
	}

}


// USB Controller 0:1d.2

Device (USB3)
{
	Name(_ADR, 0x001d0002)

	OperationRegion(U03P, PCI_Config, 0, 256)
	Field(U03P, DWordAcc, NoLock, Preserve)
	{
		Offset(0xc4),
		U3WE, 2		// USB Wake Enable
	}

	Name (_PRW, Package(){ 3, 4 }) // Power Resources for Wake

	Method (_PSW, 1)	// Power State Wake method
	{
		// USB Controller can wake OS from Sleep State
		If (Arg0) {
			Store (3, U3WE)
		} Else {
			Store (0, U3WE)
		}
	}

	// Leave USB ports on for to allow Wake from USB

	Method(_S3D,0)	// Highest D State in S3 State
	{
		Return (2)
	}

	Method(_S4D,0)	// Highest D State in S4 State
	{
		Return (2)
	}

}


// USB Controller 0:1d.3

Device (USB4)
{
	Name(_ADR, 0x001d0003)

	OperationRegion(U04P, PCI_Config, 0, 256)
	Field(U04P, DWordAcc, NoLock, Preserve)
	{
		Offset(0xc4),
		U4WE, 2		// USB Wake Enable
	}

	Name (_PRW, Package(){ 3, 4 }) // Power Resources for Wake

	Method (_PSW, 1)	// Power State Wake method
	{
		// USB Controller can wake OS from Sleep State
		If (Arg0) {
			Store (3, U4WE)
		} Else {
			Store (0, U4WE)
		}
	}

	// Leave USB ports on for to allow Wake from USB

	Method(_S3D,0)	// Highest D State in S3 State
	{
		Return (2)
	}

	Method(_S4D,0)	// Highest D State in S4 State
	{
		Return (2)
	}

}


// EHCI Controller 0:1d.7

Device (EHC1)
{
	Name(_ADR, 0x001d0007)

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


