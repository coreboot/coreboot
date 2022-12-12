/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/ioapic.h>

Device (USB0)
{
	Name (_ADR, 0x001D0000)
	Name (_PRW, Package () { 0x03, 0x05 })

	OperationRegion (USBS, PCI_Config, 0x00, 0x0100)
	Field (USBS, ByteAcc, NoLock, Preserve)
	{
		Offset (0xC4),  URES,   8
	}
}

Device (USB1)
{
	Name (_ADR, 0x001D0001)
	Name (_PRW, Package () { 0x04, 0x05 })
	OperationRegion (USBS, PCI_Config, 0x00, 0x0100)
	Field (USBS, ByteAcc, NoLock, Preserve)
	{
		Offset (0xC4),  URES,   8
	}
}

Device (USB2)
{
	Name (_ADR, 0x001D0002)
	Name (_PRW, Package () { 0x0C, 0x05 })
	OperationRegion (USBS, PCI_Config, 0x00, 0x0100)
	Field (USBS, ByteAcc, NoLock, Preserve)
	{
		Offset (0xC4),  URES,   8
	}
}

Device (USB3)
{
	Name (_ADR, 0x001D0007)
	Name (_PRW, Package () { 0x0D, 0x05 })  /* PME_B0_STS any 0:1d or 0:1f device */
	OperationRegion (USBS, PCI_Config, 0x00, 0x0100)
	Field (USBS, ByteAcc, NoLock, Preserve)
	{
		Offset (0xC4),  URES,   8
	}
}

Device(PCI5)
{
	Name (_ADR, 0x001E0000)
	Name (_PRW, Package () { 0x0B, 0x05 })    /* PME# _STS */
	Name (_PRT, Package() {
		Package() { 0x0003ffff, 0, 0, 20 },
		Package() { 0x0003ffff, 1, 0, 21 },
		Package() { 0x0003ffff, 2, 0, 22 },
		Package() { 0x0003ffff, 3, 0, 23 },
	})
}

Device (ICH0)
{
	Name (_ADR, 0x001F0000)
	OperationRegion (D310, PCI_Config, 0x00, 0xFF)
	Field (D310, ByteAcc, NoLock, Preserve)
	{
		Offset (0x40),   PBAR,   16,
		Offset (0x58),   GBAR,   16,
	}

	OperationRegion (ACPI, SystemIO, 0x0400, 0xC0)
	Field (ACPI, ByteAcc, NoLock, Preserve)
	{
		PS1L,	8,
		PS1H,	8,
		PE1L,	8,
		PE1H,	8,

		Offset (0x28),
		GS0L,	8,
		GS0H,	8,
		GSPL,	8,
		GSPH,	8,
		GE0L,	8,
		GE0H,	8,
		GEPL,	8,
		GEPH,	8,

		Offset (0xB8),
		GPLV,	8
	}

	Name (MSBF, ResourceTemplate ()
	{
		/* IOAPIC 0  */
		Memory32Fixed (ReadWrite, IO_APIC_ADDR, 0x00001000,)

		IO (Decode16, 0x0, 0x0, 0x80, 0x0, PMIO)
		IO (Decode16, 0x0, 0x0, 0x40, 0x0, GPIO)

		/* 8254 legacy irq */
		IO (Decode16, 0x04D0, 0x04D0, 0x02, 0x02,)

		/* reset generator */
		IO (Decode16, 0x0092, 0x0092, 0x01, 0x01, )
	})

	Method (_CRS, 0, NotSerialized)
	{
		CreateWordField (MSBF, \_SB_.PCI0.ICH0.PMIO._MIN, IOA1)
		CreateWordField (MSBF, \_SB_.PCI0.ICH0.PMIO._MAX, IOA2)
		CreateByteField (MSBF, \_SB_.PCI0.ICH0.PMIO._LEN, IOAL)

		Local0 = PBAR
		If (Local0 && 1)
		{
			And (Local0, 0xFFFE, Local0)
			IOA1 = Local0
			IOA2 = Local0
			IOAL = 0x80
		} Else {
			IOAL = 0x00
		}

		CreateWordField (MSBF, \_SB_.PCI0.ICH0.GPIO._MIN, IOS1)
		CreateWordField (MSBF, \_SB_.PCI0.ICH0.GPIO._MAX, IOS2)
		CreateByteField (MSBF, \_SB_.PCI0.ICH0.GPIO._LEN, IOSL)

		Local0 = GBAR
		If (Local0 && 1) {
			And (Local0, 0xFFFE, Local0)
			IOS1 = Local0
			IOS2 = Local0
			IOSL = 0x40
		} Else {
			IOSL = 0x00
		}
		Return (MSBF)
	}

	Device (FWH)
	{
		Name (_HID, EisaId ("PNP0C02"))
		Name (_UID, 0x01)


		Name (MSBG, ResourceTemplate () {
			Memory32Fixed (ReadOnly, 0xFFF00000, 0x00080000,)
			Memory32Fixed (ReadOnly, 0xFFF80000, 0x00080000,)
	        })

	        Method (_CRS, 0, NotSerialized)
		{
			Return (MSBG)
		}
	}

	Device (SMSC)
	{
		Name (_HID, EisaId ("PNP0C02"))
		Name (_UID, 0x02)
		#include "acpi/superio.asl"
	}

}
