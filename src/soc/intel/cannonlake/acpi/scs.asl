/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017-2018 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <soc/pcr_ids.h>

Scope (\_SB.PCI0) {
	/* EMMC */
	Device(PEMC) {
		Name(_ADR, 0x001A0000)
		Name (_DDN, "eMMC Controller")
		Name (TEMP, 0)

		OperationRegion(SCSR, PCI_Config, 0x00, 0x100)
		Field(SCSR, WordAcc, NoLock, Preserve) {
			Offset (0x84),	/* PMECTRLSTATUS */
			PMCR, 16,
			Offset (0xA2),	/* PG_CONFIG */
			, 2,
			PGEN, 1,	/* PG_ENABLE */
		}

		Method(_PS0, 0, Serialized) {
			Stall (50) // Sleep 50 us

			Store(0, PGEN) // Disable PG

			/* Clear register 0x1C20/0x4820 */
			^^PCRA (PID_EMMC, 0x1C20, 0x0)
			^^PCRA (PID_EMMC, 0x4820, 0x0)

			/* Set Power State to D0 */
			And (PMCR, 0xFFFC, PMCR)
			Store (PMCR, ^TEMP)
		}

		Method(_PS3, 0, Serialized) {
			Store(1, PGEN) // Enable PG

			/* Set Power State to D3 */
			Or (PMCR, 0x0003, PMCR)
			Store (PMCR, ^TEMP)
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

	/* SD CARD */
	Device (SDXC)
	{
		Name (_ADR, 0x00140005)
		Name (_DDN, "SD Controller")
		Name (TEMP, 0)

		OperationRegion (SDPC, PCI_Config, 0x00, 0x100)
		Field (SDPC, WordAcc, NoLock, Preserve)
		{
			Offset (0x84),	/* PMECTRLSTATUS */
			PMCR, 16,
			Offset (0xA2),	/* PG_CONFIG */
			, 2,
			PGEN, 1,	/* PG_ENABLE */
		}

		Method (_PS0, 0, Serialized)
		{
			Store (0, PGEN) /* Disable PG */

			/* Clear register 0x1C20/0x4820 */
			^^PCRA (PID_SDX, 0x1C20, 0x0)
			^^PCRA (PID_SDX, 0x4820, 0x0)

			/* Set Power State to D0 */
			And (PMCR, 0xFFFC, PMCR)
			Store (PMCR, ^TEMP)
		}

		Method (_PS3, 0, Serialized)
		{
			Store (1, PGEN) /* Enable PG */

			/* Set Power State to D3 */
			Or (PMCR, 0x0003, PMCR)
			Store (PMCR, ^TEMP)
		}

		Device (CARD)
		{
			Name (_ADR, 0x00000008)
			Method (_RMV, 0, NotSerialized)
			{
				Return (1)
			}
		}
	} /* Device (SDXC) */
}
