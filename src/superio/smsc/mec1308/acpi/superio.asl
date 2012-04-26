/*
 * This file is part of the coreboot project.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

// Scope is \_SB.PCI0.LPCB

Device (SIO) {
	OperationRegion (SIOA, SystemIO, 0x2E, 0x02)
	Field (SIOA, ByteAcc, NoLock, Preserve)
	{
		SI2E, 8,
		SI2F, 8,
	}

	IndexField (SI2E, SI2F, ByteAcc, NoLock, Preserve)
	{
		Offset (0x07),
		SLDN, 8,	/* Logical Device Number */
		Offset (0x30),
		SACT, 8,	/* Activate */
		Offset (0x60),
		IO0H, 8,	/* Base Address 0 MSB */
		IO0L, 8,	/* Base Address 0 LSB */
		Offset (0x62),
		IO1H, 8,	/* Base Address 1 MSB */
		IO1L, 8,	/* Base Address 1 LSB */
		Offset (0x70),
		IQ00, 8,	/* Interrupt Select */
	}

	Name (LPM1, 1)		/* ACPI PM1 */
	Name (LEC1, 2)		/* EC 1 */
	Name (LEC2, 3)		/* EC 2 */
	Name (LSP1, 4)		/* Serial Port */
	Name (LKBC, 7)		/* Keyboard */
	Name (LEC0, 8)		/* EC 0 */
	Name (LMBX, 9)		/* Mailbox */

	Method (ENTR, 0, NotSerialized)
	{
		Store (0x55, SI2E)
	}

	Method (EXIT, 0, NotSerialized)
	{
		Store (0xaa, SI2E)
	}

	/* Parse activate register for an LDN */
	Method (ISEN, 1, NotSerialized)
	{
		ENTR ()
		Store (Arg0, SLDN)
		Store (SACT, Local0)
		EXIT ()

		/* Check if it exists */
		If (LEqual (Local0, 0xFF))
		{
			Return (0x00)
		}

		/* Check if activated */
		If (LEqual (Local0, One))
		{
			Return (0x0F)
		}
		Else
		{
			Return (0x0D)
		}
	}

	/* Enable an LDN via the activate register */
	Method (SENA, 1, NotSerialized)
	{
		ENTR ()
		Store (Arg0, SLDN)
		Store (One, SACT)
		EXIT ()
	}

	/* Disable an LDN via the activate register */
	Method (SDIS, 1, NotSerialized)
	{
		ENTR ()
		Store (Arg0, SLDN)
		Store (Zero, SACT)
		EXIT ()
	}

#ifdef SIO_ENABLE_SPM1
	Device (SPM1) {
		Name (_HID, EISAID ("PNP0C02"))
		Name (_UID, 1)

		Method (_STA, 0, NotSerialized) {
			Return (ISEN (LPM1))
		}

		Name (_CRS, ResourceTemplate ()
		{
			IO (Decode16, SIO_SPM1_IO0, SIO_SPM1_IO0, 0x08, 0x08)
		})

		Name (_PRS, ResourceTemplate ()
		{
			IO (Decode16, SIO_SPM1_IO0, SIO_SPM1_IO0, 0x08, 0x08)
		})
	}
#endif

#ifdef SIO_ENABLE_SEC1
	Device (SEC1) {
		Name (_HID, EISAID ("PNP0C09"))
		Name (_UID, 2)

		Method (_STA, 0, NotSerialized) {
			Return (ISEN (LEC1))
		}

		Name (_CRS, ResourceTemplate ()
		{
			IO (Decode16, SIO_SEC1_IO0, SIO_SEC1_IO0, 0x04, 0x04)
		})

		Name (_PRS, ResourceTemplate ()
		{
			IO (Decode16, SIO_SEC1_IO0, SIO_SEC1_IO0, 0x04, 0x04)
		})
	}
#endif

#ifdef SIO_ENABLE_SEC2
	Device (SEC2) {
		Name (_HID, EISAID ("PNP0C09"))
		Name (_UID, 3)

		Method (_STA, 0, NotSerialized) {
			Return (ISEN (LEC2))
		}

		Name (_CRS, ResourceTemplate ()
		{
			IO (Decode16, SIO_SEC2_IO0, SIO_SEC2_IO0, 0x04, 0x04)
		})

		Name (_PRS, ResourceTemplate ()
		{
			IO (Decode16, SIO_SEC2_IO0, SIO_SEC2_IO0, 0x04, 0x04)
		})
	}
#endif

#ifdef SIO_ENABLE_SSP1
	Device (SSP1) {
		Name (_HID, EISAID ("PNP0501"))
		Name (_UID, 4)

		Method (_STA, 0, NotSerialized) {
			Return (ISEN (LSP1))
		}

		Name (_CRS, ResourceTemplate ()
		{
			IO (Decode16, SIO_SSP1_IO0, SIO_SSP1_IO0, 0x08, 0x08)
			IRQNoFlags () {SIO_SSP1_IRQ}
		})

		Name (_PRS, ResourceTemplate ()
		{
			IO (Decode16, SIO_SSP1_IO0, SIO_SSP1_IO0, 0x08, 0x08)
			IRQNoFlags () {SIO_SSP1_IRQ}
		})
	}
#endif

#ifdef SIO_ENABLE_SKBC
	Device (SKBC)		// Keyboard
	{
		Name (_HID, EISAID("PNP0303"))
		Name (_CID, EISAID("PNP030B"))

		Method (_STA, 0, NotSerialized) {
			Return (ISEN (LKBC))
		}

		Name (_CRS, ResourceTemplate()
		{
			IO (Decode16, 0x60, 0x60, 0x01, 0x01)
			IO (Decode16, 0x64, 0x64, 0x01, 0x01)
			IRQNoFlags () {1}
		})

		Name (_PRS, ResourceTemplate()
		{
			IO (Decode16, 0x60, 0x60, 0x01, 0x01)
			IO (Decode16, 0x64, 0x64, 0x01, 0x01)
			IRQNoFlags () {1}
		})
	}
#endif

#ifdef SIO_ENABLE_SEC0
	Device (SEC0) {
		Name (_HID, EISAID ("PNP0C09"))
		Name (_UID, 8)

		Method (_STA, 0, NotSerialized) {
			Return (ISEN (LEC0))
		}

		Name (_CRS, ResourceTemplate ()
		{
			IO (Decode16, SIO_SEC0_IO0, SIO_SEC0_IO0, 0x04, 0x04)
		})

		Name (_PRS, ResourceTemplate ()
		{
			IO (Decode16, SIO_SEC0_IO0, SIO_SEC0_IO0, 0x04, 0x04)
		})
	}
#endif

#ifdef SIO_ENABLE_SMBX
	Device (SMBX)		// Mailbox
	{
		Name (_HID, EISAID("PNP0C02"))
		Name (_UID, 9)

		Method (_STA, 0, NotSerialized) {
			Return (ISEN (LMBX))
		}

		Name (_CRS, ResourceTemplate()
		{
			FixedIO (SIO_SMBX_IO0, 0x34)
		})

		Name (_PRS, ResourceTemplate()
		{
			FixedIO (SIO_SMBX_IO0, 0x34)
		})
	}
#endif
}
