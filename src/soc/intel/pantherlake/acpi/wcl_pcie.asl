/* SPDX-License-Identifier: GPL-2.0-only */

Device (RP01)
{
	Name (_ADR, 0x001C0000)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP02)
{
	Name (_ADR, 0x001C0001)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP03)
{
	Name (_ADR, 0x001C0002)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP04)
{
	Name (_ADR, 0x001C0003)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP05)
{
	Name (_ADR, 0x00060000)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP06)
{
	Name (_ADR, 0x00060001)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}
