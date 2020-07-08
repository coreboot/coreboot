/* SPDX-License-Identifier: GPL-2.0-only */

#include <drivers/pc80/pc/ps2_controller.asl>

Device (EC0)
{
	Name (_HID, EISAID ("PNP0C09"))		// ACPI Embedded Controller
	Name (_UID, 1)
	Name (_GPE, Add(EC_SCI_GPI, 16))	// GPE for Runtime SCI
	Name (PWRS, 1)
	Name (LIDS, 1)

	// EC RAM fields
	OperationRegion(ERAM, EmbeddedControl, 0, 0xFF)
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
		Offset (0x5B),		// [Configuration Space 0x5B]
		BRIG, 8,		// Brightness
	} // End of ERAM

	Method (_STA, 0, NotSerialized)  // _STA: Status
	{
		Return (0x0F)
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
		Store ("-----> EC: _REG", Debug)

		Store (0x01, ECOS)

		Store ("<----- EC: _REG", Debug)
	}

	Method (_Q29, 0, NotSerialized)		// _Qxx: EC Query
	{
		Store ("-----> EC: _Q29", Debug)

		Store (1, PWRS)
		Notify (AC, 0x80)
		Notify (AC, 0x00)
		Notify (BAT0, 0x00)
		Notify (BAT0, 0x80)

		Store ("<----- EC: _Q29", Debug)
	}

	Method (_Q31, 0, NotSerialized)		// _Qxx: EC Query
	{
		Store ("-----> EC: _Q31", Debug)

		Store (0, PWRS)
		Notify (AC, 0x80)
		Notify (AC, 0x00)
		Notify (BAT0, 0x00)
		Notify (BAT0, 0x80)

		Store ("<----- EC: _Q31", Debug)
	}

	Method (_Q32, 0, NotSerialized)		// _Qxx: EC Query
	{
		Store ("-----> EC: _Q32", Debug)

		Sleep (2500)
		Notify (BAT0, 0x00)
		Notify (BAT0, 0x80)
		Notify (BAT0, 0x81)
		Notify (BAT0, 0x82)

		Store ("<----- EC: _Q32", Debug)
	}

	Method (_Q33, 0, NotSerialized)		// _Qxx: EC Query
	{
		Store ("-----> EC: _Q33", Debug)

		Sleep (2500)
		Notify (BAT0, 0x00)
		Notify (BAT0, 0x80)
		Notify (BAT0, 0x81)
		Notify (BAT0, 0x82)

		Store ("<---- EC: _Q33", Debug)
	}

	Method (_Q36, 0, NotSerialized)		// _Qxx: EC Query
	{
		Store ("-----> EC: _Q36", Debug)

		Notify (BAT0, 0x80)

		Store ("<----- EC: _Q36", Debug)
	}

	Method (_Q37, 0, NotSerialized)		// _Qxx: EC Query
	{
		Store ("-----> EC: _Q37", Debug)

		Notify (BAT0, 0x80)

		Store ("<----- EC: _Q37", Debug)
	}

	Method (_Q43, 0, NotSerialized)		// _Qxx: EC Query
	{
		Store ("-----> EC: _Q43", Debug)

		Store (BRIG, Local0)
		Increment (Local0)
		If (LGreater (Local0, 0xAA)) {
			Store (0xAA, Local0)
		}
		Store (Local0, BRIG)

		\_SB.PCI0.GFX0.INCB ()

		Store ("<---- EC: _Q43", Debug)
	}

	Method (_Q44, 0, NotSerialized)		// _Qxx: EC Query
	{
		Store ("-----> EC: _Q44", Debug)

		Store (BRIG, Local0)
		Decrement (Local0)
		If (LLess (Local0, 0xA0))
		{
			Store (0xA0, Local0)
		}
		Store (Local0, BRIG)

		\_SB.PCI0.GFX0.DECB ()

		Store ("<---- EC: _Q44", Debug)
	}

	Method (_Q45, 0, NotSerialized)		// _Qxx: EC Query
	{
		Store ("-----> EC: _Q45", Debug)

		Store (0, LIDS)
		Notify (LID, 0x80)

		Store ("<----- EC: _Q45", Debug)
	}

	Method (_Q46, 0, NotSerialized)		// _Qxx: EC Query
	{
		Store ("-----> EC: _Q46", Debug)

		Store (1, LIDS)
		Notify (LID, 0x80)

		Store ("<----- EC: _Q46", Debug)
	}

	Method (_Q70, 0, NotSerialized)		// _Qxx: EC Query
	{
		Store ("-----> EC: _Q70", Debug)

		Notify (ALSD, 0x80)

		Store ("<----- EC: _Q70", Debug)
	}

	#include "battery.asl"
	#include "ac.asl"
	#include "lid.asl"
}
