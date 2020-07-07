/* SPDX-License-Identifier: GPL-2.0-only */

/* PCI-X devices 04:04.0 and 04:04.1 : AIC-7902W
 * U320 SCSI dual-channel controller
 */

Device (SCS0)
{
	Name (_ADR, 0x00040000)
	OperationRegion (SCSC, PCI_Config, 0x00, 0x0100)
	Field (SCSC, ByteAcc, NoLock, Preserve)
	{
		Offset (0x2C),   SID,   32,
		Offset (0xE0),   PMC,   8,
		Offset (0xFF),   IDW,   8
	}
}

Device (SCS1)
{
	Name (_ADR, 0x00040001)
	OperationRegion (SCSC, PCI_Config, 0x00, 0x0100)
	Field (SCSC, ByteAcc, NoLock, Preserve)
	{
		Offset (0x2C),   SID,   32,
		Offset (0xE0),   PMC,   8,
		Offset (0xFF),   IDW,   8
	}
}
