/* SPDX-License-Identifier: GPL-2.0-only */

Field (ECRM, ByteAcc, NoLock, Preserve)
{
	Offset (0x84),
	    ,   3,
	LCTV,   1,
	BATP,   4,
	BPU,    1,
	Offset (0x86),
	BSEL,   4, /* battery select */
	Offset (0x87),
	LB1,    8,
	LB2,    8,
	BDC,    16,
	Offset (0x8D),
	BFC,    16, /* battery full capacity */
	BRTE,   16,
	BTC,    1,
	Offset (0x92),
	BME,    16,
	BDN,    8,
	BDV,    16, /* battery design voltage */
	BCV1,   16,
	BST,    4,
	Offset (0xC9),
	BSN,    16, /* battery serial number */
	BDAT,   16, /* battery date */
	BMF,    8,
}

Name (NGBF, 0xFF)
Name (NGBT, 0xFF)
Name (GACP, 0x07)
Name (NBAP, 0x00)
Name (NNBO, 0x01)
Name (NDCB, 0x00)
Mutex (BTMX, 0x00)

#define NLB1 0xC8 /* design capacity of warning */
#define NLB2 0x64 /* design capacity of low */

Method (ITLB, 0, NotSerialized)
{
	Local0 = (NLB1 + 9) % 0x0a
	Local1 = (NLB1 + 9) / 0x0a

	Local0 = (0x96 + 9) % 0x0a
	Local2 = (0x96 + 9) / 0x0a

	Local0 = Local0
	LB1 = Local1
	LB2 = Local2
}

Method (PWUP, 2, Serialized)
{
	Local0 = 0x00
	Acquire (BTMX, 0xFFFF)
	Local1 = (Arg0 | GACP)
	GACP = (Local1 & 0x07)
	If ((GACP & 0x02))
	{
		NGBF |= Arg1
	}

	If ((GACP & 0x04))
	{
		If ((NGBT != 0xFF))
		{
			Local0 = 0x01
		}

		NGBT |= Arg1
	}

	Release (BTMX)
	Return (Local0)
}

Method (GBAP, 0, Serialized)
{
	Acquire (BTMX, 0xFFFF)
	If ((GACP & 0x02))
	{
		GACP &= 0x05
		Release (BTMX)
		Acquire (ECMX, 0xFFFF)
		NBAP = BATP
		Release (ECMX)
	}
	Else
	{
		Release (BTMX)
	}
	Return (NBAP)
}

Method (BTDR, 1, Serialized)
{
	If ((Arg0 == 0x01))
	{
		NNBO = 0x01
	}
	ElseIf ((Arg0 == 0x00))
	{
		NNBO = 0x00
	}

	Return (NNBO)
}

Method (BSTA, 1, Serialized)
{
	BTDR (0x01)
	Local0 = GBAP ()
	Local1 = 0x0F
	If ((Local0 & Arg0))
	{
		Local1 = 0x1F
	}
	Return (Local1)
}

Device (BAT0)
{
	Name (_HID, EisaId ("PNP0C0A"))
	Name (_UID, 0x01)

	Method (_STA, 0, NotSerialized)
	{
		Return (BSTA (0x01))
	}

	Method (_BIF, 0, NotSerialized)
	{
		Return (BTIF (0x00))
	}

	Method (_BST, 0, NotSerialized)
	{
		Return (BTSZ (0x00))
	}

	Name (_PCL, Package (0x01)
	{
		\_SB
	})
}

Device (BAT1)
{
	Name (_HID, EisaId ("PNP0C0A"))
	Name (_UID, 0x02)

	Method (_STA, 0, NotSerialized)
	{
		Return (BSTA (0x02))
	}

	Method (_BIF, 0, NotSerialized)
	{
		Return (BTIF (0x01))
	}

	Method (_BST, 0, NotSerialized)
	{
		Return (BTSZ (0x01))
	}

	Name (_PCL, Package (0x01)
	{
		\_SB
	})
}

