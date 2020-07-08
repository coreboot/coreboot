/* SPDX-License-Identifier: GPL-2.0-only */

/* System Bus */
/*  _SB.PCI0 */

/* Operating System Capabilities Method */
Method(_OSC,4)
{
	/* Check for proper PCI/PCIe UUID */
	If(LEqual(Arg0,ToUUID("33DB4D5B-1FF7-401C-9657-7441C03DD766")))
	{
		/* Let OS control everything */
		Return (Arg3)
	} Else {
		CreateDWordField(Arg3,0,CDW1)
		Or(CDW1,4,CDW1)	// Unrecognized UUID
		Return(Arg3)
	}
}

Method(_BBN, 0) { /* Bus number = 0 */
	Return(0)
}
Method(_STA, 0) {
	/* DBGO("\\_SB\\PCI0\\_STA\n") */
	Return(0x0B)     /* Status is visible */
}

Method(_PRT,0) {
	If(PMOD){ Return(APR0) }   /* APIC mode */
	Return (PR0)                  /* PIC Mode */
} /* end _PRT */

/* Describe the Southbridge devices */

#include "pcie.asl"

/* PCI slot 1, 2, 3 */
Device(PIBR) {
	Name(_ADR, 0x00140004)
	Name(_PRW, Package() {0x18, 4})

	Method(_PRT, 0) {
		Return (PCIB)
	}
}

Device(STCR) {
	Name(_ADR, 0x00110000)
	#include "acpi/sata.asl"
} /* end STCR */

#include "usb.asl"

Device(SBUS) {
	Name(_ADR, 0x00140000)
} /* end SBUS */

#include "audio.asl"

#include "lpc.asl"

Device(HPBR) {
	Name(_ADR, 0x00140004)
} /* end HostPciBr */

Device(ACAD) {
	Name(_ADR, 0x00140005)
} /* end Ac97audio */

Device(ACMD) {
	Name(_ADR, 0x00140006)
} /* end Ac97modem */

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
		0x0D00,		/* range minimum */
		0xFFFF,		/* range maximum */
		0x0000,		/* translation */
		0xF300		/* length */
	)

	Memory32Fixed(READONLY, 0x000A0000, 0x00020000, VGAM)	/* VGA memory space */

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
	Store(TOM1, MM1B)
	ShiftLeft(0x10000000, 4, Local0)
	Subtract(Local0, TOM1, Local0)
	Store(Local0, MM1L)

	Return(CRES) /* note to change the Name buffer */
} /* end of Method(_SB.PCI0._CRS) */

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

	/* Determine the OS we're running on */
	OSFL()

	/* On older chips, clear PciExpWakeDisEn */
	/*if (LLessEqual(\SBRI, 0x13)) {
	*	Store(0,\PWDE)
	* }
	*/
} /* End Method(_SB._INI) */

