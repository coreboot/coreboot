/* SPDX-License-Identifier: GPL-2.0-only */


/* AcpiGpe0Blk */
OperationRegion(GP0B, SystemMemory, 0xfed80814, 0x04)
	Field(GP0B, ByteAcc, NoLock, Preserve) { , 11, USBS, 1, }
