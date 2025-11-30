/* SPDX-License-Identifier: GPL-2.0-only */

	Name (BRLV, 0) /* Brightness Last Value */
	Name (BRVA, 0) /* Brightness Valid */

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
		/*
		 * Only cover MBOX3 since Windows 10/11 doesn't like
		 * when the full (MBOX4/MBOX5 as well) OpRegion is covered.
		 */
		OperationRegion (OPRG, SystemMemory, ASLS, 0x400)
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
		 * @Return 0 on success, Ones on failure
		 * Errors: * ASLS is zero
		 *         * Mailbox 3 support not advertised
		 *         * Driver not loaded or not ready
		 *         * Driver reported an error during ASLE IRQ
		 */
		Method (XBCM, 1, Serialized)
		{
			If (ASLS == 0)
			{
				Return (Ones)
			}
			If ((MBOX & 4) == 0)
			{
				Return (Ones)
			}

			/* Always keep BCLP up to date, even if driver is not ready.
			   It requires a full 8-bit brightness value. 255 = 100% */
			Local1 = Arg0 * 255 / 100
			If (Local1 > 255) {
				Local1 = 255
			}
			/* also set valid bit */
			BCLP = Local1 | 0x80000000

			If (ARDY == 0) {
				Return (Ones)
			}

			/* Request back-light change */
			ASLC = 0x2
			/* Trigger IRQ */
			ASLE = 0x1

			Local0 = 0x20
			While (Local0 > 0) {
				Sleep (1)
				If ((ASLC & 2) == 0) {
					/* Request has been processed, check status: */
					Local1 = (ASLC >> 12) & 3
					If (Local1 == 0) {
						Return (0)
					} Else {
						Return (Ones)
					}
				}
				Local0--
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
			Return ((Arg0 + Arg1 / 2) / Arg1)
		}

		Method (XBCM, 1, NotSerialized)
		{
			BCLV = DRCL (Arg0 * BCLM, 100)
		}

		/* Find value closest to BCLV in BRIG (which must be ordered) */
		Method (XBQC, 0, NotSerialized)
		{
			/* Prevent DivideByZero if backlight control isn't enabled */
			If (BCLM == 0) {
				Return (0)
			}
			/* Local0: current percentage */
			Local0 = DRCL (BCLV * 100, BCLM)

			/* Local1: loop index (selectable values start at 2 in BRIG) */
			Local1 = 2
			While (Local1 < SizeOf (BRIG) - 1) {
				/* Local[23]: adjacent values in BRIG */
				Local2 = DeRefOf (BRIG[Local1])
				Local3 = DeRefOf (BRIG[Local1 + 1])

				If (Local0 < Local3) {
					If (Local0 < Local2 || Local0 - Local2 < Local3 - Local0) {
						Return (Local2)
					} Else {
						Return (Local3)
					}
				}

				Local1++
			}

			/* Didn't find greater/equal value: use the last */
			Return (Local3)
		}
	}

	Method (XBCM, 1, NotSerialized)
	{
		BRLV = Arg0
		BRVA = 1
		If (^BOX3.XBCM (Arg0) == Ones) {
			/*
			 * Only touch the legacy PWM registers after the graphics driver
			 * reprograms them during boot/resume (BCLM stays zero until then).
			 */
			If (BCLM != 0) {
				^LEGA.XBCM (Arg0)
			}
		}
	}

	Method (XBQC, 0, NotSerialized)
	{
		/*
		 * During early boot / resume the IGD driver has not yet populated
		 * the OpRegion brightness fields (BCLM stays zero), so fall back to
		 * the cached value we last exposed to the OS. If there's no cached
		 * value yet, use the platform's default from BRIG[0].
		 */
		If (BCLM == 0) {
			If (BRVA != 0) {
				Return (BRLV)
			}

			/* No cached brightness yet, fall back to platform default. */
			Local0 = DeRefOf (BRIG[0])
			Return (Local0)
		}

		Local0 = ^LEGA.XBQC ()
		If (BRVA != 0 && Local0 != BRLV) {
			/*
			 * The OS replays _BCM requests while the graphics driver is
			 * still reinitializing, so hardware brightness can diverge
			 * from what we cached in BRLV. Reapply the cached level once
			 * the OpRegion is ready to keep firmware and OS state aligned.
			 * Use BRCT flag to prevent recursion.
			 */
			If (BRCT == 0)
			{
				BRCT = 1
				XBCM (BRLV)
				Local0 = ^LEGA.XBQC ()
				BRCT = 0
			}
		}
		BRLV = Local0
		BRVA = 1
		Return (Local0)
	}
