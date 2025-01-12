/* SPDX-License-Identifier: GPL-2.0-only */

#if CONFIG(EC_STARLABS_MERLIN)
Method (_Q0C, 0, NotSerialized)			// Event: Lid Opened or Closed
{
	\LIDS = ECRD(RefOf(LSTE))
	Notify (LID0, 0x80)
}
#endif

Device (LID0)
{
	Name (_HID, EisaId ("PNP0C0D"))
	Method (_STA)
	{
		Return (0x0F)
	}
	Method (_LID, 0)
	{
		// 0x00 == Closed
		// 0x01 == Open
		Return (ECRD (RefOf (LSTE)))
	}
}
