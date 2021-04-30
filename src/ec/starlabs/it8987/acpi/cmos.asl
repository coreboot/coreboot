/* SPDX-License-Identifier: GPL-2.0-only */

OperationRegion (CMOS, SystemIO, 0x70, 0x02)
Field (CMOS, ByteAcc, NoLock, Preserve)
{
	NVRI, 8,
	NVRD, 8
}

IndexField (NVRI, NVRD, ByteAcc, NoLock, Preserve)
{
	Offset (0x40),
	KBBL, 8,		// Keyboard backlight timeout
	FNSW, 8,		// Ctrl Fn Reverse (make keyboard Apple-like)

	Offset (0x7D),
	FNLC, 8			// Current state of Fn Lock key.
}
