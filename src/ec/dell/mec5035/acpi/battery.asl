/* SPDX-License-Identifier: GPL-2.0-only */

Field(ERAM, ByteAcc, NoLock, Preserve)
{
	Offset (0x03),
			BIDX, 8,	/* Battery Index, 1 based */
			BSTI, 8,	/* Battery String Index */
	Offset (0x10),
			BAST, 8,	/* Battery State */
	Offset (0x12),
			BAPR, 16,	/* Battery Present Rate */
			BAVO, 16,	/* Battery Present Voltage */
			BARC, 16,	/* Battery Remaining Capacity */
	Offset (0x1E),
			BAFC, 16,	/* Last Full Charge Capacity */
			BADC, 16,	/* Design Capacity */
			BADV, 16,	/* Design Voltage */
	Offset (0x26),
			BASN, 16,	/* 16 bit numberic Serial Number */
			BAOE, 8,	/* OEM Information Enum */
			BATP, 8,	/* Battery Type Enum */
	Offset (0x2A),
			BSTR, 8,	/* String output port */
}


/* Package for _BST */
Name (BSTP, Package (4)
{
	0, /* Battery State */
	1, /* Battery Present Rate */
	0, /* Battery, Remaining Capacity */
	0 /* Battery Present Voltage */
})

/* Package for _BIF */
Name (BIFP, Package (13)
{
	1,		/*  0: Power Unit, always 1 = mA, mAh */
	0xFFFFFFFF,	/*  1: Design Capacity */
	0xFFFFFFFF,	/*  2: Last Full Charge Capacity */
	1,		/*  3: Battery Technology, always 1 = Rechargable */
	0xFFFFFFFF,	/*  4: Design Voltage */
	0,		/*  5: Design capacity of warning */
	0,		/*  6: Design capacity of low */
	0,		/*  7: Battery capacity granularity 1 */
	0,		/*  8: Battery capacity granularity 2 */
	"",		/*  9: Model Number */
	"",		/* 10: Serial Number */
	"",		/* 11: Battery Type */
	""		/* 12: OEM Information */
})

/* Arg0: String index
 * 1: Battery Model Number */
Method(RSTR, 1, NotSerialized)
{
	BSTI = Arg0
	Local0 = 1 /* character variable */
	Local1 = Buffer (32) {}
	Local2 = 0 /* Buffer index */

	/* Loop until Null terminator */
	While (Local0 && Local2 < 32)
	{
		Local0 = BSTR
		Local1[Local2] = Local0
		Local2++
	}

	Return(ToString (Local1, 32))

}

/* Battery _BST
 * Arg0: Battery Index, 1 based
 */
Method(BBST, 1, NotSerialized)
{
	Acquire (ECLK, 0xffff)
	Local0 = BSTP
	BIDX = Arg0

	If (!BPRT) {
		Release (ECLK)
		Return (Local0)
	}

	Local0[0] = BAST

	// Return negative discharge rate if charging
	Local1 = BAST && 0x2
	If (Local1) {
		Local0[1] = 0x10000 - BAPR
	} Else {
		Local0[1] = BAPR
	}

	Local0[2] = BARC
	Local0[3] = BAVO
	Release (ECLK)
	Return (Local0)
}

/* Battery _BIF
 * Arg0: Battery Index, 1 based
 */
Method(BBIF, 1, NotSerialized)
{
	Acquire (ECLK, 0xffff)
	Local0 = BIFP
	BIDX = Arg0
	Local0[1] = BADC
	Local0[2] = BAFC
	Local0[4] = BADV
	Local0[9] = RSTR(1)
	ToDecimalString(BASN, Local0[10])

	Release (ECLK)
	Return (Local0)
}

Device (BAT0)
{
	Name (_HID, EisaID ("PNP0C0A"))
	Name (STAT, Zero)

	Method (_BIF, 0, NotSerialized)
	{
		Return (BBIF (1))
	}

	Method (_BST, 0, NotSerialized)
	{
		Return (BBST (1))
	}

	Method (_STA, 0, NotSerialized)
	{
		if (BPRT) {
			Return (0x1f)
		} else {
			Return (0x0f)
		}
	}
}
