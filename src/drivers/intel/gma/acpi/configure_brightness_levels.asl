/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Nico Huber <nico.huber@secunet.com>
 * Copyright (C) 2018 Nico Huber <nico.h@gmx.de>
 * Copyright (C) 2018 Patrick Rudolph
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

	/*
	 * Pseudo device that contains methods to modify Opregion
	 * "Mailbox 3 BIOS to Driver Notification"
	 * The BIOS to Driver Notification mailbox is intended to support
	 * BIOS to Driver event notification or data storage for BIOS to
	 * Driver data synchronization purpose.
	*/
	Device (BOX3)
	{
		Name (_ADR, 0)

		OperationRegion (OPRG, SystemMemory, ASLS, 0x2000)
		Field (OPRG, DWordAcc, NoLock, Preserve)
		{
			// OpRegion Header
			Offset (0x58),
			MBOX, 32,

			// Mailbox 3
			Offset (0x300),
			ARDY, 1,	/* Offset 0   Driver readiness */
			, 31,
			ASLC, 32,	/* Offset 4   ASLE interrupt command / status */
			TCHE, 32,	/* Offset 8   Technology enabled indicator */
			ALSI, 32,	/* Offset 12  Current ALS illuminance reading */
			BCLP, 32,	/* Offset 16  Backlight britness to set */
			PFIT, 32,	/* Offset 20  Panel fitting Request */
			CBLV, 32,	/* Offset 24  Brightness Current State */
		}

		/*
		 * Request back-light brightness change through mailbox 3
		 *
		 * @param Arg0 The brightness level to set in percent
		 * @Return Zero on success, Ones on failure
		 * Errors: * ASLS is zero
		 *         * Mailbox 3 support not advertised
		 *         * Driver not loaded or not ready
		 *         * Driver reported an error during ASLE IRQ
		 */
		Method (XBCM, 1, NotSerialized)
		{
			If (LEqual(ASLS, Zero))
			{
				Return (Ones)
			}
			If (LEqual(And(MBOX, 0x4), Zero))
			{
				Return (Ones)
			}

			/* Always keep BCLP up to date, even if driver is not ready.
			   It requires a full 8-bit brightness value. 255 = 100% */
			Store (Divide (Multiply (Arg0, 255), 100), Local1)
			If (LGreater(Local1, 255)) {
				Store (255, Local1)
			}
			/* also set valid bit */
			Store (Or (Local1, 0x80000000), BCLP)

			If (LEqual(ARDY, Zero))
			{
				Return (Ones)
			}

			/* Request back-light change */
			Store (0x2, ASLC)
			/* Trigger IRQ */
			Store (0x1, ASLE)

			Store (0x20, Local0)
			While (LGreater(Local0, Zero))
			{
				Sleep (1)
				If (LEqual(And(ShiftRight(ASLC, 12), 0x3), Zero))
				{
					Return (Zero)
				}
				Decrement (Local0)
			}

			Return (Ones)
		}

		/*
		 * Get current back-light brightness through mailbox 3
		 *
		 * @Return The current brightness or Ones on error
		 * Errors: * ASLS is zero
		 *         * Mailbox 3 support not advertised
		 *         * Driver not loaded or not ready
		 *         * CBLV is not marked valid
		 */
		Method (XBQC, 0, NotSerialized)
		{
			If (LEqual(ASLS, Zero))
			{
				Return (Ones)
			}
			If (LEqual(And(MBOX, 0x4), Zero))
			{
				Return (Ones)
			}
			If (LEqual(ARDY, Zero))
			{
				Return (Ones)
			}
			If (LEqual(And (CBLV, 0x80000000), Zero))
			{
				Return (Ones)
			}
			Return (And (CBLV, 0xff))
		}
	}

	/*
	 * Pseudo device that contains methods to operate on GTT memory
	 */
	Device (LEGA)
	{
		Name (_ADR, 0)

		Method (XBCM, 1, NotSerialized)
		{
			Store (Divide (Multiply (Arg0, BCLM), 100), BCLV)
		}

		Method (XBQC, 0, NotSerialized)
		{
			/* Find value close to BCLV in BRIG (which must be ordered) */
			Store (BCLV, Local0)			// Current value
			Store (BCLM, Local1)			// For calculations
			Store (2, Local2)			// Loop index
			While (LLess (Local2, Subtract (SizeOf (BRIG), 1))) {
				Store (DeRefOf (Index (BRIG, Local2)), Local3)
				/* Use same calculation as XBCM, to get exact matches */
				Store (Divide (Multiply (Local3, Local1), 100), Local3)

				If (LLessEqual (Local0, Local3)) {
					Return (DeRefOf (Index (BRIG, Local2)))
				}
				Add (Local2, 1, Local2)
			}
			/* Didn't find greater/equal value: use the last */
			Return (DeRefOf (Index (BRIG, Local2)))
		}
	}

	Method (XBCM, 1, NotSerialized)
	{
		If (LEqual(^BOX3.XBCM (Arg0), Ones))
		{
			^LEGA.XBCM (Arg0)
		}
	}

	Method (XBQC, 0, NotSerialized)
	{
		Store (^BOX3.XBQC (), Local0)
		If (LEqual(Local0, Ones))
		{
			Store (^LEGA.XBQC (), Local0)
		}

		Return (Local0)
	}
