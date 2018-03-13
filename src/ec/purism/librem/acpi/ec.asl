/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

Device (TPSD)
{
	/*
	 * TPSACPI01 is not a valid _HID but it is what the linux topstar
	 * laptop driver expects, use this indirection to let it compile.
	 */
	Name (BHID, "TPSACPI01")
	Method (_HID)
	{
		Return (BHID)
	}
	Name (_UID, 0)

	Method (FNCX, 1, NotSerialized)
	{
		If (LEqual (Arg0, 0x86)) {
			/* Enable topstar-laptop kernel driver handling */
			Store (One, ^^EC.TPSE)
		} ElseIf (LEqual (Arg0, 0x87)) {
			/* Disable topstar-laptop kernel driver handling */
			Store (Zero, ^^EC.TPSE)
		}
	}
}

Device (EC)
{
	Name (_HID, EisaId ("PNP0C09"))
	Name (_UID, 0)
	Name (_GPE, EC_SCI_GPI)

	Name (_CRS, ResourceTemplate () {
		IO (Decode16, 0x62, 0x62, 0, 1)
		IO (Decode16, 0x66, 0x66, 0, 1)
	})

	OperationRegion (ERAM, EmbeddedControl, Zero, 0xFF)
	Field (ERAM, ByteAcc, Lock, Preserve)
	{
		Offset (0x15),
		BSTS, 2,	/* Battery Status */
		, 3,
		BTEX, 1,	/* Battery Present */
		Offset (0x1D),
		TPAD, 1,	/* Touchpad Enable/Disable */
		WIFI, 1,	/* WiFi Enable/Disable */
		, 3,
		BTLE, 1,	/* Bluetooth Enable/Disable */
		Offset (0x25),
		, 5,
		TPSE, 1,	/* topstar-laptop driver enable/disable */
		Offset (0x31),
		, 6,
		LIDS, 1,	/* LID Switch */
		ACEX, 1,	/* AC present */
		Offset (0x8E),
		BTDA, 16,	/* Battery Design Capacity */
		Offset (0x92),
		BTVO, 16,	/* Battery Present Voltage */
		Offset (0x98),
		BTRA, 16,	/* Battery Remaining Capacity */
		BTDF, 16,	/* Battery Last Full Charge Capacity */
		Offset (0x9E),
		, 4,
		BFCR, 1,	/* Battery Level Critical */
		Offset (0xA0),
		BTDV, 16,	/* Battery Design Voltage */
		Offset (0xA4),
		BTPR, 16,	/* Battery Present Rate */
		Offset (0xE6),
		TURB, 1,	/* EC Requested Turbo Enable/Disable */
		EDTB, 1,	/* Confirm Turbo Enable/Disable */
	}

	Method (_REG, 2, NotSerialized)
	{
		/* Initialize AC power state */
		Store (ACEX, \PWRS)

		/* Initialize LID switch state */
		Store (LIDS, \LIDS)
	}

	/* Notify topstar-laptop kernel driver */
	Method (TPSN, 1)
	{
		If (TPSE) {
			Notify (^^TPSD, Arg0)
		}
	}

	/* KEY_WWW */
	Method (_Q10)
	{
		TPSN (0x8A)
	}

	/* KEY_MAIL */
	Method (_Q11)
	{
		TPSN (0x8B)
	}

	/* KEY_MEDIA */
	Method (_Q12)
	{
		TPSN (0x8C)
	}

	/* AC Status Changed */
	Method (_Q20)
	{
		Store (ACEX, \PWRS)
		Notify (AC, 0x80)
		Notify (BAT, 0x80)
		PNOT ()
	}

	/* Lid Event */
	Method (_Q21)
	{
		Store (LIDS, \LIDS)
		Notify (LID0, 0x80)
	}

	/* Battery Event */
	Method (_Q22)
	{
		Notify (BAT, 0x80)
	}

	/* KEY_SWITCHVIDEOMODE */
	Method (_Q25)
	{
		TPSN (0x86)
	}

	/* KEY_BRIGHTNESSUP */
	Method (_Q28)
	{
		TPSN (0x80)
	}

	/* KEY_BRIGHTNESSDOWN */
	Method (_Q29)
	{
		TPSN (0x81)
	}

	/* KEY_MUTE */
	Method (_Q2A)
	{
		TPSN (0x85)
	}

	/* KEY_VOLUMEUP */
	Method (_Q2B)
	{
		TPSN (0x83)
	}

	/* KEY_VOLUMEDOWN */
	Method (_Q2C)
	{
		TPSN (0x84)
	}

	/* KEY_SLEEP */
	Method (_Q2F)
	{
		Notify (\_SB.SLPB, 0x80)
	}

	/* KEY_F13 (Touchpad Enable/Disable)
	Method (_Q34)
	{
		TPSN (0x87)
		XOr (^TPAD, One, ^TPAD)
	}

	/* KEY_WLAN */
	Method (_Q35)
	{
		TPSN (0x88)
		XOr (^WIFI, One, ^WIFI)
	}

	/* KEY_BLUETOOTH */
	Method (_Q37)
	{
		XOr (^BTLE, One, ^BTLE)
	}

	/* Turbo Enable/Disable */
	Method (_Q50)
	{
		/*
		 * Limiting frequency via PPC doesn't do anything if the
		 * kernel is using intel_pstate instead of ACPI _PPC.
		 *
		 * If the state is not updated in EDTB the EC will
		 * repeatedly send this event several times a second
		 * when the system is charging.
		 */
		If (TURB) {
			Store (PPCM_TURBO, PPCM)
			PPCN ()
			Store (One, EDTB)
		} Else {
			Store (PPCM_NOTURBO, PPCM)
			PPCN ()
			Store (Zero, EDTB)
		}
	}

	#include "ac.asl"
	#include "battery.asl"
}
