/* SPDX-License-Identifier: GPL-2.0-only */

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

/* 0:14.0 - SMBUS */
Device(SBUS) {
	Name(_ADR, 0x00140000)
} /* end SBUS */

#include "usb.asl"

/* 0:14.2 - I2S Audio */

/* 0:14.3 - LPC */
#include <soc/amd/common/acpi/lpc.asl>

/* 0:14.7 - SD Controller */
Device(SDCN) {
	Name(_ADR, 0x00140007)

	Method(_PS0) {
		FDDC(24, 0)
	}
	Method(_PS3) {
		FDDC(24, 3)
	}
	Method(_PSC) {
		Return(SDTD)
	}
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
	/* MISC registers */
	offset (0x03ee),
	U3PS, 2,  /* Usb3PowerSel */

	offset (0x0e28),
	,29 ,
	SARP, 1,  /* Sata Ref Clock Powerdown */
	U2RP, 1,  /* Usb2 Ref Clock Powerdown */
	U3RP, 1,  /* Usb3 Ref Clock Powerdown */

	/* XHCI_PM registers */
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

	/* AOAC Registers */
	offset (0x1e4a), /* I2C0 D3 Control */
	I0TD, 2,
	, 1,
	I0PD, 1,
	offset (0x1e4b), /* I2C0 D3 State */
	I0DS, 3,

	offset (0x1e4c), /* I2C1 D3 Control */
	I1TD, 2,
	, 1,
	I1PD, 1,
	offset (0x1e4d), /* I2C1 D3 State */
	I1DS, 3,

	offset (0x1e4e), /* I2C2 D3 Control */
	I2TD, 2,
	, 1,
	I2PD, 1,
	offset (0x1e4f), /* I2C2 D3 State */
	I2DS, 3,

	offset (0x1e50), /* I2C3 D3 Control */
	I3TD, 2,
	, 1,
	I3PD, 1,
	offset (0x1e51), /* I2C3 D3 State */
	I3DS, 3,

	offset (0x1e56), /* UART0 D3 Control */
	U0TD, 2,
	, 1,
	U0PD, 1,
	offset (0x1e57), /* UART0 D3 State */
	U0DS, 3,

	offset (0x1e58), /* UART1 D3 Control */
	U1TD, 2,
	, 1,
	U1PD, 1,
	offset (0x1e59), /* UART1 D3 State */
	U1DS, 3,

	offset (0x1e5e), /* SATA D3 Control */
	SATD, 2,
	, 1,
	SAPD, 1,
	offset (0x1e5f), /* SATA D3 State */
	SADS, 3,

	offset (0x1e64), /* USB2 D3 Control */
	U2TD, 2,
	, 1,
	U2PD, 1,
	offset (0x1e65), /* USB2 D3 State */
	U2DS, 3,

	offset (0x1e6e), /* USB3 D3 Control */
	U3TD, 2,
	, 1,
	U3PD, 1,
	offset (0x1e6f), /* USB3 D3 State */
	U3DS, 3,

	offset (0x1e70), /* SD D3 Control */
	SDTD, 2,
	, 1,
	SDPD, 1,
	, 1,
	, 1,
	SDRT, 1,
	SDSC, 1,

	offset (0x1e71), /* SD D3 State */
	SDDS, 3,

	offset (0x1e80), /* Shadow Register Request */
	, 15,
	RQ15, 1,
	, 2,
	RQ18, 1,
	, 4,
	RQ23, 1,
	RQ24, 1,
	, 5,
	RQTY, 1,
	offset (0x1e84), /* Shadow Register Status */
	, 15,
	SASR, 1,  /* SATA 15 Shadow Reg Request Status Register */
	, 2,
	U2SR, 1,  /* USB2 18 Shadow Reg Request Status Register */
	, 4,
	U3SR, 1,  /* USB3 23 Shadow Reg Request Status Register */
	SDSR, 1,  /* SD 24 Shadow Reg Request Status Register */

	offset (0x1ea0), /* PwrGood Control */
	PG1A, 1,
	PG2_, 1,
	,1,
	U3PG, 1,  /* Usb3 Power Good BIT3 */

	offset (0x1ea3), /* PwrGood Control b[31:24] */
	PGA3, 8 ,
}

