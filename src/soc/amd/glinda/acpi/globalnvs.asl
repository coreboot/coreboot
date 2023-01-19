/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * NOTE: The layout of the GNVS structure below must match the layout in
 * soc/amd/glinda/include/soc/nvs.h !!!
 */

Field (GNVS, ByteAcc, NoLock, Preserve)
{
	/* Miscellaneous */
	LIDS,	8,	// 0x00 - LID State
	CBMC,	32,	// 0x01 - 0x04 - coreboot Memory Console
	PM1I,	64,	// 0x05 - 0x0c - System Wake Source - PM1 Index
	GPEI,	64,	// 0x0d - 0x14 - GPE Wake Source
}
