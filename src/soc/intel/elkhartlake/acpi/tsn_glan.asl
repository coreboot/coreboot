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

/* Intel PSE TSN Ethernet Controller #1 0:1d.1 */

Device(OTN0) {
	Name(_ADR, 0x001D0001)
	OperationRegion(TSRT,PCI_Config,0x00,0x100)
	Field(TSRT,AnyAcc,NoLock,Preserve)
	{
		DVID, 16,
		Offset(0x10),
		TADL,  32,
		TADH,  32,
	}
}

/* Intel PSE TSN Ethernet Controller #2 0:1d.2 */

Device(OTN1) {
	Name(_ADR, 0x001D0002)
	OperationRegion(TSRT,PCI_Config,0x00,0x100)
	Field(TSRT,AnyAcc,NoLock,Preserve)
	{
		DVID, 16,
		Offset(0x10),
		TADL,  32,
		TADH,  32,
	}
}
