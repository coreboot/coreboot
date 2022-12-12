/* SPDX-License-Identifier: GPL-2.0-only */

Device (MBRS)
{
	Name (_HID, EisaId ("PNP0C01"))
	Name (_UID, 0x01)
	Name (MSBF, ResourceTemplate ()
	{
		/* System memory */
		QWordMemory (ResourceProducer, PosDecode, MinFixed,
			MaxNotFixed, Prefetchable, ReadWrite,
			0x0, 0x100000000, 0x400000000, 0x0, 0x0, ,, _Y1C,
			AddressRangeMemory, TypeStatic)

		/* Top Of Low Memory */
		Memory32 (ReadOnly, 0x0, 0x0, 0x1, 0x0, _Y1D)

		/* 640kB who wants more? */
		Memory32Fixed (ReadWrite, 0x0, 0xA0000, )

		/* 64k BIOS bootblock */
		Memory32Fixed (ReadOnly, 0xF0000, 0x10000,)

		/* ISA memory hole 15-16 MB ? */
		/* Memory32Fixed (ReadOnly, 0x100000, 0xF00000,) */
		/* ISA memory hole 14-15 MB ? */
		/* Memory32Fixed (ReadOnly, 0x100000, 0xE00000,) */

		/* Local APIC */
		Memory32Fixed (ReadWrite, 0xFEE00000, 0x00001000,)
	})

	Method (_CRS, 0, NotSerialized)
	{
		CreateQWordField (MSBF, \_SB.MBRS._Y1C._MIN, MEML)
		CreateQWordField (MSBF, \_SB.MBRS._Y1C._MAX, MEMM)
		CreateQWordField (MSBF, \_SB.MBRS._Y1C._LEN, LELM)

		And (\_SB.PCI0.RLAR, 0x03FF, Local1)
		Local1++
		If (Local1 > 0x40)
		{
			ShiftLeft (Local1, 0x1A, LELM)
		}


		CreateDWordField (MSBF, \_SB.MBRS._Y1D._MIN, MS00)
		CreateDWordField (MSBF, \_SB.MBRS._Y1D._MAX, MS01)
		CreateDWordField (MSBF, \_SB.MBRS._Y1D._LEN, MEM2)
		And (\_SB.PCI0.TOLM, 0xF800, Local1)
		ShiftRight (Local1, 0x04, Local1)
		Local1--
		If (Local1 > 0x10)
		{
			Local1 -= 0x0F
			MEM2 = ShiftLeft (Local1, 0x14)
			MS00 = 0x01000000
			MS01 = MS00
		}

		Return (MSBF)
	}

	Method (_STA, 0, NotSerialized)
	{
		Return (0x0F)
	}
}