Name (NBTI, Package (0x02)
{
	Package (0x0D)
	{
		0x01, /* power unit: mA */
		0xFFFFFFFF, /* design capacity */
		0xFFFFFFFF, /* last full charge capacity */
		0x01, /* battery technology */
		0xFFFFFFFF, /* design voltage */
		0x00, /* design capacity of warning */
		0x00, /* design capacity of low */
		0x64, /* battery capacity granularity 1 */
		0x64, /* battery capacity granularity 2 */
		"Primary", /* model number */
		"100000", /* serial number */
		"LIon", /* battery type */
		"Hewlett-Packard" /* OEM information */
	},

	Package (0x0D)
	{
		0x01,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0x01,
		0xFFFFFFFF,
		0x00,
		0x00,
		0x64,
		0x64,
		"Travel",
		"100000",
		"LIon",
		"Hewlett-Packard"
	}
})

Name (NBST, Package (0x02)
{
	Package (0x04) {0x00, 0x00, 0x0FA0, 0x04B0},
	Package (0x04) {0x00, 0x00, 0x0FA0, 0x04B0}
})

Name (NDBS, Package (0x04)
{
	0x00,
	0x00,
	0x0FA0,
	0x04B0
})

Method (\ISTR, 2, Serialized)
{
	Name (NUMB, Buffer (6) { "     " })
	Local0 = Arg0
	Local1 = Arg1
	While (Local1)
	{
		Local1--
		Local2 = Local0 % 10
		Local0 = Local0 / 10
		NUMB[Local1] = Local2 + 48
	}
	ToString (NUMB, Arg1, Local3)
	Return (Local3)
}

Method (GBSS, 2, Serialized)
{
	Local3 = \ISTR (Arg0, 0x05)
	Concatenate (Local3, " ", Local4)
	Local0 = (Arg1 >> 0x09)
	Local1 = (Local0 + 1980)
	Local2 = \ISTR (Local1, 0x04)
	Concatenate (Local4, Local2, Local3)
	Concatenate (Local3, "/", Local4)
	Local0 = (Arg1 >> 0x05)
	Local1 = (Local0 & 0x0F)
	Local2 = \ISTR (Local1, 0x02)
	Concatenate (Local4, Local2, Local3)
	Concatenate (Local3, "/", Local4)
	Local1 = (Arg1 & 0x1F)
	Local2 = \ISTR (Local1, 0x02)
	Concatenate (Local4, Local2, Local3)
	Return (Local3)
}

Method (BTIG, 1, Serialized) /* in vendor DSDT is EC0.BTIF */
{
	Local7 = (0x01 << Arg0)
	BTDR (0x01)
	If ((BSTA (Local7) == 0x0F))
	{
		Return (0xFF)
	}

	Acquire (BTMX, 0xFFFF)
	Local0 = NGBF
	Release (BTMX)
	If (((Local0 & Local7) == 0x00))
	{
		Return (0x00)
	}

	NBST [Arg0] = NDBS
	Acquire (BTMX, 0xFFFF)
	NGBT |= Local7
	Release (BTMX)

	/* fill battery information */
	Acquire (ECMX, 0xFFFF)
	BSEL = Arg0
	Local0 = BFC
	DerefOf (NBTI [Arg0]) [0x01] = Local0
	DerefOf (NBTI [Arg0]) [0x02] = Local0
	DerefOf (NBTI [Arg0]) [0x04] = BDV
	DerefOf (NBTI [Arg0]) [0x05] = NLB1
	DerefOf (NBTI [Arg0]) [0x06] = NLB2
	Local0 = BSN
	Local1 = BDAT
	Release (ECMX)

	/* serial number */
	Local2 = GBSS (Local0, Local1)
	DerefOf (NBTI [Arg0]) [0x0A] = Local2

	Acquire (BTMX, 0xFFFF)
	NGBF &= ~Local7
	Release (BTMX)
	Return (0x00)
}

Method (BTIF, 1, Serialized) /* in vendor DSDT is SB.BTIF */
{
	Local0 = BTIG (Arg0)
	If ((Local0 == 0xFF))
	{
		Return (Package (0x0D)
		{
			0x00,
			0xFFFFFFFF,
			0xFFFFFFFF,
			0x01,
			0xFFFFFFFF,
			0x00,
			0x00,
			0x00,
			0x00,
			"",
			"",
			"",
			0x00
		})
	}
	Else
	{
		Return (DerefOf (NBTI [Arg0]))
	}
}

