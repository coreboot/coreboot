/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0) {
	/* 0xD6- is the port address */
	/* 0x600- is the dynamic clock gating control register offset (GENR) */
	OperationRegion (SBMM, SystemMemory,
		CONFIG_PCR_BASE_ADDRESS | (0xD6 << PCR_PORTID_SHIFT) | 0x0600, 0x18)
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
		if (Arg0 == 1) {
			^GENR |= Arg1
		} ElseIf (Arg0 == 0) {
			^GENR &= Arg1
		}
	}

	/* eMMC */
	Device (SDHA) {
		Name (_ADR, 0x001C0000)
		Name (_DDN, "Intel(R) eMMC Controller - 80865ACC")
		Name (UUID, ToUUID ("E5C937D0-3553-4D7A-9117-EA4D19C3434D"))

		/*
		 * Device Specific Method
		 * Arg0 - UUID
		 * Arg1 - Revision
		 * Arg2 - Function Index
		 */
		Method (_DSM, 4)
		{
			If (Arg0 == ^UUID) {
				/*
				 * Function 9: Device Readiness Durations
				 * Returns a package of five integers covering
				 * various device related delays in PCIe Base Spec.
				 */
				If (Arg2 == 9) {
					/*
					 * Function 9 support for revision 3.
					 * ECN link for function definitions
					 * [https://pcisig.com/sites/default/files/
					 * specification_documents/
					 * ECN_fw_latency_optimization_final.pdf]
					 */
					If (Arg1 == 3) {
						/*
						 * Integer 0: FW reset time.
						 * Integer 1: FW data link up time.
						 * Integer 2: FW functional level reset
						 * time.
						 * Integer 3: FW D3 hot to D0 time.
						 * Integer 4: FW VF enable time.
						 * set ACPI constant Ones for elements
						 * where overriding the default value
						 * is not desired.
						 */
						Return (Package (5) {0, Ones, Ones,
									    Ones, Ones})
					}
				}
			}
			Return (Buffer() { 0x00 })
		}

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

		Device (CARD)
		{
			Name (_ADR, 0x00000008)
			Method (_RMV, 0, NotSerialized)
			{
				Return (0)
			}
		}
	} /* Device (SDHA) */

	/* SD CARD */
	Device (SDCD)
	{
		Name (_ADR, 0x001B0000)
		Name (_S0W, 4) /* _S0W: S0 Device Wake State */
		Name (SCD0, 0) /* Store SD_CD DW0 address */

		/* Set the host ownership of sdcard cd during kernel boot */
		Method (_INI, 0)
		{
			/* Check SDCard CD port is valid */
			If (\SCDP != 0 && \SCDO != 0)
			{
				/* Store DW0 address of SD_CD */
				SCD0 = GDW0 (\SCDP, \SCDO)
				/* Get the current SD_CD ownership */
				Local0 = \_SB.GHO (\SCDP, \SCDO)
				/* Set host ownership as GPIO in HOSTSW_OWN reg */
				Local0 |= 1 << (\SCDO % 32)
				\_SB.SHO (\SCDP, \SCDO, Local0)
			}
		}

		Method (_PS0, 0, NotSerialized)
		{
			/* Check SDCard CD port is valid */
			If (\SCDP != 0 && \SCDO != 0)
			{
				/* Store DW0 into local0 to get rxstate of GPIO */
				Local0 = \_SB.GPC0 (SCD0)
				/* Extract rxstate [bit 1] of sdcard card detect pin */
				Local0 &= PAD_CFG0_RX_STATE
				/* If the sdcard is present, rxstate is low.
				 * If sdcard is not present, rxstate is High.
				 * Write the inverted value of rxstate to GRR3.
				 */
				If (Local0 == 0) {
					^^GRR3 = 1
				} Else {
					^^GRR3 = 0
				}
				Sleep (2)
			}
		}

		Method (_PS3, 0, NotSerialized)
		{
			/* Clear GRR3 to Power Gate SD Controller */
			^^GRR3 = 0
		}

		Device (CARD)
		{
			Name (_ADR, 0x00000008)
			Method (_RMV, 0, NotSerialized)
			{
				Return (1)
			}
		}
	} /* Device (SDCD) */
}
