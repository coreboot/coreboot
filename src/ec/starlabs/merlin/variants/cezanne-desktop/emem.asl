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

	Offset(0x0c),
	P0MV, 8,	// PD Port 0 Major Version
	P0SV, 8,	// PD Port 0 Minor Version
	P1MV, 8,	// PD Port 1 Major Version
	P1SV, 8,	// PD Port 1 Minor Version

	Offset(0x13),
	AUDI, 8,	// Control Audio
	TRAC, 8,	// Trackpad Control

	Offset(0x18),
	BSEC, 8,	// Save to CMOS
	KLSE, 8,	// Keyboard Backlight State
	TPLE, 8,	// Trackpad State
	FLKE, 8,	// Function Lock State
	FCLS, 8,	// Ctrl Fn Reverse (Make Keyboard Apple-like)
	MXCH, 8,	// Max Charge Level
	FANM, 8,	// Fan Mode

	Offset(0x40),
	SHIP, 8,	// Shipping Mode Flag

	Offset(0x46),
	ECPS, 8,	// AC & Battery Status

	Offset(0x30),
	STEF, 8,	// Sensor T Error F

	Offset(0x62),
	SSKT, 8,	// System Skin Temperature
	SENF, 8,	// Sensor F
	TSHT, 8,	// Thermal Sensor High Trip Point
	TSLT, 8,	// Thermal Sensor Low Trip Point
	THER, 8,	// Thermal Source

	Offset(0x70),
	CPUT, 8,	// PECI CPU Temperature
	PMXT, 8,	// PLMX Temperature
	CHAR, 8,	// Charger Temperature

	Offset(0x7f),
	LSTE, 8,	// Lid Status
	ECPS, 8,	// AC & Battery Status
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
	BPRP, 8,	// Battery Remaining Percentage
	CPUT, 8,	// PECI CPU Temperature
	STCD, 8,	// Shutdown Code
	B1HL, 8,	// Battery Health
	CWFU, 8,	// CW2015 Full
	B1CC, 16,	// Battery Cycle Count

	Offset(0xb0),
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

	Offset(0xc0),
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

	Offset(0xd0),
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

	Offset(0xe0),
	CCS1, 8,	// Cross Point Switch Status 1
	CCS2, 8,	// Cross Point Switch Status 2
	TCI1, 8,	// TC Input 1 / TCHC Thermal Charge CMD [TODO, Confirm]
	TCI2, 8,	// TC Input 2 / TCHF Thermal Charge Flag [TODO, Confirm]
	PDDT, 8,	// PD Det [TODO, Confirm]
	PDBD, 8,	// B PD Det [TODO, Confirm]
	ECWD, 16,	// EC Wakeup Delay
	ECWE, 8,	// EC Wakeup Enable
	PDV1, 8,	// PD Vol [TODO, Confirm]
	PDV2, 8,	// B PD Vol [TODO, Confirm]

	// Below are the Thunderbolt Offsets from the shared EC code. There aren't
	// use for AMD boards but left for reference.
	//
	// Offset(0xf7),
	// TBTC, 8,	// Thunderbolt Command
	// TBTP, 8,	// Thunderbolt Data Port
	// TBTD, 8,	// Thunderbolt Data
	// TBTA, 8,	// Thunderbolt Acknowledge
	// TBTG, 16,	// Thunderbolt DBG Data
}
