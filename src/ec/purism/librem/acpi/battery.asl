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
	Name (BFWK, 1)

	/* Method to enable full battery workaround */
	Method (BFWE)
	{
		BFWK = 1
	}

	/* Method to disable full battery workaround */
	Method (BFWD)
	{
		BFWK = 0
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
		PBIF [2] = BTDF

		/* Design Voltage */
		PBIF [4] = BTDV

		/* Design Capacity */
		Local0 = BTDA
		PBIF [1] = Local0

		/* Design Capacity of Warning */
		Local2 = (Local0 * DWRN) / 100
		PBIF [5] = Local2

		/* Design Capacity of Low */
		Local2 = (Local0 * DLOW) / 100
		PBIF [6] = Local2

		Return (PBIF)
	}

	Method (_BIX, 0, Serialized)
	{
		/* Last Full Charge Capacity */
		PBIX [3] = BTDF

		/* Design Voltage */
		PBIX [5] = BTDV

		/* Design Capacity */
		Local0 = BTDA
		PBIX [2] = Local0

		/* Design Capacity of Warning */
		Local2 = (Local0 * DWRN) / 100
		PBIX [6] = Local2

		/* Design Capacity of Low */
		Local2 = (Local0 * DLOW) / 100
		PBIX [7] = Local2

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
			Local0 = BSTS
		} Else {
			/* Always discharging when on battery power */
			Local0 = 0x01
		}

		/* Check for critical battery level */
		If (BFCR) {
			Local0 |= 0x04
		}
		PBST [0] = Local0

		/* Notify if battery state has changed since last time */
		If (Local0 != BSTP) {
			BSTP = Local0
			Notify (BAT, 0x80)
		}

		/*
		 * 1: BATTERY PRESENT RATE
		 */
		Local1 = BTPR
		If (Local1 & 0x8000) {
			Local0 = ~Local1 & 0x7FFF
			Local0++
		} Else {
			Local0 = Local1 & 0x7FFF
		}
		If(Local0 < 0x0352)
		{
			Local0 = 0x0352
		}
		PBST [1] = Local0

		/*
		 * 2: BATTERY REMAINING CAPACITY
		 */
		Local0 = BTRA
		If (BFWK && ACEX && !BSTS) {
			Local1 = BTDF

			/* See if within ~6% of full */
			Local2 = Local1 >> 4
			If ((Local0 > (Local1 - Local2)) && (Local0 < (Local1 + Local2)))
			{
				Local0 = Local1
			}
		}
		PBST [2] = Local0

		/*
		 * 3: BATTERY PRESENT VOLTAGE
		 */
		PBST [3] = BTVO

		Return (PBST)
	}
}
