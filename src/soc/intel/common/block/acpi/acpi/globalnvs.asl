/* SPDX-License-Identifier: GPL-2.0-only */

/* Global Variables */

Field (GNVS, ByteAcc, NoLock, Preserve)
{
	/* Miscellaneous */
	,	16,	// 0x00 - Operating System
	SMIF,	8,	// 0x02 - SMI function
	,	8,	// 0x03 - Processor Count
	PPCM,	8,	// 0x04 - Max PPC State
	TLVL,	8,	// 0x05 - Throttle Level Limit
	LIDS,	8,	// 0x06 - LID State
	,	8,	// 0x07 - AC Power State
	CBMC,	32,	// 0x08 - 0x0b AC Power State
	PM1I,	64,	// 0x0c - 0x13 PM1 wake status bit
	GPEI,	64,	// 0x14 - 0x17 GPE wake status bit
	DPTE,	8,	// 0x1c - Enable DPTF
	NHLA,	64,	// 0x1d - 0x24 NHLT Address
	NHLL,	32,	// 0x25 - 0x28 NHLT Length
	,	16,	// 0x29 - 0x2a Wifi Country Identifier
	U2WE,	16,	// 0x2b - 0x2c USB2 Wake Enable Bitmap
	U3WE,	16,	// 0x2d - 0x2e USB3 Wake Enable Bitmap
	UIOR,	8,	// 0x2f - UART debug controller init on S3 resume
	,	64,	// 0x30 - 0x37 Hest log buffer (used in SMM, not ASL code)
}
