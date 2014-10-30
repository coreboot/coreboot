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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* Trusted Platform Module */

Device (TPM)
{
	Name (_HID, EISAID ("PNP0C31"))
	Name (_CID, 0x310cd041)
	Name (_UID, 1)

	Method (_STA, 0)
	{
		If (CONFIG_LPC_TPM) {
			Return (0xf)
		} Else {
			Return (0x0)
		}
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

	Method (_CRS, 0, NotSerialized)
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

		If (LGreater (IVEC, 0)) {
			/* Update interrupt vector */
			CreateField (^IBUF, ^TIRQ._INT, 32, TVEC)
			Store (IVEC, TVEC)

			/* Update interrupt type and polarity */
			CreateBitField (^IBUF, ^TIRQ._HE, TTYP)
			CreateBitField (^IBUF, ^TIRQ._LL, TPOL)
			CreateBitField (^IBUF, ^TIRQ._SHR, TSHR)

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
}
