/* SPDX-License-Identifier: GPL-2.0-only */

Device (BAT)
{
	Name (_HID, EisaId ("PNP0C0A"))
	Name (_UID, 1)
	Name (_PCL, Package () { \_SB })

	Name (PBIF, Package () {
		0x00000001,  /* 0x00: Power Unit: mAh */
		0xFFFFFFFF,  /* 0x01: Design Capacity */
		0xFFFFFFFF,  /* 0x02: Last Full Charge Capacity */
		0x00000001,  /* 0x03: Battery Technology: Rechargeable */
		0xFFFFFFFF,  /* 0x04: Design Voltage */
		0x00000003,  /* 0x05: Design Capacity of Warning */
		0xFFFFFFFF,  /* 0x06: Design Capacity of Low */
		0x00000001,  /* 0x07: Capacity Granularity 1 */
		0x00000001,  /* 0x08: Capacity Granularity 2 */
		"S10",       /* 0x09: Model Number */
		"",          /* 0x0a: Serial Number */
		"LION",      /* 0x0b: Battery Type */
		"TPS"        /* 0x0c: OEM Information */
	})

	Name (PBIX, Package () {
		0x00000000,  // 0x00: Revision
		0x00000001,  // 0x01: Power Unit: mAh
		0xFFFFFFFF,  // 0x02: Design Capacity
		0xFFFFFFFF,  // 0x03: Last Full Charge Capacity
		0x00000001,  // 0x04: Battery Technology: Rechargeable
		0xFFFFFFFF,  // 0x05: Design Voltage
		0x00000003,  // 0x06: Design Capacity of Warning
		0xFFFFFFFF,  // 0x07: Design Capacity of Low
		0x00000000,  // 0x08: Cycle Count
		0x00018000,  // 0x09: Measurement Accuracy (98.3%?)
		0x000001F4,  // 0x0a: Max Sampling Time (500ms)
		0x0000000a,  // 0x0b: Min Sampling Time (10ms)
		0xFFFFFFFF,  // 0x0c: Max Averaging Interval
		0xFFFFFFFF,  // 0x0d: Min Averaging Interval
		0x00000001,  // 0x0e: Capacity Granularity 1
		0x00000001,  // 0x0f: Capacity Granularity 2
		"S10",       // 0x10: Model Number
		"",          // 0x11: Serial Number
		"LION",      // 0x12: Battery Type
		"TPS"        // 0x13: OEM Information
	})

	Name (PBST, Package () {
		0x00000000,  /* 0x00: Battery State */
		0xFFFFFFFF,  /* 0x01: Battery Present Rate */
		0xFFFFFFFF,  /* 0x02: Battery Remaining Capacity */
		0xFFFFFFFF,  /* 0x03: Battery Present Voltage */
	})
	Name (BSTP, Zero)

	/* Battery Capacity warning at 15% */
	Name (DWRN, 15)

	/* Battery Capacity low at 10% */
	Name (DLOW, 10)

	/* Workaround for full battery status, enabled by default */
	Name (BFWK, One)

	/* Method to enable full battery workaround */
	Method (BFWE)
	{
		Store (One, BFWK)
	}

	/* Method to disable full battery workaround */
	Method (BFWD)
	{
		Store (Zero, BFWK)
	}

	Method (_STA, 0, Serialized)
	{
		If (BTEX) {
			Return (0x1F)
		} Else {
			Return (0x0F)
		}
	}

	Method (_BIF, 0, Serialized)
	{
		/* Last Full Charge Capacity */
		Store (BTDF, Index (PBIF, 2))

		/* Design Voltage */
		Store (BTDV, Index (PBIF, 4))

		/* Design Capacity */
		Store (BTDA, Local0)
		Store (Local0, Index (PBIF, 1))

		/* Design Capacity of Warning */
		Divide (Multiply (Local0, DWRN), 100, , Local2)
		Store (Local2, Index (PBIF, 5))

		/* Design Capacity of Low */
		Divide (Multiply (Local0, DLOW), 100, , Local2)
		Store (Local2, Index (PBIF, 6))

		Return (PBIF)
	}

	Method (_BIX, 0, Serialized)
	{
		/* Last Full Charge Capacity */
		Store (BTDF, Index (PBIX, 3))

		/* Design Voltage */
		Store (BTDV, Index (PBIX, 5))

		/* Design Capacity */
		Store (BTDA, Local0)
		Store (Local0, Index (PBIX, 2))

		/* Design Capacity of Warning */
		Divide (Multiply (Local0, DWRN), 100, , Local2)
		Store (Local2, Index (PBIX, 6))

		/* Design Capacity of Low */
		Divide (Multiply (Local0, DLOW), 100, , Local2)
		Store (Local2, Index (PBIX, 7))

		Return (PBIX)
	}

	Method (_BST, 0, Serialized)
	{
		/*
		 * 0: BATTERY STATE
		 *
		 * bit 0 = discharging
		 * bit 1 = charging
		 * bit 2 = critical level
		 */

		/* Check if AC is present */
		If (ACEX) {
			/* Read battery status from EC */
			Store (BSTS, Local0)
		} Else {
			/* Always discharging when on battery power */
			Store (0x01, Local0)
		}

		/* Check for critical battery level */
		If (BFCR) {
			Or (Local0, 0x04, Local0)
		}
		Store (Local0, Index (PBST, 0))

		/* Notify if battery state has changed since last time */
		If (LNotEqual (Local0, BSTP)) {
			Store (Local0, BSTP)
			Notify (BAT, 0x80)
		}

		/*
		 * 1: BATTERY PRESENT RATE
		 */
		Store (BTPR, Local1)
		If (And (Local1, 0x8000)) {
			And (Not (Local1), 0x7FFF, Local0)
			Increment (Local0)
		} Else {
			And (Local1, 0x7FFF, Local0)
		}
		If(LLess(Local0, 0x0352))
		{
			Store(0x0352, Local0)
		}
		Store (Local0, Index (PBST, 1))

		/*
		 * 2: BATTERY REMAINING CAPACITY
		 */
		Store (BTRA, Local0)
		If (LAnd (BFWK, LAnd (ACEX, LNot (BSTS)))) {
			Store (BTDF, Local1)

			/* See if within ~6% of full */
			ShiftRight (Local1, 4, Local2)
			If (LAnd (LGreater (Local0, Subtract (Local1, Local2)),
			          LLess (Local0, Add (Local1, Local2))))
			{
				Store (Local1, Local0)
			}
		}
		Store (Local0, Index (PBST, 2))

		/*
		 * 3: BATTERY PRESENT VOLTAGE
		 */
		Store (BTVO, Index (PBST, 3))

		Return (PBST)
	}
}
