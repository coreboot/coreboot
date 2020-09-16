/* SPDX-License-Identifier: GPL-2.0-only */

Device (EC0)
{
	Name (_HID, EISAID("PNP0C09"))
	Name (_UID, 0)
	Name (_GPE, CONFIG_EC_HP_KBC1126_GPE)

	Name (_CRS, ResourceTemplate ()
	{
		IO (Decode16, 0x62, 0x62, 1, 1)
		IO (Decode16, 0x66, 0x66, 1, 1)
	})

	Method (_STA, 0, NotSerialized)
	{
		Return (0x0F)
	}

	OperationRegion (ECRM, EmbeddedControl, 0x00, 0xFF)
	Field (ECRM, ByteAcc, NoLock, Preserve)
	{
		PMCD,   32,
		S0FL,   8,
		SXF0,   8,
		SXF1,   8,
		CPWR,   16,
		CVLT,   16,
		CCUR,   16,
		DIDX,   8,
		CIDX,   8,
		PMCC,   8,
		PMEP,   8,
		Offset (0x22),
		CRZN,   8,
		THTA,   8,
		HYST,   8,
		CRIT,   8,
		TEMP,   8,
		TENA,   8,
		Offset (0x29),
		TOAD,   8,
		PHTP,   8,
		THEM,   8,
		TMPO,   8,
		Offset (0x2E),
		FRDC,   8,
		FTGC,   8,
		PLTP,   8,
		Offset (0x32),
		DTMP,   8,
		Offset (0x35),
		PL1,    8,
		PL2,    8,
		BCVD,   8,
		Offset (0x40),
		ABDI,   8,
		ABAD,   8,
		ABIX,   8,
		ABDA,   8,
		ABST,   8,
		PORI,   8,
		Offset (0x4C),
		PSSB,   8,
		Offset (0x80),
		Offset (0x81),
		    ,   4,
		SLPT,   4,
		FNSW,   1,
		SFNC,   1,
		ACPI,   1,
		    ,   1,
		    ,   1,
		    ,   1,
		    ,   1,
		DETF,   1,
		LIDS,   1,
		TBLT,   1,
		    ,   1,
		    ,   1,
		    ,   1,
		COMM,   1,
		PME,    1,
		SBVD,   1,

		/* 0x84 to 0x9A is in battery.asl */

		Offset (0x9B),
		BATE,   16,
		BPR,    16,
		BCR,    16,
		BRC,    16,
		BCC,    16,
		BPV,    16,
		BCV2,   16,
		BCV3,   16,
		BCV4,   16,
		BCW,    16,
		BATF,   16,
		BCL,    16,
		MAXC,   16,
		BCG1,   8,
		BT1I,   1,
		BT2I,   1,
		    ,   2,
		BATN,   4,
		BSTS,   16,
		BCG2,   8,
		Offset (0xBD),
		BMO,    8,
		Offset (0xBF),
		BRCV,   8,
		Offset (0xC1),
		BIF,    8,
		BRCC,   8,

		/* 0xC9 to 0xCD in battery.asl */

		Offset (0xCF),
		CTLB,   8,
		Offset (0xD1),
		BTY,    8,
		Offset (0xD5),
		MFAC,   8,
		CFAN,   8,
		PFAN,   8,
		OCPS,   8,
		OCPR,   8,
		OCPE,   8,
		TMP1,   8,
		TMP2,   8,
		NABT,   4,
		BCM,    4,
		CCBQ,   16,
		CBT,    16,
		Offset (0xE3),
		OST,    4,
		Offset (0xE4),
		Offset (0xE5),
		TPTE,   1,
		TBBN,   1,
		    ,   3,
		TP,     1,
		Offset (0xE6),
		SHK,    8,
		AUDS,   1,
		SPKR,   1,
		Offset (0xE8),
		HSEN,   4,
		HSST,   4,
		Offset (0xEA),
		    ,   2,
		WWP,    1,
		WLP,    1,
		Offset (0xEF),
		INCH,   2,
		IDIS,   2,
		INAC,   1
	}

	Method (ECRI, 0, Serialized)
	{
		OST = 0x7 /* FIXME: OS specific, in Linux is 7 */
		PWUP (0x07, 0xFF)
		Local0 = GBAP ()
		ITLB ()
		SBTN (Local0, 0x81)
	}

	Method (_REG, 2, NotSerialized)
	{
		ACPI = 1
		ECRI ()
	}

	/* mailbox: port 0x200 and 0x201 */
	OperationRegion (MAIO, SystemIO, 0x0200, 0x02)
	Field (MAIO, ByteAcc, NoLock, Preserve)
	{
		MAIN,   8,
		MADT,   8
	}
	IndexField (MAIN, MADT, ByteAcc, NoLock, Preserve)
	{
		Offset (0x8C),
		    ,   7,
		CLID,   1,
		Offset (0x95),
		PWM0,   8,
		Offset (0x9D),
		PWMC,   8
	}

	Mutex (ECMX, 0x00)

#include "ac.asl"
#include "battery.asl"
#include "lid.asl"

	/* The following are _Qxx methods in vendor DSDT whose function
	   is unknown. Just leave a debug message here. */

	Method (_Q04, 0, NotSerialized)
	{
		Store ("EC: _Q04", Debug)
		PNOT()
	}

	Method (_Q05, 0, NotSerialized)
	{
		Store ("EC: _Q05", Debug)
	}

	Method (_Q0B, 0, NotSerialized)
	{
		Store ("EC: _Q0B", Debug)
	}

	Method (_Q0C, 0, NotSerialized)
	{
		Store ("EC: _Q0C", Debug)
	}

	Method (_Q0D, 0, NotSerialized)
	{
		Store ("EC: _Q0D", Debug)
	}
}
