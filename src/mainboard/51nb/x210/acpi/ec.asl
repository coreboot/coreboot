/* SPDX-License-Identifier: GPL-2.0-or-later */

Device (EC)
{
	Name (_HID, EisaId ("PNP0C09"))
	Name (_UID, 0)

	Name (_GPE, 0x4F)  // _GPE: General Purpose Events
	Name (_CRS, ResourceTemplate () {
		IO (Decode16, 0x62, 0x62, 1, 1)
		IO (Decode16, 0x66, 0x66, 1, 1)
	})

	OperationRegion (ERAM, EmbeddedControl, 0, 0xFF)
	Field (ERAM, ByteAcc, Lock, Preserve)
	{
		Offset (0x50),
		CTMP,   8,
		CFAN,   8,
		B1SS,   1,
		BSTS,   2,
		ACIN,   1,
		Offset (0x53),
		BKLG,   8,
		TOUP,   1,
		WIRE,   1,
		BLTH,   1,
		LIDC,   1,
		APFG,   1,
		WRST,   1,
		BTST,   1,
		ACEB,   1,
		CAME,   1,
		Offset (0x60),
		DGCP,   16,
		FLCP,   16,
		DGVO,   16,
		BDW,    16,
		BDL,    16,
		BPR,    16,
		BRC,    16,
		BPV,    16
	}

	Method (_REG, 2, NotSerialized)
	{
		/* Initialize AC power state */
		\PWRS = ACIN

		/* Initialize LID switch state */
		\LIDS = LIDC
	}

	/* KEY_BRIGHTNESSUP */
	Method (_Q04)
	{
		Notify(\_SB.PCI0.GFX0.LCD, 0x86)
	}

	/* KEY_BRIGHTNESSDOWN */
	Method (_Q05)
	{
		Notify(\_SB.PCI0.GFX0.LCD, 0x87)
	}

	/* Battery Information Event */
	Method (_Q0C)
	{
		Notify (BAT, 0x81)
	}

	/* AC event */
	Method (_Q0D)
	{
		\PWRS = ACIN
		Notify (AC, 0x80)
	}

	/* Lid event */
	Method (_Q0E)
	{
		\LIDS = LIDC
		Notify (LID0, 0x80)
	}

	/* Battery Information Event */
	Method (_Q13)
	{
		Notify (BAT, 0x81)
	}

	/* Battery Status Event */
	Method (_Q14)
	{
		Notify (BAT, 0x80)
	}

	Device (AC)
	{
		Name (_HID, "ACPI0003")
		Name (_PCL, Package () { \_SB })

		Method (_STA)
		{
			Return (0x0F)
		}
		Method (_PSR)
		{
			Return (\PWRS)
		}
	}

	#include "battery.asl"
}
