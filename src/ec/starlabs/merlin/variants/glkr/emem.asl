/* SPDX-License-Identifier: GPL-2.0-only */

OperationRegion (ECF2, EmbeddedControl, 0x00, 0x100)
Field (ECF2, ByteAcc, Lock, Preserve)
{
	Offset(0x00),
	ECMV, 8,	// Major Version Number
	ECSV, 8,	// Minor Version Number
	KBVS, 8,	// Keyboard Controller Version
	ECTV, 8,	// Test Version Number
	P0MV, 8,	// PD Port 0 Major Version
	P0SV, 8,	// PD Port 0 Minor Version
	P1MV, 8,	// PD Port 1 Major Version
	P1SV, 8,	// PD Port 1 Minor Version

	Offset(0x0c),
        ECT0, 8,        // EC Build Time 0
        ECT1, 8,        // EC Build Time 1
        ECT2, 8,        // EC Build Time 2
        ECT3, 8,        // EC Build Time 3

	Offset(0x12),
	KLTE, 8,	// Keyboard Backlight Timeout
	FCLA, 8,	// Fn Ctrl Reverse
	TPLE, 8,	// Trackpad State
	FLKE, 8,	// Function Lock State
	KLSE, 8,	// Keyboard Backlight State
	KLBE, 8,	// Keyboard Backlight Brightness

	Offset(0x20),
	RCMD, 8,	// Send EC command
	RCST, 8,	// Status of EC command

	Offset(0x60),
	TSE1, 8,	// Sensor 1 Temperature
	TSE2, 8,	// Sensor 2 Temperature
	TSE3, 8,	// Sensor 3 Temperature

	Offset(0x63),
	TSE4, 4,	// Sensor 4 Temperature
        SENF, 8,	// Sensor F
	TSHT, 8,	// Thermal Sensor High Trip Point
        TSLT, 8,	// Thermal Sensor Low Trip Point
        THER, 8,	// Thermal Source
	CHRA, 16,	// Charge Rate

	Offset(0x72),
	CHAR, 8,	// Charger Temperature

	Offset(0x7e),
	OSFG, 8,	// OS Flag [TODO: Confirm]
	LSTE, 1,	// Lid Status
	    , 7,	// Reserved

	Offset(0x80),
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
	BT1A, 8,	// Bt1 ASOC
	BT1T, 16,	// Bt1 Temperature
	BT1C, 8,	// Bt1 Control

	Offset(0x9d),
	OPWE, 8,	// OPM write to EC flag for UCSI

	Offset(0xa0),
	BSNL, 8,	// Battery Serial Number Low byte
	BSNH, 8,	// Battery Serial Number High Byte
	BMN1, 8,	// Battery Manufactory Name 1
	BMN2, 8,	// Battery Manufactory Name 2
	BMN3, 8,	// Battery Manufactory Name 3
	BMN4, 8,	// Battery Manufactory Name 4
	BMN5, 8,	// Battery Manufactory Name 5
	BMN6, 8,	// Battery Manufactory Name 6
	BMN7, 8,	// Battery Manufactory Name 7
	BMN8, 8,	// Battery Manufactory Name 8
	BMN9, 8,	// Battery Manufactory Name 9
	BMNA, 8,	// Battery Manufactory Name 10
	BMNB, 8,	// Battery Manufactory Name 11
	BMNC, 8,	// Battery Manufactory Name 12
	BDN1, 8,	// Battery Device Name 1
	BDN2, 8,	// Battery Device Name 2
	BDN3, 8,	// Battery Device Name 3
	BDN4, 8,	// Battery Device Name 4
	BDN5, 8,	// Battery Device Name 5
	BDN6, 8,	// Battery Device Name 6
	BDN7, 8,	// Battery Device Name 7
	BDN8, 8,	// Battery Device Name 8
	BDN9, 8,	// Battery Device Name 9
	BDNA, 8,	// Battery Device Name 10
	BDNB, 8,	// Battery Device Name 11
	BDNC, 8,	// Battery Device Name 12
	BCT1, 8,	// Battery Chemistry Type 1
	BCT2, 8,	// Battery Chemistry Type 2
	BCT3, 8,	// Battery Chemistry Type 3
	BCT4, 8,	// Battery Chemistry Type 4
	BCT5, 8,	// Battery Chemistry Type 5
	BCT6, 8,	// Battery Chemistry Type 6

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
}
