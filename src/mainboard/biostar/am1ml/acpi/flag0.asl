/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

OperationRegion (GRAM, SystemMemory, 0x0400, 0x0100)

Field (GRAM, ByteAcc, Lock, Preserve)
{
	Offset (0x10),
	FLG0,   8
}
