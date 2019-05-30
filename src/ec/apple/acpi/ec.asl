/* SPDX-License-Identifier: GPL-2.0-only */

Device(EC)
{
	Name(_HID, EISAID("PNP0C09"))
	Name(_UID, 0)

	Name(_GPE, 0x17)
	Mutex(ECLK, 0)

	OperationRegion(ERAM, EmbeddedControl, 0x00, 0x100)

	/* LID status change. */
	Method(_Q20, 0, NotSerialized)
	{
		Notify(LID, 0x80)
	}

	/* AC status change. */
	Method(_Q21, 0, NotSerialized)
	{
		Notify(AC, 0x80)
	}

	Method(_CRS, 0)
	{
		Name(ECMD, ResourceTemplate()
		{
			IO(Decode16, 0x62, 0x62, 1, 1)
			IO(Decode16, 0x66, 0x66, 1, 1)
		})
		Return(ECMD)
	}

	Method(_PRW, 0, NotSerialized)
	{
		Return(Package () { 0x23, 0x04 })
	}

	Method(_INI, 0, NotSerialized)
	{
	}

#include "battery.asl"
}
