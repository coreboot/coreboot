/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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

	OperationRegion (PMCM, SystemMemory, ShiftLeft (PWRM, 12), 0x3f)
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