OperationRegion(FCFG, SystemMemory, PCBA, 0x01000000)
Field(FCFG, DwordAcc, NoLock, Preserve)
{
	/* XHCI */
	Offset(0x00080010), /* Base address */
	XHBA, 32,
	Offset(0x0008002c), /* Subsystem ID / Vendor ID */
	XH2C, 32,

	Offset(0x00080048), /* Indirect PCI Index Register */
	IDEX, 32,
	DATA, 32,
	Offset(0x00080054), /* PME Control / Status */
	U_PS, 2,

	/* EHCI */
	Offset(0x00090004), /* Control */
	, 1,
	EHME, 1,
	Offset(0x00090010), /* Base address */
	EHBA, 32,
	Offset(0x0009002c), /* Subsystem ID / Vendor ID */
	EH2C, 32,
	Offset(0x00090054), /* EHCI Spare 1 */
	EH54, 8,
	Offset(0x00090064), /* Misc Control 2 */
	EH64, 8,

	Offset(0x000900c4), /* PME Control / Status */
	E_PS, 2,

	/* LPC Bridge */
	Offset(0x000a30cb), /* ClientRomProtect[31:24] */
	,  7,
	AUSS,  1, /* AutoSizeStart */
}

/*
 * Arg0:device:
 *  5=I2C0, 6=I2C1, 7=I2C2, 8=I2C3, 11=UART0, 12=UART1,
 *  15=SATA, 18=EHCI, 23=xHCI, 24=SD
 * Arg1:D-state
 */
Mutex (FDAS, 0) /* FCH Device AOAC Semophore */
Method(FDDC, 2, Serialized)
{
	Acquire(FDAS, 0xffff)

	if(LEqual(Arg1, 0)) {
		Switch(ToInteger(Arg0)) {
			Case(Package() {5, 15, 24}) {
				Store(One, PG1A)
			}
			Case(Package() {6, 7, 8, 11, 12, 18}) {
				Store(One, PG2_)
			}
		}
		/* put device into D0 */
		Switch(ToInteger(Arg0))
		{
			Case(5) {
				Store(0x00, I0TD)
				Store(One, I0PD)
				Store(I0DS, Local0)
				while(LNotEqual(Local0,0x7)) {
					Store(I0DS, Local0)
				}
			}
			Case(6) {
				Store(0x00, I1TD)
				Store(One, I1PD)
				Store(I1DS, Local0)
				while(LNotEqual(Local0,0x7)) {
					Store(I1DS, Local0)
				}
			}
			Case(7) {
				Store(0x00, I2TD)
				Store(One, I2PD)
				Store(I2DS, Local0)
				while(LNotEqual(Local0,0x7)) {
					Store(I2DS, Local0)
				}
			}
			Case(8) {Store(0x00, I3TD)
				Store(One, I3PD)
				Store(I3DS, Local0)
				while(LNotEqual(Local0,0x7)) {
					Store(I3DS, Local0)
				}
			}
			Case(11) {
				Store(0x00, U0TD)
				Store(One, U0PD)
				Store(U0DS, Local0)
				while(LNotEqual(Local0,0x7)) {
					Store(U0DS, Local0)
				}
			}
			Case(12) {
				Store(0x00, U1TD)
				Store(One, U1PD)
				Store(U1DS, Local0)
				while(LNotEqual(Local0,0x7)) {
					Store(U1DS, Local0)
				}
			}
/* todo			Case(15) { STD0()} */ /* SATA */
			Case(18) { U2D0()} /* EHCI */
			Case(23) { U3D0()} /* XHCI */
			Case(24) { /* SD */
				Store(0x00, SDTD)
				Store(One, SDPD)
				Store(SDDS, Local0)
				while(LNotEqual(Local0,0x7)) {
					Store(SDDS, Local0)
				}
			}
		}
	} else {
		/* put device into D3cold */
		Switch(ToInteger(Arg0))
		{
			Case(5) {
				Store(Zero, I0PD)
				Store(I0DS, Local0)
				while(LNotEqual(Local0,0x0)) {
					Store(I0DS, Local0)
				}
				Store(0x03, I0TD)
			}
			Case(6) {
				Store(Zero, I1PD)
				Store(I1DS, Local0)
				while(LNotEqual(Local0,0x0)) {
					Store(I1DS, Local0)
				}
				Store(0x03, I1TD)
			}
			Case(7)  {
				Store(Zero, I2PD)
				Store(I2DS, Local0)
				while(LNotEqual(Local0,0x0)) {
					Store(I2DS, Local0)
				}
				Store(0x03, I2TD)}
			Case(8) {
				Store(Zero, I3PD)
				Store(I3DS, Local0)
				while(LNotEqual(Local0,0x0)) {
					Store(I3DS, Local0)
				}
				Store(0x03, I3TD)
			}
			Case(11) {
				Store(Zero, U0PD)
				Store(U0DS, Local0)
				while(LNotEqual(Local0,0x0)) {
					Store(U0DS, Local0)
				}
				Store(0x03, U0TD)
			}
			Case(12) {
				Store(Zero, U1PD)
				Store(U1DS, Local0)
				while(LNotEqual(Local0,0x0)) {
					Store(U1DS, Local0)
				}
				Store(0x03, U1TD)
			}
/* todo			Case(15) { STD3()} */ /* SATA */
			Case(18) { U2D3()} /* EHCI */
			Case(23) { U3D3()} /* XHCI */
			Case(24) { /* SD */
				Store(Zero, SDPD)
				Store(SDDS, Local0)
				while(LNotEqual(Local0,0x0)) {
					Store(SDDS, Local0)
				}
				Store(0x03, SDTD)
			}
		}
		/* Turn off Power */
		if(LEqual(I0TD, 3)) {
			if(LEqual(SATD, 3)) {
				if(LEqual(SDTD, 3)) { Store(Zero, PG1A) }
			}
		}
		if(LEqual(I1TD, 3)) {
			if(LEqual(I2TD, 3)) {
				if(LEqual(I3TD, 3)) {
					if(LEqual(U0TD, 3)) {
						if(LEqual(U1TD, 3)) {
							if(LEqual(U2TD, 3)) {
								Store(Zero, PG2_)
							}
						}
					}
				}
			}
		}
	}
	Release(FDAS)
}

