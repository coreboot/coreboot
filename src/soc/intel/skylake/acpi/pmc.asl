/* SPDX-License-Identifier: GPL-2.0-only */

Device (PMC)
{
	Name (_ADR, 0x001f0002)
	Name (_DDN, "Power Management Controller")

	OperationRegion (PMCP, PCI_Config, 0x00, 0x100)
	Field (PMCP, AnyAcc, NoLock, Preserve)
	{
		Offset (0x48),
		, 12,
		PWRM, 20,	/* PWRMBASE */
	}

	OperationRegion (PMCM, SystemMemory, PWRM << 12, 0x3f)
	Field (PMCM, DWordAcc, NoLock, Preserve)
	{
		Offset (0x1c),	/* PCH_PM_STS */
		, 24,
		PMFS, 1,	/* PMC_MSG_FULL_STS */
		Offset (0x20),
		MPMC, 32,	/* MTPMC */
		Offset (0x24),	/* PCH_PM_STS2 */
		, 20,
		UWAB, 1,	/* USB2 Workaround Available */
	}
}
