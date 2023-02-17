/* SPDX-License-Identifier: GPL-2.0-only */

#include <drivers/pc80/pc/ps2_controller.asl>

Device (EC0)
{
	Name (_HID, EISAID ("PNP0C09"))		// ACPI Embedded Controller
	Name (_UID, 1)
	Name (_GPE, EC_SCI_GPI)
	Name (PWRS, 1)
	Name (LIDS, 1)

	// EC RAM fields
	OperationRegion(ERAM, EmbeddedControl, 0, 0xff)
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset(0x02),		// [Configuration Space 0x2]
		B0ST, 16,		// Battery 0 State
		B0FC, 16,		// Battery 0 Last Full Charge Capacity
		B0RC, 16,		// Battery 0 Remaining Capacity
		B0U1, 16,		// Unknown
		B0VT, 16,		// Battery 0 Present Voltage
		B0AC, 16,		// Battery 0 Present Rate
		Offset(0x40),		// [Configuration Space 0x40]
		ECOS, 8,		// OS System Type (0: DOS, 1: ACPI-compatible)
		CPUT, 8,		// CPU Temperature
		LOCT, 8,		// LOC Temperature
		OEMT, 8,		// OEM Temperature
		Offset (0x48),		// [Configuration Space 0x48]
		ECPS, 8,		// EC Power Source
		Offset (0x50),		// [Configuration Space 0x50]
		LUXH, 8,		// Ambient Light Illuminance High
		LUXL, 8,		// Ambient Light Illuminance Low
		Offset (0x5b),		// [Configuration Space 0x5b]
		BRIG, 8,		// Brightness
	} // End of ERAM

	Method (_STA, 0, NotSerialized)
	{
		Return (0x0f)
	}

	Method (_CRS, 0, NotSerialized)
	{
		Return (ResourceTemplate()
		{
			IO (Decode16, 0x62, 0x62, 0x00, 0x01)
			IO (Decode16, 0x66, 0x66, 0x00, 0x01)
		})
	}

	Method (_REG, 2, NotSerialized)
	{
		Printf ("-----> EC: _REG")

		ECOS = 0x01

		Printf ("<----- EC: _REG")
	}

	Method (_Q29, 0, NotSerialized)		// _Qxx: EC Query
	{
		Printf ("-----> EC: _Q29")

		PWRS = 1
		Notify (AC, 0x80)
		Notify (AC, 0x00)
		Notify (BAT0, 0x00)
		Notify (BAT0, 0x80)

		Printf ("<----- EC: _Q29")
	}

	Method (_Q31, 0, NotSerialized)		// _Qxx: EC Query
	{
		Printf ("-----> EC: _Q31")

		PWRS = 0
		Notify (AC, 0x80)
		Notify (AC, 0x00)
		Notify (BAT0, 0x00)
		Notify (BAT0, 0x80)

		Printf ("<----- EC: _Q31")
	}

	Method (_Q32, 0, NotSerialized)		// _Qxx: EC Query
	{
		Printf ("-----> EC: _Q32")

		Sleep (2500)
		Notify (BAT0, 0x00)
		Notify (BAT0, 0x80)
		Notify (BAT0, 0x81)
		Notify (BAT0, 0x82)

		Printf ("<----- EC: _Q32")
	}

	Method (_Q33, 0, NotSerialized)		// _Qxx: EC Query
	{
		Printf ("-----> EC: _Q33")

		Sleep (2500)
		Notify (BAT0, 0x00)
		Notify (BAT0, 0x80)
		Notify (BAT0, 0x81)
		Notify (BAT0, 0x82)

		Printf ("<---- EC: _Q33")
	}

	Method (_Q36, 0, NotSerialized)		// _Qxx: EC Query
	{
		Printf ("-----> EC: _Q36")

		Notify (BAT0, 0x80)

		Printf ("<----- EC: _Q36")
	}

	Method (_Q37, 0, NotSerialized)		// _Qxx: EC Query
	{
		Printf ("-----> EC: _Q37")

		Notify (BAT0, 0x80)

		Printf ("<----- EC: _Q37")
	}

	Method (_Q43, 0, NotSerialized)		// _Qxx: EC Query
	{
		Printf ("-----> EC: _Q43")

		Local0 = BRIG + 1
		If (Local0 > 0xaa) {
			Local0 = 0xaa
		}
		BRIG = Local0

		\_SB.PCI0.GFX0.INCB ()

		Printf ("<---- EC: _Q43")
	}

	Method (_Q44, 0, NotSerialized)		// _Qxx: EC Query
	{
		Printf ("-----> EC: _Q44")

		Local0 = BRIG - 1
		If (Local0 < 0xa0)
		{
			Local0 = 0xa0
		}
		BRIG = Local0

		\_SB.PCI0.GFX0.DECB ()

		Printf ("<---- EC: _Q44")
	}

	Method (_Q45, 0, NotSerialized)		// _Qxx: EC Query
	{
		Printf ("-----> EC: _Q45")

		LIDS = 0
		Notify (LID, 0x80)

		Printf ("<----- EC: _Q45")
	}

	Method (_Q46, 0, NotSerialized)		// _Qxx: EC Query
	{
		Printf ("-----> EC: _Q46")

		LIDS = 1
		Notify (LID, 0x80)

		Printf ("<----- EC: _Q46")
	}

	Method (_Q70, 0, NotSerialized)		// _Qxx: EC Query
	{
		Printf ("-----> EC: _Q70")

		Notify (ALSD, 0x80)

		Printf ("<----- EC: _Q70")
	}

	#include "battery.asl"
	#include "ac.asl"
	#include "lid.asl"
}
