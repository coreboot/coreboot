/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/ioapic.h>

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
	 * PCI buses can have 256 secondary buses which
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

	/* VGA memory (0xa0000-0xbffff) */
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000a0000, 0x000bffff, 0x00000000,
			0x00020000)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadOnly,
			0x00000000, 0x000c0000, 0x000dffff, 0x00000000,
			0x00020000)

	/* memory space for PCI BARs below 4GB */
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			NonCacheable, ReadWrite,
			0x00000000, 0x00000000, 0x00000000, 0x00000000,
			0x00000000,,, PM01)
}) /* End Name(_SB.PCI0.CRES) */

Method(_CRS, 0) {
	/* Find PCI resource area in CRES */
	CreateDwordField (CRES, ^PM01._MIN, P1MN)
	CreateDwordField (CRES, ^PM01._MAX, P1MX)
	CreateDwordField (CRES, ^PM01._LEN, P1LN)

	/* Declare memory between TOM1 and MMCONF as available for PCI MMIO. */
	P1MN = TOM1
	P1MX = CONFIG_ECAM_MMCONF_BASE_ADDRESS - 1
	P1LN = P1MX - P1MN + 1

	CreateWordField(CRES, ^PSB0._MAX, BMAX)
	CreateWordField(CRES, ^PSB0._LEN, BLEN)
	BMAX = CONFIG_ECAM_MMCONF_BUS_NUMBER - 1
	BLEN = CONFIG_ECAM_MMCONF_BUS_NUMBER

	Return(CRES) /* note to change the Name buffer */
} /* end of Method(_SB.PCI0._CRS) */
