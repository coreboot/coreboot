/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 * Copyright (C) 2013 Sage Electronic Engineering, LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

External(\_SB.ALIB, MethodObj)

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

/* 0:14.2 - I2S Audio */

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
		0x03b0,		/* range minimum */
		0x03df,		/* range maximum */
		0x0000,		/* translation */
		0x0030		/* length */
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
Method(_INI, 0, Serialized) {
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

	/* Send ALIB Function 1 the AC/DC state */
	Name(F1BF, Buffer(0x03){})
	CreateWordField(F1BF, 0, F1SZ)
	CreateByteField(F1BF, 2, F1DA)

	Store(3, F1SZ)
	Store(\PWRS, F1DA)

	\_SB.ALIB(1, F1BF)

} /* End Method(_SB._INI) */

Method(OSFL, 0){

	if (LNotEqual(OSVR, Ones)) {Return(OSVR)}	/* OS version was already detected */

	if (CondRefOf(\_OSI))
	{
		Store(1, OSVR)					/* Assume some form of XP */
		if (\_OSI("Windows 2006"))		/* Vista */
		{
			Store(2, OSVR)
		}
	} else {
		If(WCMP(\_OS,"Linux")) {
			Store(3, OSVR)				/* Linux */
		} Else {
			Store(4, OSVR)				/* Gotta be WinCE */
		}
	}
	Return(OSVR)
}

OperationRegion(SMIC, SystemMemory, 0xfed80000, 0x80000)
Field( SMIC, ByteAcc, NoLock, Preserve) {
	offset (0x03ee),
	U3PS, 2,  /* Usb3PowerSel */

	offset (0x0e28),
	,29 ,
	SARP, 1,  /* Sata Ref Clock Powerdown */
	U2RP, 1,  /* Usb2 Ref Clock Powerdown */
	U3RP, 1,  /* Usb3 Ref Clock Powerdown */

	offset (0x1c00),
	, 1,
	,6,
	U3PY, 1,
	, 7,
	UD3P, 1,  /* bit 15 */
	U3PR, 1,  /* bit 16 */
	, 11,
	FWLM, 1,  /* FirmWare Load Mode  */
	FPLS, 1,  /* Fw PreLoad Start    */
	FPLC, 1,  /* Fw PreLoad Complete */

	offset (0x1c04),
	UA04, 16,
	, 15,
	ROAM, 1,  /* 1= ROM 0=RAM */

	offset (0x1c08),
	UA08, 32,

	offset (0x1e4a),
	I0TD, 2,
	, 1,
	I0PD, 1,
	offset (0x1e4b),
	I0DS, 3,

	offset (0x1e4c),
	I1TD, 2,
	, 1,
	I1PD, 1,
	offset (0x1e4d),
	I1DS, 3,

	offset (0x1e4e),
	I2TD, 2,
	, 1,
	I2PD, 1,
	offset (0x1e4f),
	I2DS, 3,

	offset (0x1e50),
	I3TD, 2,
	, 1,
	I3PD, 1,
	offset (0x1e51),
	I3DS, 3,

	offset (0x1e56),
	U0TD, 2,
	, 1,
	U0PD, 1,
	offset (0x1e57),
	U0DS, 3,

	offset (0x1e58),
	U1TD, 2,
	, 1,
	U1PD, 1,
	offset (0x1e59),
	U1DS, 3,

	offset (0x1e5e),
	SATD, 2,
	, 1,
	SAPD, 1,
	offset (0x1e5f),
	SADS, 3,

	offset (0x1e64),
	U2TD, 2,
	, 1,
	U2PD, 1,
	offset (0x1e65),
	U2DS, 3,

	offset (0x1e6e),
	U3TD, 2,
	, 1,
	U3PD, 1,
	offset (0x1e6f),
	U3DS, 3,

	offset (0x1e70),
	SDTD, 2,
	, 1,
	, 1,
	, 2,
	SDRT, 1,
	SDSC, 1,

	offset (0x1e71),
	SDDS, 3,

	offset (0x1e80),
	, 15,
	RQ15, 1,
	, 2,
	RQ18, 1,
	, 4,
	RQ23, 1,
	RQ24, 1,
	, 5,
	RQTY, 1,
	offset (0x1e84),
	, 15,
	SASR, 1,  /* SATA 15 Shadow Reg Request Status Register */
	, 2,
	U2SR, 1,  /* USB2 18 Shadow Reg Request Status Register */
	, 4,
	U3SR, 1,  /* USB3 23 Shadow Reg Request Status Register */
	SDSR, 1,  /* SD 24 Shadow Reg Request Status Register */
	offset (0x1e88),
	SRSA, 32, /* Shadow Reg SRAM Addr */
	SRSD, 32, /* Shadow Reg SRAM DATA */

	offset (0x1e94),
	SRDY, 1,  /* S0i3 bios ready */

	offset (0x1ea0),
	PG1A, 1,
	PG2_, 1,
	,1,
	U3PG, 1,  /* Usb3 Power Good BIT3 */

	offset (0x1ea3), /* Power Good Control */
	PGA3, 8 ,
}

OperationRegion(FCFG, SystemMemory, PCBA, 0x01000000)
Field(FCFG, DwordAcc, NoLock, Preserve)
{
	/* XHCI */
	Offset(0x00080010),
	XHBA, 32,
	Offset(0x0008002c),
	XH2C, 32,

	Offset(0x00080048),
	IDEX, 32,
	DATA, 32,
	Offset(0x00080054),
	U_PS, 2,

	/* SATA */
	Offset(0x00088010),
	ST10, 32,
	ST14, 32,
	ST18, 32,
	ST1C, 32,
	ST20, 32,
	ST24, 32,

	Offset(0x0008802c),
	ST2C, 32,

	Offset(0x00088040),
	ST40, 1,

	Offset(0x00088044),
	ST44, 1,

	Offset(0x0008804c),
	, 2,
	DDIC, 1, /* DisableDynamicInterfaceClockPowerSaving */

	Offset(0x00088064),
	S_PS, 2,

	Offset(0x00088084),
	, 1,
	ST84, 1,
	, 28,
	DSDN, 1, /* DShutDowN */

	Offset(0x0008808c),
	ST8C, 8,

	/* EHCI */
	Offset(0x00090004),
	, 1,
	EHME, 1,
	Offset(0x00090010),
	EHBA, 32,
	Offset(0x0009002c),
	EH2C, 32,
	Offset(0x00090054),
	EH54, 8,
	Offset(0x00090064),
	EH64, 8,

	Offset(0x000900c4),
	E_PS, 2,

	/* LPC Bridge */
	Offset(0x000a3078),
	, 2,
	LDQ0,  1,

	Offset(0x000a30cb),
	,  7,
	AUSS,  1, /* AutoSizeStart */

	/* SD */
	Offset(0x000a7004),
	,  1,
	SDME,  1,

	Offset(0x000a7010),
	SDBA,  32,
	Offset(0x000a702c),
	SD2C,  32,
	Offset(0x000a7094),
	D_PS,  2,
	,  6,
	SDPE, 1,
	,  6,
	PMES, 1,

	Offset(0x000a70b3), /* Version 2.0 = 0x1, Version 3.0 = 0x2 */
	SDB3,  8,
	Offset(0x000a70b4), /* Set Enable */
	,  8,
	SETE, 1,

	Offset(0x000a70d0),
	,  17,
	FC18, 1, /* Force 1.8v */

}
