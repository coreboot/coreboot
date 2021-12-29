/* SPDX-License-Identifier: GPL-2.0-or-later */

Device (BAT)
{
	Name (_HID, EisaId ("PNP0C0A"))
	Name (_UID, 1)
	Name (_PCL, Package () { \_SB })

	Method (_STA, 0, NotSerialized)  // _STA: Status
	{
		If (B1SS)
		{
			Return (0x1F)
		}
		Else
		{
			Return (0x0F)
		}
	}

	Name (PBIF, Package () {
		0x00000001,  /* 0x00: Power Unit: mAH */
		0xFFFFFFFF,  /* 0x01: Design Capacity */
		0xFFFFFFFF,  /* 0x02: Last Full Charge Capacity */
		0x00000001,  /* 0x03: Battery Technology: Rechargeable */
		0xFFFFFFFF,  /* 0x04: Design Voltage */
		0x00000000,  /* 0x05: Design Capacity of Warning */
		0xFFFFFFFF,  /* 0x06: Design Capacity of Low */
		0x00000001,  /* 0x07: Capacity Granularity 1 */
		0x00000001,  /* 0x08: Capacity Granularity 2 */
		"Y91",       /* 0x09: Model Number */
		"",          /* 0x0a: Serial Number */
		"LION",      /* 0x0b: Battery Type */
		"CJOYIN"     /* 0x0c: OEM Information */
	})

	Method (_BIF, 0, Serialized)
	{
		/* Design Capacity */
		Store (DGCP * 10000 / DGVO, PBIF[1])

		/* Last Full Charge Capacity */
		Store (FLCP * 10000 / DGVO, PBIF[2])

		/* Design Voltage */
		Store (DGVO, PBIF[4])

		/* Design Capacity of Warning */
		Store (BDW * 10000 / DGVO, PBIF[5])

		/* Design Capacity of Low */
		Store (BDL, PBIF[6])

		Return (PBIF)
	}

	Name (PBST, Package () {
		0x00000000,  /* 0x00: Battery State */
		0xFFFFFFFF,  /* 0x01: Battery Present Rate */
		0xFFFFFFFF,  /* 0x02: Battery Remaining Capacity */
		0xFFFFFFFF,  /* 0x03: Battery Present Voltage */
	})

	Method (_BST, 0, NotSerialized)  // _BST: Battery Status
	{
		/*
		* 0: BATTERY STATE
		*
		* bit 0 = discharging
		* bit 1 = charging
		* bit 2 = critical level
		*/
		Store (BSTS, PBST[0])

		/*
		* 1: BATTERY PRESENT RATE
		*/
		Store (BPR, PBST[1])

		/*
		* 2: BATTERY REMAINING CAPACITY
		*/
		Store (BRC * 10000 / DGVO, PBST[2])

		/*
		* 3: BATTERY PRESENT VOLTAGE
		*/
		Store (BPV, PBST[3])

		Return (PBST)
	}
}
