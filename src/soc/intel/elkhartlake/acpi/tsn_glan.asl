/* SPDX-License-Identifier: GPL-2.0-only */

/* Intel PCH TSN Ethernet Controller 0:1e.4 */

Device(GTSN) {
	Name(_ADR, 0x001E0004)
	OperationRegion(TSRT,PCI_Config,0x00,0x100)
	Field(TSRT,AnyAcc,NoLock,Preserve)
	{
		DVID, 16,
		Offset(0x10),
		TADL,  32,
		TADH,  32,
	}
}
