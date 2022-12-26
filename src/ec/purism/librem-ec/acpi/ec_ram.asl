/* SPDX-License-Identifier: GPL-2.0-only */

OperationRegion (ERAM, EmbeddedControl, 0, 0xFF)
Field (ERAM, ByteAcc, Lock, Preserve)
{
	Offset (0x03),
	LSTE, 1,	// Lid is open
	, 1,
	LWKE, 1,	// Lid wake
	, 5,
	Offset (0x07),
	TMP1, 8,	// CPU temperature
	Offset (0x10),
	ADP, 1,		// AC adapter connected
	, 1,
	BAT0, 1,	// Battery connected
	, 5,
	WFNO, 8,	// Wake cause (not implemented)
	Offset (0x16),
	BDC0, 32,	// Battery design capacity
	BFC0, 32,	// Battery full capacity
	Offset (0x22),
	BDV0, 32,	// Battery design voltage
	BST0, 32,	// Battery status
	BPR0, 32,	// Battery current
	BRC0, 32,	// Battery remaining capacity
	BPV0, 32,	// Battery voltage
	Offset (0x3A),
	BCW0, 32,
	BCL0, 32,
	Offset (0x68),
	ECOS, 8,	// Detected OS, 0 = no ACPI, 1 = ACPI but no driver, 2 = ACPI with driver
	Offset (0xC8),
	OEM1, 8,
	OEM2, 8,
	OEM3, 16,
	OEM4, 8,	// Extra SCI data
	Offset (0xCD),
	TMP2, 8,	// GPU temperature
	DUT1, 8,	// Fan 1 duty
	DUT2, 8,	// Fan 2 duty
	RPM1, 16,	// Fan 1 RPM
	RPM2, 16,	// Fan 2 RPM
	Offset (0xD9),
	WLED, 8,	// WiFi LED
	NOTR, 8,	// notification LED R
	NOTG, 8,	// notification LED G
	NOTB, 8,	// notification LED B
	WINF, 8,	// Enable ACPI brightness controls
	Offset (0xF8),
	FCMD, 8,
	FDAT, 8,
	FBUF, 8,
	FBF1, 8,
	FBF2, 8,
	FBF3, 8,
}
