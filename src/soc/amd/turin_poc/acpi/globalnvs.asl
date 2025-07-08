/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * NOTE: The layout of the GNVS structure below must match the layout in
 * soc/amd/turin_poc/include/soc/nvs.h !!!
 */

Field (GNVS, ByteAcc, NoLock, Preserve)
{
	/* Miscellaneous */
	PM1I,	64,	// 0x00 - 0x07 - System Wake Source - PM1 Index
	GPEI,	64,	// 0x08 - 0x0f - GPE Wake Source
}
