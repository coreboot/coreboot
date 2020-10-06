/* SPDX-License-Identifier: GPL-2.0-only */

Device (EC)
{
	Name (_HID, EisaId ("PNP0C09"))
	Name (_UID, 0)
	Name (_GPE, 0x50) // Copied over

	Name (_CRS, ResourceTemplate () {
		IO (Decode16, 0x62, 0x62, 0, 1)
		IO (Decode16, 0x66, 0x66, 0, 1)
	})

	Name (ACEX, 0)

	OperationRegion (ERAM, EmbeddedControl, 0x00, 0xFF)
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset (0x1C),
		ODP1,   8,
		ODP2,   8,
		Offset (0x56),
		CPUT,   8,
		CPU1,   8,
		GPUT,   8,
		ADPV,   16,
		ADPC,   16,
		FANC,   8,
		Offset (0x60),
		BSER,   256, // BAT Serial Number
		Offset (0x90),
		BIF0,   16,
		BDCP,   16, // BAT Design Capacity
		BFCP,   16, // BAT Full Capacity
		BRCH,   16, // BAT Rechargable
		BDVT,   16, // BAT Design Voltage
		BIF5,   16,
		BIF6,   16,
		BIF7,   16,
		BIF8,   16,
		BCST,   16, // BAT Current State
		BCRT,   16, // BAT Current Rate
		BRCP,   16, // BAT Remaining Capacity
		BCVT,   16, // BAT Current Voltage
		PWRS,   8,  // Power State (?)
		ECN0,   8,
		Offset (0xB0),
		SRNM,   16,
		MFDA,   16,
		PHMR,   8,
		BLDA,   8,
		Offset (0xE2),
		LIDS,   8   // Lid state
	}

	Method (_REG, 2, NotSerialized)
	{
		/* Initialize AC power state */
		ACEX = PWRS - 0x82

		/* Initialize LID switch state */
		\LIDS = LIDS
	}


	// Close ?
	Method (_Q14, 0, NotSerialized)
	{
		\LIDS = LIDS
		Notify (LID0, 0x80)
	}

	//Open
	Method (_Q15, 0, NotSerialized)
	{
		\LIDS = LIDS
		Notify (LID0, 0x80)
	}


	// AC plugged
	Method (_Q13, 0, NotSerialized)
	{
		ACEX = PWRS - 0x82
		Notify (BAT, 0x80) // Status Change
		Notify (BAT, 0x81) // Information Change
		Notify (AC, 0x80) // Status Change
	}

	#include "ac.asl"
	#include "battery.asl"
}
