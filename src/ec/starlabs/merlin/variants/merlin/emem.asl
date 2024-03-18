/* SPDX-License-Identifier: GPL-2.0-only */

OperationRegion (ECF2, EmbeddedControl, 0x00, 0x100)
Field (ECF2, ByteAcc, Lock, Preserve)
{
	Offset (0x00),
	ECMV, 8,	// Major Version Number
	ECSV, 8,	// Minor Version Number

	Offset (0x04),
	OSFG, 8,	// OS Flag
	FRMF, 8,	// Mirror Flag

	Offset(0x09),
	KLBE, 8,	// Keyboard Backlight Brightness
	KLSE, 8,	// Keyboard Backlight State

	Offset (0x0c),
	TPLE, 8,	// Trackpad State

	Offset (0x0f),
	FLKE, 8,	// Function Lock State
	KLTE, 8,	// Keyboard Backlight Timeout

	Offset(0x17),
	FCLA, 8,	// Fn Ctrl Reverse

	Offset(0x1a),
	BFCP, 8,	// Maximum Charge Level
	FANM, 8,	// Fan Mode

	Offset(0x40),
	SHIP, 8,	// Shipping Mode Flag

	Offset(0x7f),
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
	BPRP, 16,	// Battery Remaining percentage
}
