/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

/* Intel Storage Controllers */

Device (EMMC)
{
	Name (_ADR, 0x001E0004)
	Name (_DDN, "eMMC Controller")

	OperationRegion (EMCR, PCI_Config, 0x00, 0x100)
	Field (EMCR, DWordAcc, NoLock, Preserve)
	{
		Offset (0x84),	/* PMECTRLSTATUS */
		D0D3, 2,	/* POWERSTATE */
		Offset (0xa2),	/* PG_CONFIG */
		, 2,
		PGEN, 1,	/* PG_ENABLE */
	}

	Method (_PS0, 0, Serialized)
	{
		/* Disable Power Gate */
		Store (0, ^PGEN)

		/* Clear bits 31, 6, 2, 0 */
		^^PCRA (PID_SCS, 0x600, 0x7FFFFFBA)
		Sleep (2)

		/* Set bits 31, 6, 2, 0 */
		^^PCRO (PID_SCS, 0x600, 0x80000045)

		/* Set Power State to D0 */
		Store (Zero, Local0)
		Store (Local0, ^D0D3)
		Store (^D0D3, Local0)
	}

	Method (_PS3, 0, Serialized)
	{
		/* Enable Power Gate */
		Store (1, ^PGEN)

		/* Set Power State to D0 */
		Store (3, Local0)
		Store (Local0, ^D0D3)
		Store (^D0D3, Local0)
	}

	Device (CARD)
	{
		Name (_ADR, 0x00000008)
		Method (_RMV, 0, NotSerialized)
		{
			Return (0)
		}
	}
}

Device (SDXC)
{
	Name (_ADR, 0x001E0006)
	Name (_DDN, "SD Controller")

	OperationRegion (SDCR, PCI_Config, 0x00, 0x100)
	Field (SDCR, DWordAcc, NoLock, Preserve)
	{
		Offset (0x84),	/* PMECTRLSTATUS */
		D0D3, 2,	/* POWERSTATE */
		Offset (0xa2),	/* PG_CONFIG */
		, 2,
		PGEN, 1,	/* PG_ENABLE */
	}

	Method (_PS0, 0, Serialized)
	{
		/* Disable Power Gate */
		Store (0, ^PGEN)

		/* Clear bits 8, 7, 2, 0 */
		^^PCRA (PID_SCS, 0x600, 0xFFFFFE7A)
		Sleep (2)

		/* Set bits 31, 6, 2, 0 */
		^^PCRO (PID_SCS, 0x600, 0x00000185)

		/* Set Power State to D0 */
		Store (Zero, Local0)
		Store (Local0, ^D0D3)
		Store (^D0D3, Local0)
	}

	Method (_PS3, 0, Serialized)
	{
		/* Enable Power Gate */
		Store (1, ^PGEN)

		/* Set Power State to D0 */
		Store (3, Local0)
		Store (Local0, ^D0D3)
		Store (^D0D3, Local0)
	}

	Device (CARD)
	{
		Name (_ADR, 0x00000008)
		Method (_RMV, 0, NotSerialized)
		{
			Return (1)
		}
	}
}
