/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/ioapic.h>

External(\_SB.ALIB, MethodObj)

/* System Bus */
/*  _SB.PCI0 */

/* Operating System Capabilities Method */
Method(_OSC,4)
{
	/* Check for proper PCI/PCIe UUID */
	If (Arg0 == ToUUID("33DB4D5B-1FF7-401C-9657-7441C03DD766"))
	{
		/* Let OS control everything */
		Return (Arg3)
	} Else {
		CreateDWordField(Arg3,0,CDW1)
		CDW1 |= 4	// Unrecognized UUID
		Return (Arg3)
	}
}

/* 0:14.3 - LPC */
#include <soc/amd/common/acpi/lpc.asl>
#include <soc/amd/common/acpi/platform.asl>

Name(CRES, ResourceTemplate() {
	/* Set the Bus number and Secondary Bus number for the PCI0 device
	 * The Secondary bus range for PCI0 lets the system
	 * know what bus values are allowed on the downstream
	 * side of this PCI bus if there is a PCI-PCI bridge.
	 * PCI busses can have 256 secondary busses which
	 * range from [0-0xFF] but they do not need to be
	 * sequential.
	 */
	WordBusNumber (ResourceProducer, MinFixed, MaxFixed, PosDecode,
		0x0000,		/* address granularity */
		0x0000,		/* range minimum */
		0x00ff,		/* range maximum */
		0x0000,		/* translation */
		0x0100,		/* length */
		,, PSB0)		/* ResourceSourceIndex, ResourceSource, DescriptorName */

	IO(Decode16, 0x0cf8, 0x0cf8, 1,	8)

	WORDIO(ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
		0x0000,		/* address granularity */
		0x0000,		/* range minimum */
		0x0cf7,		/* range maximum */
		0x0000,		/* translation */
		0x0cf8		/* length */
	)

	WORDIO(ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
		0x0000,		/* address granularity */
		0x0d00,		/* range minimum */
		0xffff,		/* range maximum */
		0x0000,		/* translation */
		0xf300		/* length */
	)

	Memory32Fixed(READONLY, 0x000a0000, 0x00020000, VGAM)	/* VGA memory space */
	Memory32Fixed(READONLY, 0x000c0000, 0x00020000, EMM1)	/* Assume C0000-E0000 empty */

	/* memory space for PCI BARs below 4GB */
	Memory32Fixed(ReadOnly, 0x00000000, 0x00000000, MMIO)
}) /* End Name(_SB.PCI0.CRES) */

Method(_CRS, 0) {
	/* DBGO("\\_SB\\PCI0\\_CRS\n") */
	CreateDWordField(CRES, ^MMIO._BAS, MM1B)
	CreateDWordField(CRES, ^MMIO._LEN, MM1L)

	/* Declare memory between TOM1 and IOAPIC as available for PCI MMIO. */
	MM1B = TOM1
	Local0 = IO_APIC_ADDR /* This is the first MMIO device after TOM1. */
	Local0 -= TOM1
	MM1L = Local0

	CreateWordField(CRES, ^PSB0._MAX, BMAX)
	CreateWordField(CRES, ^PSB0._LEN, BLEN)
	BMAX = CONFIG_MMCONF_BUS_NUMBER - 1
	BLEN = CONFIG_MMCONF_BUS_NUMBER

	Return(CRES) /* note to change the Name buffer */
} /* end of Method(_SB.PCI0._CRS) */
