/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation.
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
	/* 0xD6- is the port address */
	/* 0x600- is the dynamic clock gating control register offset (GENR) */
	OperationRegion (SBMM, SystemMemory,
				Or ( Or (CONFIG_IOSF_BASE_ADDRESS,
					ShiftLeft(0xD6, 16)), 0x0600), 0x18)
	Field (SBMM, DWordAcc, NoLock, Preserve)
	{
		GENR, 32,
		Offset (0x08),
		,  5, /* bit[5] represents Force Card Detect SD Card */
		GRR3,  1, /* GPPRVRW3 for SD Card detect Bypass. It's active high */
	}

	/* SCC power gate control method, this method must be serialized as
	 * multiple device will control the GENR register
	 *
	 * Arguments: (2)
	 * Arg0: 0-AND  1-OR
	 * Arg1: Value
	 */
	Method (SCPG, 2, Serialized)
	{
		if (LEqual(Arg0, 0x1)) {
			Or (^GENR, Arg1, ^GENR)
		} ElseIf (LEqual(Arg0, 0x0)){
			And (^GENR, Arg1, ^GENR)
		}
	}

	/* eMMC */
	Device (SDHA) {
		Name (_ADR, 0x001C0000)
		Name (_DDN, "Intel(R) eMMC Controller - 80865ACC")

		Method (_PS0, 0, NotSerialized)
		{
			/* Clear clock gate
			 * Clear bit 6 and 0
			 */
			^^SCPG(0,0xFFFFFFBE)
			/* Sleep 2 ms */
			Sleep (2)
		}

		Method (_PS3, 0, NotSerialized)
		{
			/* Enable power gate
			 * Restore clock gate
			 * Restore bit 6 and 0
			 */
			^^SCPG(1,0x00000041)
		}
	} /* Device (SDHA) */

	/* SD CARD */
	Device (SDCD)
	{
		Name (_ADR, 0x001B0000)

		Method (_PS0, 0, NotSerialized)
		{
			/* Check SDCard CD pin address is valid */
			If (LNotEqual (SCD0, 0))
			{
				/* Store DW0 into local0 to get rxstate of GPIO */
				Store (\_SB.GPC0 (\SCD0), Local0)
				/* Extract rxstate [bit 1] of sdcard card detect pin */
				And (Local0, PAD_CFG0_RX_STATE, Local0)
				/* If the sdcard is present, rxstate is low.
				 * If sdcard is not present, rxstate is High.
				 * Write the inverted value of rxstate to GRR3.
				 */
				If (LEqual (Local0, 0)) {
					Store (1, ^^GRR3)
				} Else {
					Store (0, ^^GRR3)
				}
				Sleep (2)
			}
		}

		Method (_PS3, 0, NotSerialized)
		{
			/* Clear GRR3 to Power Gate SD Controller */
			Store (0, ^^GRR3)
		}

	} /* Device (SDCD) */
}
