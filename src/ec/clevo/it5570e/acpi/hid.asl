/* SPDX-License-Identifier: GPL-2.0-only */

/* Minimal implementation of HID event filter device for airplane hotkey support */

Device (HIDD)
{
	Name (_HID, "INTC1051")

	Name (HRDY, 0)
	Name (HBSY, 0)
	Name (HIDX, 0)

	/*
	 * Workaround:
	 * There was a bug in Linux' HID driver, making evaluation of function 2 fail.
	 * The driver falls back to legacy mode and evaluates integers instead of _DSM.
	 * A bugfix for this was merged to mainline and stable but not LTS-branches. Thus,
	 * let's keep this for some time.
	 * https://lore.kernel.org/r/66f813f5bcc724a0f6dd5adefe6a9728dbe509e3.camel@mniewoehner.de
	 */
	Name (HDMM, 0)

	Method (_STA)
	{
		Return (0x0f)
	}

	Method (_DSM, 4, Serialized)
	{
		If (Arg0 == ToUUID ("eeec56b3-4442-408f-a792-4edd4d758054"))
		{
			If (Arg1 == 1) /* Revision 1 */
			{
				Printf ("HIDD: _DSM called, fn=%o", ToDecimalString(Arg2))

				Switch (ToInteger (Arg2))
				{
					Case (0)
					{
						/* Supported functions: 0, 2, 3, 4, 7 */
						Return (Buffer () {0x9d, 0x00})
					}
					Case (2)
					{
						/* Simple mode */
						Return (0)
					}
					Case (3)
					{
						/* Driver status */
						HRDY = DeRefOf (Arg3[0])
					}
					Case (4)
					{
						/* HID driver calls this to get event */
						HBSY = 0
						Return (HIDX)
					}
					Case (7)
					{
						/* Only airplane mode button implemented */
						Return (1 << 1)
					}
				}
			}
		}

		Return (Buffer () {0})
	}

	/*
	 * HID Platform Event Method
	 * Called to trigger HID event.
	 */
	Method (HPEM, 1, Serialized)
	{
		Printf ("HIDD: HPEM called, event=%o", ToHexString (Arg0))

		If (!HRDY)
		{
			Printf ("HIDD: HID driver not ready. Ignoring event.")
		}

		HBSY = 1
		HIDX = Arg0

		Notify (HIDD, 0xc0)

		/* Wait max. 1 second for HID driver */
		Local0 = 0
		While ((Local0 < 250) && HBSY)
		{
			Sleep (4)
			Local0++
		}

		If (HBSY)
		{
			Printf ("HIDD: HPEM timeout")

			HBSY = 0
			HIDX = 0

			Return (1) /* Timeout */
		}

		Return (0)
	}
}
