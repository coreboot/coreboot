/* SPDX-License-Identifier: GPL-2.0-only */

	External(LCD0, DeviceObj)

	Name (BRCT, 0)

	Method(BRID, 1, NotSerialized)
	{
		Store (Match (BRIG, MEQ, Arg0, MTR, Zero, 2), Local0)
		If (Local0 == Ones)
		{
			Return (SizeOf(BRIG) - 1)
		}
		Return (Local0)
	}

	Method (XBCL, 0, NotSerialized)
	{
		Store (1, BRCT)
		Return (BRIG)
	}

	/* Display Output Switching */
	Method (_DOS, 1)
	{
	}

	/*
	 * Decrement display brightness.
	 *
	 * Using Notify is the right way. But Windows doesn't handle
	 * it well. So use both method in a way to avoid double action.
	 */
	Method (DECB, 0, NotSerialized)
	{
		If (BRCT)
		{
			Notify (LCD0, 0x87)
		} Else {
			Store (BRID (XBQC ()), Local0)
			If (Local0 != 2)
			{
				Local0--
			}
			XBCM (DerefOf (BRIG[Local0]))
		}
	}

	/*
	 * Increment display brightness.
	 */
	Method (INCB, 0, NotSerialized)
	{
		If (BRCT)
		{
			Notify (LCD0, 0x86)
		} Else {
			Store (BRID (XBQC ()), Local0)
			If (Local0 != SizeOf(BRIG) - 1)
			{
				Local0++
			}
			XBCM (DerefOf (BRIG[Local0]))
		}
	}
