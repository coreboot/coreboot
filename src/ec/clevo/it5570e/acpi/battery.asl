/* SPDX-License-Identifier: GPL-2.0-only */

#define PBST_STATE		 0
#define PBST_PRESENT_RATE	 1
#define PBST_REMAINING_CAP	 2
#define PBST_PRESENT_VOLT	 3

#define PBIX_DESIGN_CAP		 2
#define PBIX_LAST_FULL_CHG_CAP	 3
#define PBIX_DESIGN_VOLT	 5
#define PBIX_DESIGN_CAP_WARN	 6
#define PBIX_DESIGN_CAP_LOW	 7
#define PBIX_CYCLE_COUNT	 8
#define PBIX_MODEL		16
#define PBIX_SERIAL		17
#define PBIX_TYPE		18
#define PBIX_OEM_INFO		19

Device (BAT0)
{
	Name (_HID, EisaId ("PNP0C0A"))
	Name (_UID, 0)
	Name (_PCL, Package () { \_SB })

	Name (BFCC, 0) /* Full Charge Capacity */

	Name (PBST, Package () {
		0,	// Battery State
			//   [0] : Discharging
			//   [1] : Charging
			//   [2] : Critical Low
		0,	// Battery Present Rate
		0,	// Battery Remaining Capacity
		0,	// Battery Present Voltage
	})

	Name (PBIX, Package () {
		1,	// Revision
		1,	// Power Unit (1 = mA(h))
		0,	// Design Capacity
		0,	// Last Full Charge Capacity
		1,	// Battery Technology (1 = rechargeable)
		0,	// Design Voltage
		0,	// Design Capacity of Warning
		0,	// Design Capacity of Low
		0,	// Cycle Count
		95000,	// Measurement Accuracy (95 %)
		0,	// Max Sampling Time
		0,	// Min Sampling Time
		0,	// Max Averaging Interval
		0,	// Min Averaging Interval
		1,	// Battery Capacity Granularity 1 (low < warning)
		1,	// Battery Capacity Granularity 2 (warning < full)
		" ",	// Model Number
		" ",	// Serial Number
		" ",	// Battery Type
		" ",	// OEM Information
		0,	// Battery Swapping Capability (0 = not swappable)
	})

	Method (_STA)
	{
		Local0 = 0x0f

		If (\_SB.PCI0.LPCB.EC0.BAT0)
		{
			Local0 |= 0x10 /* battery present */
		}

		Return (Local0)
	}

	Method (_BST)
	{
		/*
		* Trigger update of static info update when
		* the last full charge capacity changes.
		* (This is what the vendor does.)
		*/
		If (BFCC != ToInteger (\_SB.PCI0.LPCB.EC0.BFC0))
		{
			Notify (BAT0, 0x81) /* information change */
		}

		/* Convert signed current to absolute value */
		Local0 = ToInteger (\_SB.PCI0.LPCB.EC0.BPR0)
		If (Local0 & 0x8000)
		{
			Local0 = (~Local0 & 0xffff) + 1
		}

		PBST [PBST_STATE]		= ToInteger (\_SB.PCI0.LPCB.EC0.BST0)
		PBST [PBST_PRESENT_RATE]	= Local0
		PBST [PBST_REMAINING_CAP]	= ToInteger (\_SB.PCI0.LPCB.EC0.BRC0)
		PBST [PBST_PRESENT_VOLT]	= ToInteger (\_SB.PCI0.LPCB.EC0.BPV0)

		Return (PBST)
	}

	Method (_BIX)
	{
		BFCC				= ToInteger   (\_SB.PCI0.LPCB.EC0.BFC0)
		PBIX [PBIX_DESIGN_CAP]		= ToInteger   (\_SB.PCI0.LPCB.EC0.BDC0)
		PBIX [PBIX_LAST_FULL_CHG_CAP]	= ToInteger   (\_SB.PCI0.LPCB.EC0.BFC0)
		PBIX [PBIX_DESIGN_VOLT]		= ToInteger   (\_SB.PCI0.LPCB.EC0.BDV0)
		PBIX [PBIX_DESIGN_CAP_WARN]	= ToInteger   (\_SB.PCI0.LPCB.EC0.BCW0)
		PBIX [PBIX_DESIGN_CAP_LOW]	= ToInteger   (\_SB.PCI0.LPCB.EC0.BCL0)
		PBIX [PBIX_CYCLE_COUNT]		= ToInteger   (\_SB.PCI0.LPCB.EC0.CYC0)
		PBIX [PBIX_MODEL]		= ToBuffer    (\_SB.PCI0.LPCB.EC0.BMO0)
		PBIX [PBIX_SERIAL]		= ToHexString (\_SB.PCI0.LPCB.EC0.BSN0)
		PBIX [PBIX_TYPE]		= ToBuffer    (\_SB.PCI0.LPCB.EC0.BTY0)
		PBIX [PBIX_OEM_INFO]		= ToBuffer    (\_SB.PCI0.LPCB.EC0.BIF0)

		Return (PBIX)
	}
}
