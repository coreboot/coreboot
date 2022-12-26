/* SPDX-License-Identifier: GPL-2.0-only */

Mutex (BATM, 0)

Method (ESTA, 1, Serialized)
{
	/* Check battery presence */
	If (Arg0 == 1) {
		Local0 = R (BTEX)
	} ElseIf (Arg0 == 2) {
		Local0 = R (P3B2)
	} Else {
		Local0 = Zero
	}

	/* Indicate battery is present */
	If (Local0) {
		Return (0x1f)
	} Else {
		Return (0xf)
	}
}

Name (BSTM, 0x01)	/* Battery Model String */
Name (BSTC, 0x02)	/* Battery Chemistry String */
Name (BSTG, 0x03)	/* Battery Manufacturer String */

/*
 * Read a battery string from the EC
 *  Arg0 = Battery String ID
 *         BSTM = Model
 *         BSTC = Chemical
 *         BSTG = Manufacturer
 */
Method (BSTR, 1, Serialized)
{
	/* Max string length */
	Local0 = 32

	/* Select battery string */
	W (SSEL, Arg0)

	/* Fill buffer until NUL or max string length is read */
	Name (IBUF, Buffer (Local0 + 1) {})
	For (Local1 = Zero, Local1 < Local0, Local1++) {
		Local2 = R (BSBS)
		If (!Local2) {
			Break
		}
		IBUF[Local1] = Local2
	}

	/* Truncate and NUL terminate */
	Local1++
	Name (OBUF, Buffer (Local1) {})
	OBUF = Concatenate (IBUF, Zero)

	Return (OBUF)
}

/*
 * Handle Battery Status
 *  Arg0 = Battery Number
 *  Arg1 = _BST package
 */
Method (EBST, 2, Serialized)
{
	If (Acquire (BATM, 1000)) {
		Return (Arg1)
	}

	/* Select battery */
	W (BSEL, Arg0)

	/* 0: Battery State */
	Arg1[0] = R (BCST)

	/* 1: Battery Present Rate */
	Local0 = R (BCCL)
	If (!Local0) {
		Local0++
	} ElseIf (R (ACEX)) {
		If (Local0 & 0x8000) {
			Local0 = Ones
		}
	} Else {
		If (Local0 & 0x8000) {
			Local0 = 0 - Local0
			Local0 &= 0xffff
		} Else {
			Local0 = Ones
		}
	}
	Arg1[1] = Local0

	/* 2: Battery Remaining Capacity */
	Arg1[2] = R (BCYL)

	/* 3: Battery Present Voltage */
	Arg1[3] = R (BCVL)

	Release (BATM)
	Return (Arg1)
}

/*
 * Handle Battery Info
 *  Arg0 = Battery Number
 *  Arg1 = _BIF package
 */
Method (EBIF, 2, Serialized)
{
	If (Acquire (BATM, 1000)) {
		Return (Arg1)
	}

	/* Select battery */
	W (BSEL, Arg0)

	/* Design Capacity */
	Local0 = R (BSCL)
	Arg1[1] = Local0

	/* Last Full Charge Capacity */
	Arg1[2] = R (BSFL)

	/* Design Voltage */
	Arg1[4] = R (BSVL)

	/* Design Warning Capacity */
	Arg1[5] = (Local0 * 10) / 100

	/* Design Low Capacity */
	Arg1[6] = (Local0 * 33) / 100

	/* Granularity */
	Arg1[7] = Local0 / 100
	Arg1[8] = Local0 / 100

	/* Battery Model Number */
	Arg1[9] = BSTR (BSTM)

	/* Battery Serial Number */
	Arg1[10] = ToDecimalString (R (BSSL))

	/* Battery Type */
	Arg1[11] = BSTR (BSTC)

	/* Battery Manufacturer Name */
	Arg1[12] = BSTR (BSTG)

	Release (BATM)
	Return (Arg1)
}

/*
 * Handle Battery Info Extended
 *  Arg0 = Battery Number
 *  Arg1 = _BIX package
 */
Method (EBIX, 2, Serialized)
{
	If (Acquire (BATM, 1000)) {
		Return (Arg1)
	}

	/* Select battery */
	W (BSEL, Arg0)

	/* Design Capacity */
	Local0 = R (BSCL)
	Arg1[2] = Local0

	/* Last Full Charge Capacity */
	Arg1[3] = R (BSFL)

	/* Design Voltage */
	Arg1[5] = R (BSVL)

	/* Design Warning Capacity */
	Arg1[6] = (Local0 * 10) / 100

	/* Design Low Capacity */
	Arg1[7] = (Local0 * 33) / 100

	/* Cycle Count */
	Arg1[8] = R (BCCY)

	/* Granularity */
	Arg1[14] = Local0 / 100
	Arg1[15] = Local0 / 100

	/* Battery Model Number */
	Arg1[16] = BSTR (BSTM)

	/* Battery Serial Number */
	Arg1[17] = ToDecimalString (R (BSSL))

	/* Battery Type */
	Arg1[18] = BSTR (BSTC)

	/* Battery Manufacturer Name */
	Arg1[19] = BSTR (BSTG)

	Release (BATM)
	Return (Arg1)
}

