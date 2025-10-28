
/* SPDX-License-Identifier: GPL-2.0-or-later */

Device (CXLM)
{
	Name (STAV, 0x00)
	Name (_HID, "ACPI0017") /* CXL Root Device */
	Method (_STA, 0, NotSerialized)
	{
		Return (STAV)
	}
	Name (_UID, 0)

	Method (_DSM, 4, Serialized)
	{
		Name (MQTG, 0)
		Name (PKRL, 0)
		Name (PKWL, 0)
		Name (PKRB, 0)
		Name (PKWB, 0)
		Name (QTGR, Package (0x04)
		{
			PKRL,
			PKWL,
			PKRB,
			PKWB
		})
		If ((Arg0 == ToUUID ("f365f9a6-a7de-4071-a66a-b40c0b4f8e52")))
		{
			If ((Arg1 == One))
			{
				If ((Arg2 == One))
				{
					Return (Package (0x02)
					{
						MQTG,
						QTGR
					})
				}
			}
		}

		Return (Buffer() {0x00})
	}
}
