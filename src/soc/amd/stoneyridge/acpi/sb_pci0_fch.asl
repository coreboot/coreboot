/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/alib.h>

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
	MM1B = TOM1
	Local0 = 0x10000000 << 4
	Local0 -= TOM1
	MM1L = Local0

	Return (CRES) /* note to change the Name buffer */
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

	/* Send ALIB Function 1 the AC/DC state */
	Name(F1BF, Buffer(0x03){})
	CreateWordField(F1BF, 0, F1SZ)
	CreateByteField(F1BF, 2, F1DA)

	F1SZ = 3
	F1DA= \PWRS

	\_SB.ALIB(ALIB_FUNCTION_REPORT_AC_DC_STATE, F1BF)

} /* End Method(_SB._INI) */

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

Field(PCFG, DwordAcc, NoLock, Preserve)
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

	if (Arg1 == 0) {
		Switch(ToInteger(Arg0)) {
			Case(Package() {5, 15, 24}) {
				PG1A = 1
			}
			Case(Package() {6, 7, 8, 11, 12, 18}) {
				PG2_ = 1
			}
		}
		/* put device into D0 */
		Switch(ToInteger(Arg0))
		{
			Case(5) {
				I0TD = 0x00
				I0PD = 1
				Local0 = I0DS
				while(Local0 != 0x7) {
					Local0 = I0DS
				}
			}
			Case(6) {
				I1TD = 0x00
				I1PD = 1
				Local0 = I1DS
				while(Local0 != 0x7) {
					Local0 = I1DS
				}
			}
			Case(7) {
				I2TD = 0x00
				I2PD = 1
				Local0 = I2DS
				while(Local0 != 0x7) {
					Local0 = I2DS
				}
			}
			Case(8) {
				I3TD = 0x00
				I3PD = 1
				Local0 = I3DS
				while(Local0 != 0x7) {
					Local0 = I3DS
				}
			}
			Case(11) {
				U0TD = 0x00
				U0PD = 1
				Local0 = U0DS
				while(Local0 != 0x7) {
					Local0 = U0DS
				}
			}
			Case(12) {
				U1TD = 0x00
				U1PD = 1
				Local0 = U1DS
				while(Local0 != 0x7) {
					Local0 = U1DS
				}
			}
/* todo			Case(15) { STD0()} */ /* SATA */
			Case(18) { U2D0()} /* EHCI */
			Case(23) { U3D0()} /* XHCI */
			Case(24) { /* SD */
				SDTD = 0x00
				SDPD = 1
				Local0 = SDDS
				while(Local0 != 0x7) {
					Local0 = SDDS
				}
			}
		}
	} else {
		/* put device into D3cold */
		Switch(ToInteger(Arg0))
		{
			Case(5) {
				I0PD = 0
				Local0 = I0DS
				while(Local0 != 0x0) {
					Local0 = I0DS
				}
				I0TD = 0x03
			}
			Case(6) {
				I1PD = 0
				Local0 = I1DS
				while(Local0 != 0x0) {
					Local0 = I1DS
				}
				I1TD = 0x03
			}
			Case(7) {
				I2PD = 0
				Local0 = I2DS
				while(Local0 != 0x0) {
					Local0 = I2DS
				}
				I2TD = 0x03
			}
			Case(8) {
				I3PD = 0
				Local0 = I3DS
				while(Local0 != 0x0) {
					Local0 = I3DS
				}
				I3TD = 0x03
			}
			Case(11) {
				U0PD = 0
				Local0 = U0DS
				while(Local0 != 0x0) {
					Local0 = U0DS
				}
				U0TD = 0x03
			}
			Case(12) {
				U1PD = 0
				Local0 = U1DS
				while(Local0 != 0x0) {
					Local0 = U1DS
				}
				U1TD = 0x03
			}
/* todo			Case(15) { STD3()} */ /* SATA */
			Case(18) { U2D3()} /* EHCI */
			Case(23) { U3D3()} /* XHCI */
			Case(24) { /* SD */
				SDPD = 0
				Local0 = SDDS
				while(Local0 != 0x0) {
					Local0 = SDDS
				}
				SDTD = 0x03
			}
		}
		/* Turn off Power */
		if (I0TD == 3) {
			if (SATD == 3) {
				if (SDTD == 3) { PG1A = 0 }
			}
		}
		if (I1TD == 3) {
			if (I2TD == 3) {
				if (I3TD == 3) {
					if (U0TD == 3) {
						if (U1TD == 3) {
							if (U2TD == 3) {
								PG2_ = 0
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
	if (\XHCE == one) {
		if (U3TD != 0x03) {
			FDDC(23, 3)
		}
	}
	if (U2TD != 0x03) {
		FDDC(18, 3)
	}
}

Method(FWAK,0, Serialized)  /* FCH _WAK */
{
	if (\XHCE == one) {
		if (U3TD == 0x03) {
			FDDC(23, 0)
		}
	}
	if (U2TD == 0x03) {
		FDDC(18, 0)
	}
	if (\UT0E == zero) {
		if (U0TD != 0x03) {
			FDDC(11, 3)
		}
	}
	if (\UT1E == zero) {
		if (U1TD != 0x03) {
			FDDC(12, 3)
		}
	}
	if (\IC0E == zero) {
		if (I0TD != 0x03) {
			FDDC(5, 3)
		}
	}
	if (\IC1E == zero) {
		if (I1TD != 0x03) {
			FDDC(6, 3)
		}
	}
	if (\IC2E == zero) {
		if (I2TD != 0x03) {
			FDDC(7, 3)
		}
	}
	if (\IC3E == zero) {
		if (I3TD != 0x03) {
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
	Local0 = PGA3 & 0xdf  /* do SwUsb3SlpShutdown below */
	if (Arg1) {
		Local0 |= Arg0
	} else {
		Local1 = ~Arg0
		Local0 &= Local1
	}
	PGA3 = Local0
	if (Arg0 == 0x20) { /* if SwUsb3SlpShutdown */
		Local0 = PGA3
		Local0 &= Arg0
		while(!Local0) { /* wait SwUsb3SlpShutdown to complete */
			Local0 = PGA3
			Local0 &= Arg0
		}
	}
}
