/* SPDX-License-Identifier: GPL-2.0-only */

Device (BAT)
{
	Name (_HID, EisaId ("PNP0C0A"))
	Name (_UID, 1)
	Name (_PCL, Package () { \_SB })

	Name (BSTP, 0)

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
		"Razer Blade Stealth",
		"SERIAL",
		"LiIon",
		"Razer"
	})


	Name (PBST, Package () {
		0x00000000,  /* 0x00: Battery State */
		0xFFFFFFFF,  /* 0x01: Battery Present Rate */
		0xFFFFFFFF,  /* 0x02: Battery Remaining Capacity */
		0xFFFFFFFF,  /* 0x03: Battery Present Voltage */
	})

	Method (_STA, 0, Serialized)
	{
		Return (0x1F)
	}

	Method (_BIF, 0, Serialized)
	{
		/* Last Full Charge Capacity */
		PBIF [2] = BFCP

		/* Design Voltage */
		PBIF [4] = BDVT

		/* Design Capacity */
		PBIF [1] = BDCP

		/* Design Capacity of Warning */
		PBIF [5] = BDCP / 50

		/* Design Capacity of Low */
		PBIF [6] = BDCP / 100

		PBIF [10] = ToString (BSER, Ones)

		Return (PBIF)
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
			Local0 = BCST
		} Else {
			/* Always discharging when on battery power */
			Local0 = 0x01
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
		PBST [1] = BCRT

		/*
		 * 2: BATTERY REMAINING CAPACITY
		 */
		PBST [2] = BRCP

		/*
		 * 3: BATTERY PRESENT VOLTAGE
		 */
		PBST [3] = BCVT

		Return (PBST)
	}
}
