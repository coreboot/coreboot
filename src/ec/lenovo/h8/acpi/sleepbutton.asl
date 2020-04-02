/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

Field(ERAM, ByteAcc, NoLock, Preserve)
{
		Offset (0x32),
			    , 4,
			WKFN, 1,
		Offset(0x83),
			FNKY, 8
}

Device(SLPB)
{
	Name (_HID, EisaId ("PNP0C0E"))
	Method(_PRW, 0, NotSerialized)
	{
		Return (Package() { 0x18, 0x03 })
	}

	Method(_PSW, 1, NotSerialized)
	{
		if (Arg0) {
			Store(6, FNKY) /* Fn key acts as wake button */
			Store(1, WKFN)
		} else {
			Store(0, FNKY) /* Fn key normal operation */
			Store(0, WKFN)
		}
	}
}
