/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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

/* Trusted Platform Module */

Device (TPM)
{
	Name (_HID, EISAID ("PNP0C31"))
	Name (_CID, 0x310cd041)
	Name (_UID, 1)

	Method (_STA, 0)
	{
#if CONFIG_LPC_TPM && !CONFIG_TPM_DEACTIVATE
		Return (0xf)
#else
		Return (0x0)
#endif
	}

	Name (IBUF, ResourceTemplate ()
	{
		/* Updated based on TPM interrupt for Locality 0 */
		Interrupt (ResourceConsumer, Edge, ActiveHigh,
			   Exclusive, , , TIRQ) { 0 }
	})

	Name (RBUF, ResourceTemplate ()
	{
		IO (Decode16, 0x2e, 0x2e, 0x01, 0x02)
		Memory32Fixed (ReadWrite, CONFIG_TPM_TIS_BASE_ADDRESS, 0x5000)
	})

	Method (_CRS, 0, Serialized)
	{
		OperationRegion (TREG, SystemMemory,
				 CONFIG_TPM_TIS_BASE_ADDRESS, 0x5000)
		Field (TREG, ByteAcc, NoLock, Preserve)
		{
			/* TPM_INT_ENABLE_0 */
			Offset (0x0008),
			, 3,
			ITPL, 2,  /* Interrupt type and polarity */

			/* TPM_INT_VECTOR_0 */
			Offset (0x000C),
			IVEC, 4,  /* SERIRQ vector */
		}

		CreateField (^IBUF, ^TIRQ._INT, 32, TVEC)
		CreateBitField (^IBUF, ^TIRQ._HE, TTYP)
		CreateBitField (^IBUF, ^TIRQ._LL, TPOL)
		CreateBitField (^IBUF, ^TIRQ._SHR, TSHR)

		If (LGreater (CONFIG_TPM_PIRQ, 0)) {
			/*
			 * PIRQ: Update interrupt vector with configured PIRQ
			 */
			Store (CONFIG_TPM_PIRQ, TVEC)

			/* Active-Low Level-Triggered Shared */
			Store (One, TPOL)
			Store (Zero, TTYP)
			Store (One, TSHR)

			/* Merge IRQ with base address */
			Return (ConcatenateResTemplate (RBUF, IBUF))
		} ElseIf (LGreater (IVEC, 0)) {
			/*
			 * SERIRQ: Update interrupt vector based on TPM register
			 */
			Store (IVEC, TVEC)

			If (LEqual (ITPL, 0x0)) {
				/* Active-High Level-Triggered Shared */
				Store (Zero, TPOL)
				Store (Zero, TTYP)
				Store (One, TSHR)
			} ElseIf (LEqual (ITPL, 0x1)) {
				/* Active-Low Level-Triggered Shared */
				Store (One, TPOL)
				Store (Zero, TTYP)
				Store (One, TSHR)
			} ElseIf (LEqual (ITPL, 0x2)) {
				/* Active-High Edge-Triggered Exclusive */
				Store (Zero, TPOL)
				Store (One, TTYP)
				Store (Zero, TSHR)
			} ElseIf (LEqual (ITPL, 0x3)) {
				/* Active-Low Edge-Triggered Exclusive */
				Store (One, TPOL)
				Store (One, TTYP)
				Store (Zero, TSHR)
			}

			/* Merge IRQ with base address */
			Return (ConcatenateResTemplate (RBUF, IBUF))
		} Else {
			Return (RBUF)
		}
	}

	/* Dummy _DSM to make Bitlocker work.  */
	Method (_DSM, 4, Serialized)
	{
		/* Physical presence interface.
		   This is used to submit commands like "Clear TPM" to
		   be run at next reboot provided that user confirms them.
		   Spec allows user to cancel all commands and/or
		   configure BIOS to reject commands. So we pretend that
		   user did just this: cancelled everything. If user
		   really wants to clear TPM the only option now is to do it manually
		   in payload.
		 */
		If (LEqual (Arg0, ToUUID ("3dddfaa6-361b-4eb4-a424-8d10089d1653")))
		{
			If (LEqual (Arg2, 0))
			{
				/* Functions 1-8.  */
				Return (Buffer (2) { 0xFF, 0x01 })
			}

			/* Interface version: 1.2  */
			If (LEqual (Arg2, 1))
			{
				Return ("1.2")
			}

			/* Submit operations: drop on the floor and return success.  */
			If (LEqual (Arg2, 2))
			{
				Return (0x00)
			}

			/* Pending operation: none.  */
			If (LEqual (Arg2, 3))
			{
				Return (Package (2) { 0, 0 })
			}

			/* Pre-OS transition method: reboot. */
			If (LEqual (Arg2, 4))
			{
				Return (2)
			}

			/* Operation response: no operation executed.  */
			If (LEqual (Arg2, 5))
			{
				Return (Package (3) { 0, 0, 0 })
			}

			/* Set preffered user language: deprecated and must return 3 aka "not implemented".  */
			If (LEqual (Arg2, 6))
			{
				Return (3)
			}

			/* Submit operations: deny.  */
			If (LEqual (Arg2, 7))
			{
				Return (3)
			}

			/* All actions are forbidden.  */
			If (LEqual (Arg2, 8))
			{
				Return (1)
			}

			Return (1)
		}

		/* Memory clearing on boot: just a dummy.  */
		If (LEqual (Arg0, ToUUID("376054ed-cc13-4675-901c-4756d7f2d45d")))
		{
			If (LEqual (Arg2, 0))
			{
				/* Function 1.  */
				Return (Buffer (1) { 3 })
			}

			/* Just return success.  */
			If (LEqual (Arg2, 1))
			{
				Return (0)
			}

			Return (1)
		}

		Return (Buffer (1) { 0 })
	}
}
