/* SPDX-License-Identifier: GPL-2.0-only */

#if CONFIG(EC_STARLABS_MERLIN)
Method (_Q0A, 0, NotSerialized)			// Event: Charger Status Update
{
	Notify (ADP1, 0x80)
}
#endif

Device (ADP1)
{
	Name (_HID, "ACPI0003")
	Method (_STA)
	{
		Return (0x0F)
	}
	Method (_PSR, 0)
	{
		PWRS = (ECRD (RefOf(ECPS)) & 0x01)
		Return (PWRS)
	}
	Method (_PCL, 0)
	{
		Return (
			Package() { _SB }
		)
	}
}
