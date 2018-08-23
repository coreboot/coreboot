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
	}

	/*
	 * Pseudo device that contains methods to operate on GTT memory
	 */
	Device (LEGA)
	{
		Name (_ADR, 0)

		/* Divide round closest */
		Method (DRCL, 2)
		{
			Return (Divide (Add (Arg0, Divide (Arg1, 2)), Arg1))
		}

		Method (XBCM, 1, NotSerialized)
		{
			Store (DRCL (Multiply (Arg0, BCLM), 100), BCLV)
		}

		/* Find value closest to BCLV in BRIG (which must be ordered) */
		Method (XBQC, 0, NotSerialized)
		{
			/* Local0: current percentage */
			Store (DRCL (Multiply (BCLV, 100), BCLM), Local0)

			/* Local1: loop index (selectable values start at 2 in BRIG) */
			Store (2, Local1)
			While (LLess (Local1, Subtract (SizeOf (BRIG), 1))) {
				/* Local[23]: adjacent values in BRIG */
				Store (DeRefOf (Index (BRIG, Local1)), Local2)
				Store (DeRefOf (Index (BRIG, Add (Local1, 1))), Local3)

				If (LLess (Local0, Local3)) {
					If (LOr (LLess (Local0, Local2),
						 LLess (Subtract (Local0, Local2),
							Subtract (Local3, Local0)))) {
						Return (Local2)
					} Else {
						Return (Local3)
					}
				}

				Increment (Local1)
			}

			/* Didn't find greater/equal value: use the last */
			Return (Local3)
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
		/*
		 * Always query the hardware directly. Not all OS drivers
		 * keep CBLV up to date (one is Linux' i915). Some years
		 * after that is fixed we can probably use CBLV?
		 */
		Return (^LEGA.XBQC ())
	}
