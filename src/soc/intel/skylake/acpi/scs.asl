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
}

#if !IS_ENABLED(CONFIG_EXCLUDE_NATIVE_SD_INTERFACE)
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
		/* Disable 20K pull-down on CLK, CMD and DAT lines */
		^^PCRA (PID_GPIOCOM3, 0x4c4, 0xFFFFEFFF)
		^^PCRA (PID_GPIOCOM3, 0x4cc, 0xFFFFEFFF)
		^^PCRA (PID_GPIOCOM3, 0x4d4, 0xFFFFEFFF)
		^^PCRA (PID_GPIOCOM3, 0x4dc, 0xFFFFEFFF)
		^^PCRA (PID_GPIOCOM3, 0x4e4, 0xFFFFEFFF)
		^^PCRA (PID_GPIOCOM3, 0x4f4, 0xFFFFEFFF)

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

		/* Enable 20K pull-down on CLK, CMD and DAT lines */
		^^PCRO (PID_GPIOCOM3, 0x4c4, 0x00001000)
		^^PCRO (PID_GPIOCOM3, 0x4cc, 0x00001000)
		^^PCRO (PID_GPIOCOM3, 0x4d4, 0x00001000)
		^^PCRO (PID_GPIOCOM3, 0x4dc, 0x00001000)
		^^PCRO (PID_GPIOCOM3, 0x4e4, 0x00001000)
		^^PCRO (PID_GPIOCOM3, 0x4f4, 0x00001000)
	}
}
#endif
