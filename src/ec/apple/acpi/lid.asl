/* SPDX-License-Identifier: GPL-2.0-only */

Scope(\_SB.PCI0.LPCB.EC)
{
	Field(ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset(LIDS_OFFSET),
		LIDS, 1,   /* Lid status */

		Offset(WKLD_OFFSET),
		WKLD, 1,   /* Lid wake */
	}

	Device(LID)
	{
		Name(_HID, "PNP0C0D")

		Method(_LID, 0, NotSerialized)
		{
			Return(LIDS)
		}

		Method(_PRW, 0, NotSerialized)
		{
#if LIDS_OFFSET == 0x01
			Return(Package() { 0x1d, 0x03 })
#else
			Return(Package() { 0x23, 0x04 })
#endif
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
}
