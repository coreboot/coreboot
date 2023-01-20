/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * NOTE: The layout of the GNVS structure below must match the layout in
 * soc/amd/phoenix/include/soc/nvs.h !!!
 */

Field (GNVS, ByteAcc, NoLock, Preserve)
{
	/* Miscellaneous */
	CBMC,	32,	// 0x00 - 0x03 - coreboot Memory Console
	PM1I,	64,	// 0x04 - 0x0b - System Wake Source - PM1 Index
	GPEI,	64,	// 0x0c - 0x13 - GPE Wake Source
}
