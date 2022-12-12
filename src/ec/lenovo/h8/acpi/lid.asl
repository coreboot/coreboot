/* SPDX-License-Identifier: GPL-2.0-only */

Field(ERAM, ByteAcc, NoLock, Preserve)
{
		Offset (0x32),
			    , 2,
			WKLD, 1,
		Offset (0x46),
			    , 2,
			LIDS, 1
}

Device(LID)
{
	Name(_HID, "PNP0C0D")

	Method(_LID, 0, NotSerialized)
	{
		return (LIDS)
	}

	Method(_PRW, 0, NotSerialized)
	{
		Return (Package() { 0x18, 0x03 })
	}

	Method(_PSW, 1, NotSerialized)
	{
		if (Arg0) {
			WKLD = 1
		} else {
			WKLD = 0
		}
	}
}
