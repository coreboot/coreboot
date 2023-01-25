/* SPDX-License-Identifier: GPL-2.0-or-later */

OperationRegion (TMEM, PCI_Config, 0x00, 0x0100)
Field (TMEM, ByteAcc, NoLock, Preserve)
{
	Offset (0x40),
	    ,   4,
	BSEG,   4,
	PAMS,   48,
	Offset (0x52),
	DIM0,   4,
	DIM1,   4,
	Offset (0x54),
	DIM2,   4
}

Name (MTBL, Package (0x10)
{
	0x00,
	0x20,
	0x20,
	0x30,
	0x40,
	0x40,
	0x60,
	0x80,
	0x80,
	0x80,
	0x80,
	0xC0,
	0x0100,
	0x0100,
	0x0100,
	0x0200
})
Name (ERNG, Package (0x0D)
{
	0x000C0000,
	0x000C4000,
	0x000C8000,
	0x000CC000,
	0x000D0000,
	0x000D4000,
	0x000D8000,
	0x000DC000,
	0x000E0000,
	0x000E4000,
	0x000E8000,
	0x000EC000,
	0x000F0000
})
Name (PAMB, Buffer (0x07){})

Device (APIC)
{
	Name (_HID, EisaId ("PNP0003") /* IO-APIC Interrupt Controller */)
	Name (_CRS, ResourceTemplate ()
	{
		Memory32Fixed (ReadOnly, 0xFEC00000, 0x00100000)
	})
}

Device (LPCB)
{
	Name (_ADR, 0x001F0000)
	Name (_DDN, "LPC Bus Device")

	Device (DMAC)
	{
		Name (_HID, EisaId ("PNP0200") /* PC-class DMA Controller */)
		Name (_CRS, ResourceTemplate ()
		{
			IO (Decode16,
				0x0000,             // Range Minimum
				0x0000,             // Range Maximum
				0x00,               // Alignment
				0x10,               // Length
				)
			IO (Decode16,
				0x0081,             // Range Minimum
				0x0081,             // Range Maximum
				0x00,               // Alignment
				0x03,               // Length
				)
			IO (Decode16,
				0x0087,             // Range Minimum
				0x0087,             // Range Maximum
				0x00,               // Alignment
				0x01,               // Length
				)
			IO (Decode16,
				0x0089,             // Range Minimum
				0x0089,             // Range Maximum
				0x00,               // Alignment
				0x03,               // Length
				)
			IO (Decode16,
				0x008F,             // Range Minimum
				0x008F,             // Range Maximum
				0x00,               // Alignment
				0x01,               // Length
				)
			IO (Decode16,
				0x00C0,             // Range Minimum
				0x00C0,             // Range Maximum
				0x00,               // Alignment
				0x20,               // Length
				)
			DMA (Compatibility, NotBusMaster, Transfer8, )
				{4}
		})
	}

	Device (RTC)
	{
		Name (_HID, EisaId ("PNP0B00") /* AT Real-Time Clock */)
		Name (_CRS, ResourceTemplate ()
		{
			IO (Decode16,
				0x0070,             // Range Minimum
				0x0070,             // Range Maximum
				0x01,               // Alignment
				0x02,               // Length
				)
			IO (Decode16,
				0x0072,             // Range Minimum
				0x0072,             // Range Maximum
				0x01,               // Alignment
				0x02,               // Length
				)
			IO (Decode16,
				0x0074,             // Range Minimum
				0x0074,             // Range Maximum
				0x01,               // Alignment
				0x04,               // Length
				)
			IRQNoFlags ()
				{8}
		})
	}

	Device (PIC)
	{
		Name (_HID, EisaId ("PNP0000") /* 8259-compatible Programmable Interrupt Controller */)
		Name (_CRS, ResourceTemplate ()
		{
			IO (Decode16,
				0x0020,             // Range Minimum
				0x0020,             // Range Maximum
				0x01,               // Alignment
				0x1E,               // Length
				)
			IO (Decode16,
				0x00A0,             // Range Minimum
				0x00A0,             // Range Maximum
				0x01,               // Alignment
				0x1E,               // Length
				)
			IO (Decode16,
				0x04D0,             // Range Minimum
				0x04D0,             // Range Maximum
				0x01,               // Alignment
				0x02,               // Length
				)
		})
	}

	Device (FPU)
	{
		Name (_HID, EisaId ("PNP0C04") /* x87-compatible Floating Point Processing Unit */)
		Name (_CRS, ResourceTemplate ()
		{
			IO (Decode16,
				0x00F0,             // Range Minimum
				0x00F0,             // Range Maximum
				0x01,               // Alignment
				0x01,               // Length
				)
			IRQNoFlags ()
				{13}
		})
	}

	Device (TMR)
	{
		Name (_HID, EisaId ("PNP0100"))
		Name (_CRS, ResourceTemplate ()
		{
			IO (Decode16,
				0x0040,             // Range Minimum
				0x0040,             // Range Maximum
				0x01,               // Alignment
				0x04,               // Length
				)
			IO (Decode16,
				0x0050,             // Range Minimum
				0x0050,             // Range Maximum
				0x01,               // Alignment
				0x04,               // Length
				)
			IRQNoFlags ()
				{0}
		})
	}

	Device (SPKR)
	{
		Name (_HID, EisaId ("PNP0800"))
		Name (_CRS, ResourceTemplate ()
		{
			IO (Decode16,
				0x0061,             // Range Minimum
				0x0061,             // Range Maximum
				0x01,               // Alignment
				0x01,               // Length
				)
		})
	}

	Device (XTRA)
	{
		Name (_HID, EisaId ("PNP0C02"))
		Name (_CRS, ResourceTemplate ()
		{
			IO (Decode16,
				0x0500,             // Range Minimum
				0x0500,             // Range Maximum
				0x01,               // Alignment
				0xFF,               // Length
				)
			IO (Decode16,
				0x0400,             // Range Minimum
				0x0400,             // Range Maximum
				0x01,               // Alignment
				0x20,               // Length
				)
			IO (Decode16,
				0x0010,             // Range Minimum
				0x0010,             // Range Maximum
				0x01,               // Alignment
				0x10,               // Length
				)
			IO (Decode16,
				0x0080,             // Range Minimum
				0x0080,             // Range Maximum
				0x01,               // Alignment
				0x01,               // Length
				)
			IO (Decode16,
				0x0084,             // Range Minimum
				0x0084,             // Range Maximum
				0x01,               // Alignment
				0x03,               // Length
				)
			IO (Decode16,
				0x0088,             // Range Minimum
				0x0088,             // Range Maximum
				0x01,               // Alignment
				0x01,               // Length
				)
			IO (Decode16,
				0x008C,             // Range Minimum
				0x008C,             // Range Maximum
				0x01,               // Alignment
				0x03,               // Length
				)
			IO (Decode16,
				0x0090,             // Range Minimum
				0x0090,             // Range Maximum
				0x01,               // Alignment
				0x10,               // Length
				)
			IO (Decode16,
				0x0600,             // Range Minimum
				0x0600,             // Range Maximum
				0x01,               // Alignment
				0x20,               // Length
				)
			IO (Decode16,
				0x0CA0,             // Range Minimum
				0x0CA0,             // Range Maximum
				0x01,               // Alignment
				0x02,               // Length
				)
			IO (Decode16,
				0x0CA4,             // Range Minimum
				0x0CA4,             // Range Maximum
				0x01,               // Alignment
				0x03,               // Length
				)
			Memory32Fixed (ReadOnly,
				0xFF000000,         // Address Base
				0x01000000,         // Address Length
				)
		})
	}

	Device (HPET)
	{
		Name (_HID, EisaId ("PNP0103") /* HPET System Timer */)
		Method (_STA, 0, NotSerialized)  // _STA: Status
		{
			Return (0x0F)
		}

		Name (CRS0, ResourceTemplate ()
		{
			Memory32Fixed (ReadWrite,
			0xFED00000,         // Address Base
			0x00000400,         // Address Length
			)
		})
		Name (CRS1, ResourceTemplate ()
		{
			Memory32Fixed (ReadWrite,
			0xFED01000,         // Address Base
			0x00000400,         // Address Length
			)
		})
		Name (CRS2, ResourceTemplate ()
		{
			Memory32Fixed (ReadWrite,
			0xFED02000,         // Address Base
			0x00000400,         // Address Length
			)
		})
		Name (CRS3, ResourceTemplate ()
		{
			Memory32Fixed (ReadWrite,
			0xFED03000,         // Address Base
			0x00000400,         // Address Length
			)
		})
		Method (_CRS, 0, Serialized)
		{
			Return (CRS0) /* \_SB_.PC00.HPET.CRS0 */
		}
    }
}

