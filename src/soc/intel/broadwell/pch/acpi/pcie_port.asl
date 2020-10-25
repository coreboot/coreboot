/* SPDX-License-Identifier: GPL-2.0-only */

/* Included in each PCIe Root Port device */

OperationRegion (RPCS, PCI_Config, 0x00, 0xFF)
Field (RPCS, AnyAcc, NoLock, Preserve)
{
	Offset (0x4c),	// Link Capabilities
	, 24,
	RPPN, 8,	// Root Port Number
	Offset (0x5A),
	, 3,
	PDC, 1,
	Offset (0xDF),
	, 6,
	HPCS, 1,
}