Name (NFBS, 0x01)
Method (BTSZ, 1, Serialized) /* in vendor DSDT is EC.BTST */
{
	Local1 = NFBS
	If (NFBS)
	{
		NFBS = 0x00
	}
	BTST (Arg0, Local1)
	Return (DerefOf (NBST [Arg0]))
}

Method (BTST, 2, Serialized)
{
	Local7 = (0x01 << Arg0)
	BTDR (0x01)
	If ((BSTA (Local7) == 0x0F))
	{
		NBST [Arg0] = Package (0x04)
		{
			0x00,
			0xFFFFFFFF,
			0xFFFFFFFF,
			0xFFFFFFFF
		}
		Return (0xFF)
	}

	Acquire (BTMX, 0xFFFF)
	If (Arg1)
	{
		NGBT = 0xFF
	}

	Local0 = NGBT /* \_SB_.PCI0.LPCB.EC0_.NGBT */
	Release (BTMX)
	If (((Local0 & Local7) == 0x00))
	{
		Return (0x00)
	}

	Acquire (ECMX, 0xFFFF)
	BSEL = Arg0
	Local0 = BST
	Local3 = BPR
	DerefOf (NBST [Arg0]) [0x02] = BRC
	DerefOf (NBST [Arg0]) [0x03] = BPV
	Release (ECMX)

	If ((GACS () == 0x01))
	{
		Local0 &= ~0x01
	}
	Else
	{
		Local0 &= ~0x02
	}

	If ((Local0 & 0x01))
	{
		Acquire (BTMX, 0xFFFF)
		NDCB = Local7
		Release (BTMX)
	}

	DerefOf (NBST [Arg0]) [0x00] = Local0
	If ((Local0 & 0x01))
	{
		If (((Local3 < 0x0190) || (Local3 > 0x1964)))
		{
			Local5 = DerefOf (DerefOf (NBST [Arg0]) [0x01])
			If (((Local5 < 0x0190) || (Local5 > 0x1964)))
			{
				Local3 = (0x1AF4 / 0x02)
			}
			Else
			{
				Local3 = Local5
			}
		}
	}
	ElseIf (((Local0 & 0x02) == 0x00))
	{
		Local3 = 0x00
	}

	DerefOf (NBST [Arg0]) [0x01] = Local3
	Acquire (BTMX, 0xFFFF)
	NGBT &= ~Local7
	Release (BTMX)
	Return (0x00)
}

Method (SBTN, 2, Serialized)
{
	If ((Arg0 & 0x01))
	{
		Notify (BAT0, Arg1)
	}

	If ((Arg0 & 0x02))
	{
		Notify (BAT1, Arg1)
	}
}

Method (_Q03, 0, NotSerialized)
{
	Printf ("EC: _Q03")
	Acquire (BTMX, 0xFFFF)
	Local0 = NDCB
	Release (BTMX)
	PWUP (0x04, Local0)
	SBTN (Local0, 0x80)
}

Method (_Q08, 0, NotSerialized)
{
	Printf ("EC: PRIMARY BATTERY ATTACHED/DETACHED")
	PWUP (0x06, 0x01)
	Local0 = GBAP ()
	If ((Local0 != 0x02))
	{
		PWUP (0x04, 0x02)
		If (BTDR (0x02))
		{
			Notify (BAT1, 0x80)
		}
	}

	If (BTDR (0x02))
	{
		Notify (BAT0, 0x81)
	}
}

Method (_Q09, 0, NotSerialized)
{
	Printf ("EC: PRIMARY BATTERY STATUS")
	PWUP (0x04, 0x01)
	If (BTDR (0x02))
	{
		Notify (BAT0, 0x80)
	}
}

Method (_Q18, 0, NotSerialized)
{
	Printf ("EC: SECONDARY BATTERY ATTACHED/DETACHED")
	PWUP (0x06, 0x02)
	Local0 = GBAP ()
	If ((Local0 != 0x01))
	{
		PWUP (0x04, 0x01)
		If (BTDR (0x02))
		{
			Notify (BAT0, 0x80)
		}
	}

	If (BTDR (0x02))
	{
		Notify (BAT1, 0x81) // Information Change
	}
}

Method (_Q19, 0, NotSerialized)
{
	Printf ("EC: SECONDARY BATTERY STATUS")
	PWUP (0x04, 0x02)
	If (BTDR (0x02))
	{
		Notify (BAT1, 0x80)
	}
}
