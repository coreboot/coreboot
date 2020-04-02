/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
		Store (BFCP, Index (PBIF, 2))

		/* Design Voltage */
		Store (BDVT, Index (PBIF, 4))

		/* Design Capacity */
		Store (BDCP, Index (PBIF, 1))

		/* Design Capacity of Warning */
		Store (BDCP / 0x32, Index (PBIF, 5))

		/* Design Capacity of Low */
		Store (BDCP / 0x64, Index (PBIF, 6))

		Store (ToString (BSER, Ones), Index (PBIF, 0x0A))

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
			Store (BCST, Local0)
		} Else {
			/* Always discharging when on battery power */
			Store (0x01, Local0)
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
		Store (BCRT, Index (PBST, 1))

		/*
		 * 2: BATTERY REMAINING CAPACITY
		 */
		Store (BRCP, Index (PBST, 2))

		/*
		 * 3: BATTERY PRESENT VOLTAGE
		 */
		Store (BCVT, Index (PBST, 3))

		Return (PBST)
	}
}
