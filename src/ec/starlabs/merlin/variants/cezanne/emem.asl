/* SPDX-License-Identifier: GPL-2.0-only */

OperationRegion (ECF2, EmbeddedControl, 0x00, 0x100)
Field (ECF2, ByteAcc, Lock, Preserve)
{
	Offset(0x00),
	ECMV, 8,	// Major Version Number
	ECSV, 8,	// Minor Version Number
	KBVS, 8,	// Keyboard Controller Version
	ECTV, 8,	// Test Version Number
	OSFG, 8,	// OS Flag
	FRMF, 8,	// Force Mirror Flag
	PDMV, 8,	// PD Major Version
	PDSV, 8,	// PD Minor Version

	Offset(0x0a),
	CPUT, 8,	// CPU Temperature

	Offset(0x13),
	AUDI, 8,	// Control Audio
	TPLE, 8,	// Trackpad State
	PSTC, 8,	// Current P State
	PSTL, 8,	// Last P State
	FNFT, 8,	// Fan Fault

	Offset(0x1a),
	FCLA, 8,	// Fn Ctrl Reverse

	Offset(0x20),
	WIFI, 8,	// WIFI Enable

	Offset(0x30),
	KLMT, 8,	// Keyboard Matrix
	BFCP, 8,	// Battery Full Charge Percentage
	FANM, 8,	// Fan Mode

	Offset(0x34),
	KLTE, 8,	// Keyboard Backlight Timeout
	KLSE, 8,	// Keyboard Backlight State
	KLBE, 8,	// Keyboard Backlight Brightness

	Offset(0x40),
	SHIP, 8,	// Shipping Mode Flag
	LIDF, 8,	// Lid FLag
	STCD, 8,	// Shutdown Code
	SSDT, 8,	// SSD Type
	S0I3, 8,	// S0I3 Mode
	CPUF, 8,	// CPU Family
	CPUM, 8,	// CPU Model
	CPUS, 8,	// CPU Stepping

	Offset(0x50),
	OEMM, 8,	// OEM Charger Mode
	OEMF, 8,	// OEM Charger Mode F

	Offset(0x70),
	FLKE, 8,	// Function Lock State

	Offset(0x74),
	P0MV, 8,	// PD Port 0 Major Version
	P0SV, 8,	// PD Port 0 Minor Version
	P1MV, 8,	// PD Port 1 Major Version
	P1SV, 8,	// PD Port 1 Minor Version
	USMA, 8,	// USCI Major Revision
	USMI, 8,	// USCI Minor Revision
	USRV, 8,	// USCI Revision Version

	Offset(0x7f),
	LSTE, 1,	// Lid Status
	    , 7,	// Reserved

	Offset (0x80),
	ECPS, 8,	// AC & Battery status
	B1MN, 8,	// Battery Model Number Code
	B1SN, 16,	// Battery Serial Number
	B1DC, 16,	// Battery Design Capacity
	B1DV, 16,	// Battery Design Voltage
	B1FC, 16,	// Battery Last Full Charge Capacity
	B1TP, 16,	// Battery Trip Point
	B1ST, 8,	// Battery State
	B1PR, 16,	// Battery Present Rate
	B1RC, 16,	// Battery Remaining Capacity
	B1PV, 16,	// Battery Present Voltage
	BPRP, 8,	// Battery Remaining percentage


	Offset(0xb0),
	UCSV, 16,	// UCSI DS Version
	UCSD, 16,	// UCSI DS Reserved
	CCI0, 8,	// UCSI DS CCI 0
	CCI1, 8,	// UCSI DS CCI 1
	CCI2, 8,	// UCSI DS CCI 2
	CCI3, 8,	// UCSI DS CCI 3
	CTL0, 8,	// UCSI DS Control 0
	CTL1, 8,	// UCSI DS Control 0
	CTL2, 8,	// UCSI DS Control 0
	CTL3, 8,	// UCSI DS Control 0
	CTL4, 8,	// UCSI DS Control 0
	CTL5, 8,	// UCSI DS Control 0
	CTL6, 8,	// UCSI DS Control 0
	CTL7, 8,	// UCSI DS Control 0

	Offset(0xc0),
	MGI0, 8,	// UCSI DS MGI 0
	MGI1, 8,	// UCSI DS MGI 1
	MGI2, 8,	// UCSI DS MGI 2
	MGI3, 8,	// UCSI DS MGI 3
	MGI4, 8,	// UCSI DS MGI 4
	MGI5, 8,	// UCSI DS MGI 5
	MGI6, 8,	// UCSI DS MGI 6
	MGI7, 8,	// UCSI DS MGI 7
	MGI8, 8,	// UCSI DS MGI 8
	MGI9, 8,	// UCSI DS MGI 9
	MGIA, 8,	// UCSI DS MGI A
	MGIB, 8,	// UCSI DS MGI B
	MGIC, 8,	// UCSI DS MGI C
	MGID, 8,	// UCSI DS MGI D
	MGIE, 8,	// UCSI DS MGI E
	MGIF, 8,	// UCSI DS MGI F

	Offset(0xd0),
	MGO0, 8,	// UCSI DS MGO 0
	MGO1, 8,	// UCSI DS MGO 1
	MGO2, 8,	// UCSI DS MGO 2
	MGO3, 8,	// UCSI DS MGO 3
	MGO4, 8,	// UCSI DS MGO 4
	MGO5, 8,	// UCSI DS MGO 5
	MGO6, 8,	// UCSI DS MGO 6
	MGO7, 8,	// UCSI DS MGO 7
	MGO8, 8,	// UCSI DS MGO 8
	MGO9, 8,	// UCSI DS MGO 9
	MGOA, 8,	// UCSI DS MGO A
	MGOB, 8,	// UCSI DS MGO B
	MGOC, 8,	// UCSI DS MGO C
	MGOD, 8,	// UCSI DS MGO D
	MGOE, 8,	// UCSI DS MGO E
	MGOF, 8,	// UCSI DS MGO F

	Offset(0xe6),
	ECWD, 16,	// EC Wakeup Delay
	ECWE, 8,	// EC Wakeup Enable

	Offset(0xf7),
	TBTC, 8,	// Thunderbolt Command
	TBTP, 8,	// Thunderbolt Data Port
	TBTD, 8,	// Thunderbolt Data
	TBTA, 8,	// Thunderbolt Acknowledge
	TBTG, 16,	// Thunderbolt DBG Data
}