Method(FPTS,0, Serialized)  /* FCH _PTS */
{
	if(LEqual(\XHCE, one)) {
		if(LNotEqual(U3TD, 0x03)) {
			FDDC(23, 3)
		}
	}
	if(LNotEqual(U2TD, 0x03)) {
		FDDC(18, 3)
	}
}

Method(FWAK,0, Serialized)  /* FCH _WAK */
{
	if(LEqual(\XHCE, one)) {
		if(LEqual(U3TD, 0x03)) {
			FDDC(23, 0)
		}
	}
	if(LEqual(U2TD, 0x03)) {
		FDDC(18, 0)
	}
	if(LEqual(\UT0E, zero)) {
		if(LNotEqual(U0TD, 0x03)) {
			FDDC(11, 3)
		}
	}
	if(LEqual(\UT1E, zero)) {
		if(LNotEqual(U1TD, 0x03)) {
			FDDC(12, 3)
		}
	}
	if(LEqual(\IC0E, zero)) {
		if(LNotEqual(I0TD, 0x03)) {
			FDDC(5, 3)
		}
	}
	if(LEqual(\IC1E, zero)) {
		if(LNotEqual(I1TD, 0x03)) {
			FDDC(6, 3)
		}
	}
	if(LEqual(\IC2E, zero)) {
		if(LNotEqual(I2TD, 0x03)) {
			FDDC(7, 3)
		}
	}
	if(LEqual(\IC3E, zero)) {
		if(LNotEqual(I3TD, 0x03)) {
			FDDC(8, 3)
		}
	}
}

/*
 * Helper for setting a bit in AOACxA0 PwrGood Control
 * Arg0: bit to set or clear
 * Arg1: 0 = clear bit[Arg0], non-zero = set bit[Arg0]
 */
Method(PWGC,2, Serialized)
{
	And (PGA3, 0xdf, Local0)  /* do SwUsb3SlpShutdown below */
	if(Arg1) {
		Or(Arg0, Local0, Local0)
	} else {
		Not(Arg0, Local1)
		And(Local1, Local0, Local0)
	}
	Store(Local0, PGA3)
	if(LEqual(Arg0, 0x20)) { /* if SwUsb3SlpShutdown */
		Store(PGA3, Local0)
		And(Arg0, Local0, Local0)
		while(LNot(Local0)) { /* wait SwUsb3SlpShutdown to complete */
			Store(PGA3, Local0)
			And(Arg0, Local0, Local0)
		}
	}
}
