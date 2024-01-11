/* SPDX-License-Identifier: GPL-2.0-only */

OperationRegion (CMS2, SystemIO, 0x72, 0x2)
Field (CMS2, ByteAcc, NoLock, Preserve)
{
	IND2, 8,
	DAT2, 8,
}

IndexField (IND2, DAT2, ByteAcc, NoLock, Preserve)
{
	Offset (0x80),
	FLKC, 8,		// Function Lock State
	TPLC, 8,		// Trackpad State
	KLBC, 8,		// Keyboard Backlight Brightness
	KLSC, 8,		// Keyboard Backlight State
}
