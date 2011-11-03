/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Kyösti Mälkki <kyosti.malkki@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */


/* SuperIO GPIO configuration via logical device 0x0A */

Name (MSBF, ResourceTemplate ()
{
	IO (Decode16, 0x0000, 0x0000, 0x01, 0x80, _Y1B)
})

OperationRegion (LPC0, SystemIO, 0x0E00, 0x60)
Field (LPC0, ByteAcc, NoLock, Preserve)
{
	PME0,   8,
	Offset (0x02),	PME2,8,
	Offset (0x04),	PME4,8,
	Offset (0x0A),	PMEA,8,
	Offset (0x23),
		GC10,8, GC11,8, GC12,8, GC13,8, GC14,8, GC15,8, GC16,8, GC17,8,
		GC20,8, GC21,8, GC22,8, GC23,8, GC24,8, GC25,8, GC26,8, GC27,8,
		GC30,8, GC31,8, GC32,8, GC33,8, GC34,8, GC35,8, GC36,8, GC37,8,
		GC40,8, GC41,8, GC42,8, GC43,8,

	Offset (0x3F),
		GC50,8, GC51,8, GC52,8, GC53,8, GC54,8, GC55,8, GC56,8, GC57,8,
		GC60,8, GC61,8,

	Offset (0x4B),
		GP_1,8, GP_2,8, GP_3,8, GP_4,8, GP_5,8, GP_6,8,
	Offset (0x56),	FAN1,8,
	Offset (0x5D),	LED1,8, LED2,8,
}

OperationRegion (SMC1, SystemIO, 0x2E, 0x02)
Field (SMC1, ByteAcc, NoLock, Preserve)
{
	INDX,   8,	DATA,   8
}

IndexField (INDX, DATA, ByteAcc, NoLock, Preserve)
{
	Offset (0x07),	LDN,    8,
	Offset (0x22),	PWRC,   8,
	Offset (0x30),	ACTR,   8,
	Offset (0x60),
		IOAH,   8,	IOAL,   8,
		IOBH,   8,	IOBL,   8,

	Offset (0x70),	INTR,   8,
	Offset (0x72),	INT1,   8,
	Offset (0x74),	DMCH,   8,
	Offset (0xB2),	SPS1,   8,	SPS2,   8,
	Offset (0xB8),	D2TS,   8,
	Offset (0xF0),	OPT1,   8,	OPT2,   8,	OPT3,   8,
	Offset (0xF4),	WDTC,   8,
	Offset (0xF6),	GP01,   8,	GP02,   8,	GP04,   8
}

Method (ECFG, 0, NotSerialized)
{
	Store (0x55, INDX)
}
Method (XCFG, 0, NotSerialized)
{
	Store (0xAA, INDX)
}

Method (_CRS, 0, NotSerialized)
{
	CreateWordField (MSBF, \_SB.PCI0.ICH0.SMSC._Y1B._MIN, IOM1)
	CreateWordField (MSBF, \_SB.PCI0.ICH0.SMSC._Y1B._MAX, IOM2)
	CreateByteField (MSBF, \_SB.PCI0.ICH0.SMSC._Y1B._LEN, IOML)

	ECFG ()
	Store (0x0A, \_SB.PCI0.ICH0.SMSC.LDN)
	Store (0x00, IOM1)
	Store (0x00, IOM2)
	Or (\_SB.PCI0.ICH0.SMSC.IOAH, IOM1, IOM1)
	ShiftLeft (IOM1, 0x08, IOM1)
	Or (\_SB.PCI0.ICH0.SMSC.IOAL, IOM1, IOM1)
	Store (IOM1, IOM2)
	If (LNotEqual (IOM1, 0x00))
	{
		Store (0x80, IOML)
	}
	XCFG ()

	Return (MSBF)
}


Method (_INI, 0, NotSerialized)
{
	/* GPIO configuration */
	Store (0x00, GC10)
	Store (0x81, GC11)
	Store (0x00, GC17)
	Store (0x0c, GC21)
	Store (0x00, GC22)
	Store (0x04, GC27)
	Store (0x04, GC30)
	Store (0x01, GC31)
	Store (0x01, GC32)
	Store (0x01, GC33)
	Store (0x01, GC34) /* GPI password jumper */
	Store (0x01, GC35) /* GPI scsi enable jumper */
#if 1
	Store (0x01, GC42)  /* GPI */
#else
	Store (0x84, GC42)  /* nIO_PME */
#endif
	Store (0x86, GC60) /* led 1 */
	Store (0x81, GC61) /* led 2 ?? */

	/* GPIO initial output levels */
	Store (GP_1, Local0)
	And( Local0, 0x7C, Local0)
	Or ( Local0, 0x81, Local0)
	Store (Local0, GP_1)

	Store (GP_2, Local0)
	And( Local0, 0xFE, Local0)
	Or ( Local0, 0x00, Local0)
	Store (Local0, GP_2)

	Store (GP_3, Local0)
	And( Local0, 0x7F, Local0)
	Or ( Local0, 0x80, Local0)
	Store (Local0, GP_3)

	Store (GP_4, Local0)
	And( Local0, 0x7F, Local0)
	Or ( Local0, 0x00, Local0)
	Store (Local0, GP_4)

	/* Power Led */
	Store (LED1, Local0)
	And( Local0, 0xfc, Local0)
	Or ( Local0, 0x01, Local0)
	Store (Local0, LED1)

}

Method (MLED, 1, NotSerialized)
{
	If (LEqual (Arg0, 0x00))
	{
		Store (0x00, LED1)
	}

	If (LOr (LEqual (Arg0, 0x01), LEqual (Arg0, 0x02)))
	{
		Store (0x01, LED1)
	}

	If (LEqual (Arg0, 0x03))
	{
		Store (0x02, LED1)
	}

	If (LOr (LEqual (Arg0, 0x04), LEqual (Arg0, 0x05)))
	{
		Store (0x03, LED1)
	}
}

