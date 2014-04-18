/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Sage Electronic Engineering, LLC
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

	/* Data to be patched by the BIOS during POST */
	/* FIXME the patching is not done yet! */
	/* Memory related values */
	Name(LOMH, 0x0)	/* Start of unused memory in C0000-E0000 range */
	Name(PBAD, 0x0)	/* Address of BIOS area (If TOM2 != 0, Addr >> 16) */
	Name(PBLN, 0x0)	/* Length of BIOS area */

	Name(PCBA, CONFIG_MMCONF_BASE_ADDRESS)	/* Base address of PCIe config space */
	Name(PCLN, Multiply(0x100000, CONFIG_MMCONF_BUS_NUMBER)) /* Length of PCIe config space, 1MB each bus */
	Name(HPBA, 0xFED00000)	/* Base address of HPET table */

	Name(SSFG, 0x0D)		/* S1 support: bit 0, S2 Support: bit 1, etc. S0 & S5 assumed */

	/* Some global data */
	Name(OSTP, 3)   /* Assume nothing. WinXp = 1, Vista = 2, Linux = 3, WinCE = 4 */
	Name(OSV, Ones) /* Assume nothing */
	Name(PMOD, One) /* Assume APIC */

	/* Variables used by EC */
	/* TODO: These may belong in global non-volatile storage */
	Name(PWRS, Zero)

	/* AcpiGpe0Blk */
	OperationRegion(GP0B, SystemMemory, 0xfed80814, 0x04)
		Field(GP0B, ByteAcc, NoLock, Preserve) {
		, 11,
		USBS, 1,
	}

	/* GPIO control block -- hardcoded to 0xfed80100 by AGESA */
	OperationRegion (GPIO, SystemMemory, 0xfed80100, 0x100)
	Field (GPIO, ByteAcc, NoLock, Preserve) {
		Offset (0x39),
		, 6,
		GP57, 1,	/* out: WLAN control (rf-kill) */
		Offset (0x76),
		, 7,
		GE22, 1,	/* General event 22 - connected to lid switch */
	}

	/* SMI/SCI control block -- hardcoded to 0xfed80200 by AGESA */
	OperationRegion (SMIX, SystemMemory, 0xfed80200, 0x100)
	Field (SMIX, AnyAcc, NoLock, Preserve) {
		Offset (0x08),	/* SCI level: 0 = active low, 1 = active high */
		, 22,
		LPOL, 1,	/* SCI22 trigger polarity - lid switch */
	}

	/*
	 * Used by EC code on certain events
	 *
	 * From ec/compal/ene932/acpi/ec.asl:
	 * The mainboard must define a PNOT method to handle power state
	 * notifications and Notify CPU device objects to re-evaluate their
	 * _PPC and _CST tables.
	 */
	Method (PNOT)
	{
		Store("Received PNOT call (probably from EC)", Debug)
		/* TODO: Implement this */
	}

Scope (\_SB) {
	Device (LID)
	{
		Name(_HID, EisaId("PNP0C0D"))
		Name(_PRW, Package () {EC_LID_GPE, 0x04}) /* wake from S1-S4 */
		Method(_LID, 0)
		{
			Return (GE22)	/* GE pin 22 */
		}

		Method (_INI, 0)
		{
			/* Make sure lid trigger polarity is set so that we
			 * trigger an SCI when lid status changes.
			 */
			Not(GE22, LPOL)
		}
	}

	Device(PWRB) {
		Name(_HID, EisaId("PNP0C0C"))
		Name(_UID, 0xAA)
		Name(_STA, 0x0B)
	}

	Device (MB) {
		/* Lid open */
		Method (LIDO) { /* Stub */ }
		/* Lid closed */
		Method (LIDC) { /* Stub */ }
		/* Increase brightness */
		Method (BRTU) { /* Stub */ }
		/* Decrease brightness */
		Method (BRTD) { /* Stub */ }
		/* Switch display */
		Method (DSPS) { /* Stub */ }
		/* Toggle wireless */
		Method (WLTG)
		{
			Store( Not(GP57), GP57 )
		}
		/* Return lid state */
		Method (LIDS)
		{
			Return(GE22)
		}
	}
}