Scope(\){
	/* Client Management index/data registers */
	OperationRegion(CMT, SystemIO, 0x00000C50, 0x00000002)
		Field(CMT, ByteAcc, NoLock, Preserve) {
		CMTI,      8,
		/* Client Management Data register */
		G64E,   1,
		G64O,      1,
		G32O,      2,
		,       2,
		GPSL,     2,
	}

	/* GPM Port register */
	OperationRegion(GPT, SystemIO, 0x00000C52, 0x00000001)
		Field(GPT, ByteAcc, NoLock, Preserve) {
		GPB0,1,
		GPB1,1,
		GPB2,1,
		GPB3,1,
		GPB4,1,
		GPB5,1,
		GPB6,1,
		GPB7,1,
	}

	/* Flash ROM program enable register */
	OperationRegion(FRE, SystemIO, 0x00000C6F, 0x00000001)
		Field(FRE, ByteAcc, NoLock, Preserve) {
		,     0x00000006,
		FLRE, 0x00000001,
	}

	/* PM2 index/data registers */
	OperationRegion(PM2R, SystemIO, 0x00000CD0, 0x00000002)
		Field(PM2R, ByteAcc, NoLock, Preserve) {
		PM2I, 0x00000008,
		PM2D, 0x00000008,
	}

	/* Power Management I/O registers, TODO:PMIO is quite different in SB800. */
	OperationRegion(PIOR, SystemIO, 0x00000CD6, 0x00000002)
		Field(PIOR, ByteAcc, NoLock, Preserve) {
		PIOI, 0x00000008,
		PIOD, 0x00000008,
	}
	IndexField (PIOI, PIOD, ByteAcc, NoLock, Preserve) {
		    , 1,	/* MiscControl */
		T1EE, 1,
		T2EE, 1,
		Offset(0x01),	/* MiscStatus */
		, 1,
		T1E, 1,
		T2E, 1,
		Offset(0x04),	/* SmiWakeUpEventEnable3 */
		, 7,
		SSEN, 1,
		Offset(0x07),	/* SmiWakeUpEventStatus3 */
		, 7,
		CSSM, 1,
		Offset(0x10),	/* AcpiEnable */
		, 6,
		PWDE, 1,
		Offset(0x1C),	/* ProgramIoEnable */
		, 3,
		MKME, 1,
		IO3E, 1,
		IO2E, 1,
		IO1E, 1,
		IO0E, 1,
		Offset(0x1D),	/* IOMonitorStatus */
		, 3,
		MKMS, 1,
		IO3S, 1,
		IO2S, 1,
		IO1S, 1,
		IO0S,1,
		Offset(0x20),	/* AcpiPmEvtBlk. TODO: should be 0x60 */
		APEB, 16,
		Offset(0x36),	/* GEvtLevelConfig */
		, 6,
		ELC6, 1,
		ELC7, 1,
		Offset(0x37),	/* GPMLevelConfig0 */
		, 3,
		PLC0, 1,
		PLC1, 1,
		PLC2, 1,
		PLC3, 1,
		PLC8, 1,
		Offset(0x38),	/* GPMLevelConfig1 */
		, 1,
		 PLC4, 1,
		 PLC5, 1,
		, 1,
		 PLC6, 1,
		 PLC7, 1,
		Offset(0x3B),	/* PMEStatus1 */
		GP0S, 1,
		GM4S, 1,
		GM5S, 1,
		APS, 1,
		GM6S, 1,
		GM7S, 1,
		GP2S, 1,
		STSS, 1,
		Offset(0x55),	/* SoftPciRst */
		SPRE, 1,
		, 1,
		, 1,
		PNAT, 1,
		PWMK, 1,
		PWNS, 1,

		/*	Offset(0x61), */	/*  Options_1 */
		/*		,7,  */
		/*		R617,1, */

		Offset(0x65),	/* UsbPMControl */
		, 4,
		URRE, 1,
		Offset(0x68),	/* MiscEnable68 */
		, 3,
		TMTE, 1,
		, 1,
		Offset(0x92),	/* GEVENTIN */
		, 7,
		E7IS, 1,
		Offset(0x96),	/* GPM98IN */
		G8IS, 1,
		G9IS, 1,
		Offset(0x9A),	/* EnhanceControl */
		,7,
		HPDE, 1,
		Offset(0xA8),	/* PIO7654Enable */
		IO4E, 1,
		IO5E, 1,
		IO6E, 1,
		IO7E, 1,
		Offset(0xA9),	/* PIO7654Status */
		IO4S, 1,
		IO5S, 1,
		IO6S, 1,
		IO7S, 1,
	}

	/* PM1 Event Block
	* First word is PM1_Status, Second word is PM1_Enable
	*/
	OperationRegion(P1EB, SystemIO, APEB, 0x04)
		Field(P1EB, ByteAcc, NoLock, Preserve) {
		TMST, 1,
		,    3,
		BMST,    1,
		GBST,   1,
		Offset(0x01),
		PBST, 1,
		, 1,
		RTST, 1,
		, 3,
		PWST, 1,
		SPWS, 1,
		Offset(0x02),
		TMEN, 1,
		, 4,
		GBEN, 1,
		Offset(0x03),
		PBEN, 1,
		, 1,
		RTEN, 1,
		, 3,
		PWDA, 1,
	}
}
