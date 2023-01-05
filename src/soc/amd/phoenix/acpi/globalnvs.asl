/* SPDX-License-Identifier: GPL-2.0-or-later */

/* TODO: Check if this is still correct */

/*
 * NOTE: The layout of the GNVS structure below must match the layout in
 * soc/amd/phoenix/include/soc/nvs.h !!!
 */

Field (GNVS, ByteAcc, NoLock, Preserve)
{
	/* Miscellaneous */
	,	8,	// 0x00 - Processor Count
	LIDS,	8,	// 0x01 - LID State
	,	8,	// 0x02 - AC Power State
	CBMC,	32,	// 0x03 - 0x06 - coreboot Memory Console
	PM1I,	64,	// 0x07 - 0x0e - System Wake Source - PM1 Index
	GPEI,	64,	// 0x0f - 0x16 - GPE Wake Source
	TMPS,	8,	// 0x17 - Temperature Sensor ID
	TCRT,	8,	// 0x18 - Critical Threshold
	TPSV,	8,	// 0x19 - Passive Threshold
}