Device (HDAS)
{
	Name (_ADR, 0x001F0003)
	OperationRegion (HDAR, PCI_Config, 0x00, 0x0100)
	Field (HDAR, WordAcc, NoLock, Preserve)
	{
		VDID,   32
	}

	Name (_S0W, 0x03)  // _S0W: S0 Device Wake State
	Method (_DSW, 3, NotSerialized)  // _DSW: Device Sleep Wake
	{
	}
}

Scope (\_GPE)
{
	OperationRegion (PMIO, SystemIO, ACPI_BASE_ADDRESS, 0xFF)
	Field (PMIO, ByteAcc, NoLock, Preserve) {
		Offset(0x34),   /* 0x34, SMI/SCI STS*/
			,  9,
		SGCS, 1,        /* SWGPE STS BIT */

		Offset(0x40),   /* 0x40, SMI/SCI_EN*/
			,  17,
		SGPC, 1,        /* SWGPE CTRL BIT */

		Offset(0x6C),   /* 0x6C, General Purpose Event 0 Status [127:96] */
			,  2,
		SGPS,  1,       /* SWGPE STATUS */

		Offset(0x7C),   /* 0x7C, General Purpose Event 0 Enable [127:96] */
			,  2,
		SGPE,  1        /* SWGPE ENABLE */
	}
	Device (RAS)
	{
		Name (_HID, EisaId ("PNP0C33"))
		Name (_UID, 0)
		Name (_DDN, "RAS Error Device Controller")
		Printf ("Initialized RAS Device PNP0C33")
	}
	Method(_L62, 0) {
		Printf ("SWGPE Method _L62")
		SGPC = 0 // clear SWGPE enable
		SGPS = 1 // clear SWGPE Status
		Notify(RAS, 0x80)
	}

}

#include "pch_rp.asl"
