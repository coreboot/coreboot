/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
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

Scope (\_SB.PCI0) {
	/* EMMC */
	Device(PEMC) {
		Name(_ADR, 0x001A0000)

		OperationRegion(SCSR, PCI_Config, 0x00, 0x100)
		Field(SCSR, WordAcc, NoLock, Preserve) {
			Offset(0xA2),   // 0xA2, Device PG config
			, 2,
			PGEN, 1         // [BIT2] PGE - PG Enable
		}

		Method(_PS0, 0, Serialized) {
			Stall (50) // Sleep 50 ms
			Store(0, PGEN) // Disable PG
		}

		Method(_PS3, 0, Serialized) {
			Store(1, PGEN) // Enable PG
		}
	}

	/* SD CARD */
	Device (SDXC)
	{
		Name (_ADR, 0x00140005)

		OperationRegion (SDPC, PCI_Config, 0x00, 0x100)
		Field (SDPC, WordAcc, NoLock, Preserve)
		{
			Offset(0xA2),  /* Device Power Gate config */
			, 2,
			PGEN, 1 /* PGE - PG Enable */
		}

		Method (_PS0, 0, Serialized)
		{
			Store (0, PGEN) /* Disable PG */
		}

		Method (_PS3, 0, Serialized)
		{
			Store (1, PGEN) /* Enable PG */
		}
	} /* Device (SDXC) */
}
