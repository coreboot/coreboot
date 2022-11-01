/* SPDX-License-Identifier: GPL-2.0-only */

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

/* Describe the Southbridge devices */

/* 0:11.0 - SATA */
Device(STCR) {
	Name(_ADR, 0x00110000)
} /* end STCR */

/* 0:14.0 - SMBUS */
Device(SBUS) {
	Name(_ADR, 0x00140000)
} /* end SBUS */

#include "usb.asl"

/* 0:14.2 - HD Audio */
#if !CONFIG(SOUTHBRIDGE_AMD_PI_KERN)
#include "audio.asl"
#endif

/* 0:14.3 - LPC */
#include "lpc.asl"

/* 0:14.7 - SD Controller */
Device(SDCN) {
	Name(_ADR, 0x00140007)
} /* end SDCN */

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
		0x00FF,		/* range maximum */
		0x0000,		/* translation */
		0x0100,		/* length */
		,, PSB0)		/* ResourceSourceIndex, ResourceSource, DescriptorName */

	IO(Decode16, 0x0CF8, 0x0CF8, 1,	8)

	WORDIO(ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
		0x0000,		/* address granularity */
		0x0000,		/* range minimum */
		0x0CF7,		/* range maximum */
		0x0000,		/* translation */
		0x0CF8		/* length */
	)
	WORDIO(ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
		0x0000,		/* address granularity */
		0x03B0,		/* range minimum */
		0x03DF,		/* range maximum */
		0x0000,		/* translation */
		0x0030		/* length */
	)

	WORDIO(ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
		0x0000,		/* address granularity */
		0x0D00,		/* range minimum */
		0xFFFF,		/* range maximum */
		0x0000,		/* translation */
		0xF300		/* length */
	)

	Memory32Fixed(READONLY, 0x000A0000, 0x00020000, VGAM)	/* VGA memory space */
	Memory32Fixed(READONLY, 0x000C0000, 0x00020000, EMM1)	/* Assume C0000-E0000 empty */

	/* memory space for PCI BARs below 4GB */
	Memory32Fixed(ReadOnly, 0x00000000, 0x00000000, MMIO)
}) /* End Name(_SB.PCI0.CRES) */

Method(_CRS, 0) {
	/* DBGO("\\_SB\\PCI0\\_CRS\n") */
	CreateDWordField(CRES, ^MMIO._BAS, MM1B)
	CreateDWordField(CRES, ^MMIO._LEN, MM1L)

	/*
	 * Declare memory between TOM1 and 4GB as available
	 * for PCI MMIO.
	 * Use ShiftLeft to avoid 64bit constant (for XP).
	 * This will work even if the OS does 32bit arithmetic, as
	 * 32bit (0x00000000 - TOM1) will wrap and give the same
	 * result as 64bit (0x100000000 - TOM1).
	 */
	MM1B = TOM1
	Local0 = 0x10000000 << 4
	Local0 -= TOM1
	MM1L = Local0

	Return (CRES) /* note to change the Name buffer */
} /* end of Method(_SB.PCI0._CRS) */

#if CONFIG(HUDSON_IMC_FWM)
	/* TODO: It is unstable.
	 * might be fixed by restructuring
	 */
	#include "acpi/AmdImc.asl" /* Hudson IMC function */
#endif

/*
 *
 *               FIRST METHOD CALLED UPON BOOT
 *
 *  1. If debugging, print current OS and ACPI interpreter.
 *  2. Get PCI Interrupt routing from ACPI VSM, this
 *     value is based on user choice in BIOS setup.
 */
Method(_INI, 0) {
	/* DBGO("\\_SB\\_INI\n") */
	/* DBGO("   DSDT.ASL code from ") */
	/* DBGO(__DATE__) */
	/* DBGO(" ") */
	/* DBGO(__TIME__) */
	/* DBGO("\n   Sleep states supported: ") */
	/* DBGO("\n") */
	/* DBGO("   \\_OS=") */
	/* DBGO(\_OS) */
	/* DBGO("\n   \\_REV=") */
	/* DBGO(\_REV) */
	/* DBGO("\n") */

} /* End Method(_SB._INI) */
