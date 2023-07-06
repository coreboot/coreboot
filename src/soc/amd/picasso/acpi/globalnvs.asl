/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * NOTE: The layout of the GNVS structure below must match the layout in
 * soc/amd/picasso/include/soc/nvs.h !!!
 *
 */

Field (GNVS, ByteAcc, NoLock, Preserve)
{
	/* Miscellaneous */
	PM1I,	64,	// 0x00 - 0x07 - System Wake Source - PM1 Index
	GPEI,	64,	// 0x08 - 0x0f - GPE Wake Source
	TMPS,	8,	// 0x10 - Temperature Sensor ID
	TCRT,	8,	// 0x11 - Critical Threshold
	TPSV,	8,	// 0x12 - Passive Threshold
}
