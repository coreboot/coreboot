/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
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

Device (EC0)
{
	Name (_HID, EisaId ("PNP0C09"))
	Name (_UID, 1)
	Name (_GPE, EC_SCI_GPI)
	Name (_STA, 0xf)
	Name (DBUG, Zero)

	Name (_CRS, ResourceTemplate() {
		IO (Decode16,
		    CONFIG_EC_BASE_ACPI_DATA,
		    CONFIG_EC_BASE_ACPI_DATA,
		    4, 4)
		IO (Decode16,
		    CONFIG_EC_BASE_ACPI_COMMAND,
		    CONFIG_EC_BASE_ACPI_COMMAND,
		    4, 4)
	})

	/* Handle registration of EmbeddedControl region */
	Name (EREG, Zero)
	OperationRegion (ERAM, EmbeddedControl, 0, 0xff)
	Method (_REG, 2)
	{
		/* Indicate region is registered */
		EREG = Arg1

		/* Store initial value for power status */
		ECPR = R (PWSR)

		/* Indicate to EC that OS is ready for queries */
		W (ERDY, Arg1)

		/* Indicate that the OS supports S0ix */
		W (CSOS, One)

		/* Tell EC to stop emulating PS/2 mouse */
		W (PS2M, Zero)

		/* Enable DPTF support if enabled in devicetree */
		If (\DPTE == One) {
			W (DWST, Arg1)
		}

		/* Initialize UCSI */
		^UCSI.INIT ()
	}

	/*
	 * Find bitmask for field
	 *  Arg0 = EC field structure
	 *  Arg1 = Value
	 */
	Method (EBIT, 2, NotSerialized)
	{
		Local0 = DeRefOf (Arg0[1])	/* Mask */
		Local1 = Arg1 & Local0
		FindSetRightBit (Local0, Local2)
		If (Local2) {
			Local1 >>= Local2 - 1
		}
		Return (Local1)
	}

	/*
	 * READ or WRITE from EC region
	 *  Arg0 = EC field structure
	 *  Arg1 = Value to write
	 */
	Method (ECRW, 2, Serialized, 2)
	{
		If (!EREG) {
			Return (Zero)
		}

		Local0 = DeRefOf (Arg0[0])	/* Byte offset */
		Local1 = DeRefOf (Arg0[1])	/* Mask */
		Local2 = DeRefOf (Arg0[2])	/* Read/Write */

		OperationRegion (ERAM, EmbeddedControl, Local0, 2)
		Field (ERAM, ByteAcc, Lock, WriteAsZeros)
		{
			BYT1, 8,
			BYT2, 8,
		}

		If (Local2 == RD) {
			/* Read first byte */
			Local3 = BYT1

			/* Read second byte if needed */
			FindSetLeftBit (Local1, Local4)
			If (Local4 > 8) {
				Local4 = BYT2
				Local4 <<= 8
				Local3 |= Local4
			}

			Local5 = EBIT (Arg0, Local3)
			If (DBUG) {
				Printf ("ECRD %o = %o", Local0, Local5)
			}
			Return (Local5)
		} ElseIf (Local2 == WR) {
			/* Write byte */
			If (DBUG) {
				Printf ("ECWR %o = %o", Local0, Arg1)
			}
			BYT1 = Arg1
		}
		Return (Zero)
	}

	/*
	 * Read a field from EC
	 *  Arg0 = EC field structure
	 */
	Method (R, 1, Serialized, 2)
	{
		Return (ECRW (Arg0, Zero))
	}

	/*
	 * Write value to a field from EC
	 *  Arg0 = EC field structure
	 *  Arg1 = Value to write
	 */
	Method (W, 2, Serialized, 2)
	{
		Return (ECRW (Arg0, Arg1))
	}

	/*
	 * Tell EC that the OS is entering or exiting S0ix
	 */
	Method (S0IX, 1, Serialized)
	{
		If (Arg0) {
			Printf ("EC Enter S0ix")
			W (CSEX, One)

			/*
			 * Read back from EC RAM after enabling S0ix
			 * to prevent EC from aborting S0ix entry.
			 */
			R (EVT1)
		} Else {
			Printf ("EC Exit S0ix")
			W (CSEX, Zero)
		}
	}

	#include "ec_dev.asl"
	#include "ec_ram.asl"
	#include "ac.asl"
	#include "battery.asl"
	#include "event.asl"
	#include "lid.asl"
	#include "platform.asl"
	#include "vbtn.asl"
	#include "ucsi.asl"
#ifdef EC_ENABLE_DPTF
	#include "dptf.asl"
#endif
#ifdef EC_ENABLE_PRIVACY
	#include "privacy.asl"
#endif
}
