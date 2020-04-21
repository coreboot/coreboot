/* SPDX-License-Identifier: GPL-2.0-only */

#include "southbridge/intel/i82371eb/i82371eb.h"

/* Declares assorted devices that fall under this southbridge. */
Device (PX40)
{
	Name(_ADR, 0x00040000)
	OperationRegion (PIRQ, PCI_Config, 0x60, 0x04)
	Field (PIRQ, ByteAcc, NoLock, Preserve)
	{
		PIRA,   8,
		PIRB,   8,
		PIRC,   8,
		PIRD,   8
	}

	OperationRegion (S1XX, PCI_Config, 0xB2, 0x01)
	Field (S1XX, ByteAcc, NoLock, Preserve)
	{
		FXS1,   8
	}

	/* PNP Motherboard Resources */
	Device (SYSR)
	{
		Name (_HID, EisaId ("PNP0C02"))
		Name (_UID, 0x02)
		Method (_CRS, 0, NotSerialized)
		{
			Name (BUF1, ResourceTemplate ()
			{
				/* PM register ports */
				IO (Decode16, 0x0000, 0x0000, 0x01, 0x40, _Y06)
				/* SMBus register ports */
				IO (Decode16, 0x0000, 0x0000, 0x01, 0x10, _Y07)
				/* PIIX4E ports */
				/* Aliased DMA ports */
				IO (Decode16, 0x0010, 0x0010, 0x01, 0x10, )
				/* Aliased PIC ports */
				IO (Decode16, 0x0022, 0x0022, 0x01, 0x1E, )
				/* Aliased timer ports */
				IO (Decode16, 0x0050, 0x0050, 0x01, 0x04, )
				IO (Decode16, 0x0062, 0x0062, 0x01, 0x02, )
				IO (Decode16, 0x0065, 0x0065, 0x01, 0x0B, )
				IO (Decode16, 0x0074, 0x0074, 0x01, 0x0C, )
				IO (Decode16, 0x0091, 0x0091, 0x01, 0x03, )
				IO (Decode16, 0x00A2, 0x00A2, 0x01, 0x1E, )
				IO (Decode16, 0x00E0, 0x00E0, 0x01, 0x10, )
				IO (Decode16, 0x0294, 0x0294, 0x01, 0x04, )
				IO (Decode16, 0x03F0, 0x03F0, 0x01, 0x02, )
				IO (Decode16, 0x04D0, 0x04D0, 0x01, 0x02, )
			})
			CreateWordField (BUF1, _Y06._MIN, PMLO)
			CreateWordField (BUF1, _Y06._MAX, PMRL)
			CreateWordField (BUF1, _Y07._MIN, SBLO)
			CreateWordField (BUF1, _Y07._MAX, SBRL)
			And (\_SB.PCI0.PX43.PM00, 0xFFFE, PMLO)
			And (\_SB.PCI0.PX43.SB00, 0xFFFE, SBLO)
			Store (PMLO, PMRL)
			Store (SBLO, SBRL)
			Return (BUF1)
		}
	}
	/* 8259-compatible Programmable Interrupt Controller */
	Device (PIC)
	{
		Name (_HID, EisaId ("PNP0000"))
		Name (_CRS, ResourceTemplate ()
		{
			IO (Decode16, 0x0020, 0x0020, 0x01, 0x02,)
			IO (Decode16, 0x00A0, 0x00A0, 0x01, 0x02,)
			IRQNoFlags () {2}
		})
	}

	/* PC-class DMA Controller */
	Device (DMA1)
	{
		Name (_HID, EisaId ("PNP0200"))
		Name (_CRS, ResourceTemplate ()
		{
			DMA (Compatibility, BusMaster, Transfer8,) {4}
			IO (Decode16, 0x0000, 0x0000, 0x01, 0x10,)
			IO (Decode16, 0x0080, 0x0080, 0x01, 0x11,)
			IO (Decode16, 0x0094, 0x0094, 0x01, 0x0C,)
			IO (Decode16, 0x00C0, 0x00C0, 0x01, 0x20,)
		})
	}

	/* PC-class System Timer */
	Device (TMR)
	{
		Name (_HID, EisaId ("PNP0100"))
		Name (_CRS, ResourceTemplate ()
		{
			IO (Decode16,0x0040,0x0040,0x01,0x04,)
			IRQNoFlags () {0}
		})
	}

	/* AT Real-Time Clock */
	Device (RTC)
	{
		Name (_HID, EisaId ("PNP0B00"))
		Name (_CRS, ResourceTemplate ()
		{
			IO (Decode16,0x0070,0x0070,0x01,0x04,)
			IRQNoFlags () {8}
		})
	}

	Device (SPKR)
	{
		Name (_HID, EisaId ("PNP0800"))
		Name (_CRS, ResourceTemplate ()
		{
			IO (Decode16,0x0061,0x0061,0x01,0x01,)
		})
	}

	/* x87-compatible Floating Point Processing Unit */
	Device (COPR)
	{
		Name (_HID, EisaId ("PNP0C04"))
		Name (_CRS, ResourceTemplate ()
		{
			IO (Decode16,0x00F0,0x00F0,0x01,0x10,)
			IRQNoFlags () {13}
		})
	}
}
Device (PX43)
{
	Name (_ADR, 0x00040003)  // _ADR: Address

	Method (_CRS, 0, NotSerialized)
	{
		Name (BUF1, ResourceTemplate ()
		{
			/* PM register ports */
			IO (Decode16, PM_IO_BASE, PM_IO_BASE, 0x01, 0x40, )
			/* SMBus register ports */
			IO (Decode16, SMBUS_IO_BASE, SMBUS_IO_BASE, 0x01, 0x10, )
		})
		Return (BUF1)
	}

	OperationRegion (IPMU, PCI_Config, PMBA, 0x02)
	Field (IPMU, ByteAcc, NoLock, Preserve)
	{
	    PM00,   16
	}

	OperationRegion (ISMB, PCI_Config, SMBBA, 0x02)
	Field (ISMB, ByteAcc, NoLock, Preserve)
	{
	    SB00,   16
	}
}