Device (BAT0)
{
	Name (_HID, EisaId ("PNP0C0A"))
	Name (_UID, 1)
	Name (_PCL, Package () { \_SB })

	Name (PBIF, Package () {
		0x00000001,	/* 0x00: Power Unit: mAh */
		0xFFFFFFFF,	/* 0x01: Design Capacity */
		0xFFFFFFFF,	/* 0x02: Last Full Charge Capacity */
		0x00000001,	/* 0x03: Battery Technology: Rechargeable */
		0xFFFFFFFF,	/* 0x04: Design Voltage */
		0x00000003,	/* 0x05: Design Capacity of Warning */
		0xFFFFFFFF,	/* 0x06: Design Capacity of Low */
		0x00000001,	/* 0x07: Capacity Granularity 1 */
		0x00000001,	/* 0x08: Capacity Granularity 2 */
		"",		/* 0x09: Model Number */
		"",		/* 0x0a: Serial Number */
		"LION",		/* 0x0b: Battery Type */
		""		/* 0x0c: OEM Information */
	})

	Name (PBIX, Package () {
		0x00000000,	/* 0x00: Revision */
		0x00000001,	/* 0x01: Power Unit: mAh */
		0xFFFFFFFF,	/* 0x02: Design Capacity */
		0xFFFFFFFF,	/* 0x03: Last Full Charge Capacity */
		0x00000001,	/* 0x04: Battery Technology: Rechargeable */
		0xFFFFFFFF,	/* 0x05: Design Voltage */
		0x00000003,	/* 0x06: Design Capacity of Warning */
		0xFFFFFFFF,	/* 0x07: Design Capacity of Low */
		0x00000000,	/* 0x08: Cycle Count */
		0x00018000,	/* 0x09: Measurement Accuracy (98.3%?) */
		0x000001F4,	/* 0x0a: Max Sampling Time (500ms) */
		0x0000000a,	/* 0x0b: Min Sampling Time (10ms) */
		0xFFFFFFFF,	/* 0x0c: Max Averaging Interval */
		0xFFFFFFFF,	/* 0x0d: Min Averaging Interval */
		0x00000001,	/* 0x0e: Capacity Granularity 1 */
		0x00000001,	/* 0x0f: Capacity Granularity 2 */
		"",		/* 0x10 Model Number */
		"",		/* 0x11: Serial Number */
		"LION",		/* 0x12: Battery Type */
		""		/* 0x13: OEM Information */
	})

	Name (PBST, Package () {
		0x00000000,	/* 0x00: Battery State */
		0xFFFFFFFF,	/* 0x01: Battery Present Rate */
		0xFFFFFFFF,	/* 0x02: Battery Remaining Capacity */
		0xFFFFFFFF,	/* 0x03: Battery Present Voltage */
	})

	Method (_STA, 0, Serialized)
	{
		Return (ESTA (^_UID))
	}

	Method (_BST, 0, Serialized)
	{
		Return (EBST (^_UID, PBST))
	}

	Method (_BIF, 0, Serialized)
	{
		Return (EBIF (^_UID, PBIF))
	}

	Method (_BIX, 0, Serialized)
	{
		Return (EBIX (^_UID, PBIX))
	}
}

Device (BAT1)
{
	Name (_HID, EisaId ("PNP0C0A"))
	Name (_UID, 2)
	Name (_PCL, Package () { \_SB })

	Name (PBIF, Package () {
		0x00000001,	/* 0x00: Power Unit: mAh */
		0xFFFFFFFF,	/* 0x01: Design Capacity */
		0xFFFFFFFF,	/* 0x02: Last Full Charge Capacity */
		0x00000001,	/* 0x03: Battery Technology: Rechargeable */
		0xFFFFFFFF,	/* 0x04: Design Voltage */
		0x00000003,	/* 0x05: Design Capacity of Warning */
		0xFFFFFFFF,	/* 0x06: Design Capacity of Low */
		0x00000001,	/* 0x07: Capacity Granularity 1 */
		0x00000001,	/* 0x08: Capacity Granularity 2 */
		"",		/* 0x09: Model Number */
		"",		/* 0x0a: Serial Number */
		"LION",		/* 0x0b: Battery Type */
		""		/* 0x0c: OEM Information */
	})

	Name (PBIX, Package () {
		0x00000000,	/* 0x00: Revision */
		0x00000001,	/* 0x01: Power Unit: mAh */
		0xFFFFFFFF,	/* 0x02: Design Capacity */
		0xFFFFFFFF,	/* 0x03: Last Full Charge Capacity */
		0x00000001,	/* 0x04: Battery Technology: Rechargeable */
		0xFFFFFFFF,	/* 0x05: Design Voltage */
		0x00000003,	/* 0x06: Design Capacity of Warning */
		0xFFFFFFFF,	/* 0x07: Design Capacity of Low */
		0x00000000,	/* 0x08: Cycle Count */
		0x00018000,	/* 0x09: Measurement Accuracy (98.3%?) */
		0x000001F4,	/* 0x0a: Max Sampling Time (500ms) */
		0x0000000a,	/* 0x0b: Min Sampling Time (10ms) */
		0xFFFFFFFF,	/* 0x0c: Max Averaging Interval */
		0xFFFFFFFF,	/* 0x0d: Min Averaging Interval */
		0x00000001,	/* 0x0e: Capacity Granularity 1 */
		0x00000001,	/* 0x0f: Capacity Granularity 2 */
		"",		/* 0x10 Model Number */
		"",		/* 0x11: Serial Number */
		"LION",		/* 0x12: Battery Type */
		""		/* 0x13: OEM Information */
	})

	Name (PBST, Package () {
		0x00000000,	/* 0x00: Battery State */
		0xFFFFFFFF,	/* 0x01: Battery Present Rate */
		0xFFFFFFFF,	/* 0x02: Battery Remaining Capacity */
		0xFFFFFFFF,	/* 0x03: Battery Present Voltage */
	})

	Method (_STA, 0, Serialized)
	{
		Return (ESTA (^_UID))
	}

	Method (_BST, 0, Serialized)
	{
		Return (EBST (^_UID, PBST))
	}

	Method (_BIF, 0, Serialized)
	{
		Return (EBIF (^_UID, PBIF))
	}

	Method (_BIX, 0, Serialized)
	{
		Return (EBIX (^_UID, PBIX))
	}
}
