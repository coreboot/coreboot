/* SPDX-License-Identifier: GPL-2.0-only */

Device (ADP1)
{
	Name (_HID, "ACPI0003" /* Power Source Device */)
	Name (_PCL, Package () { \_SB })  // _PCL: Power Consumer List

	Method (_PSR, 0, NotSerialized)  // _PSR: Power Source
	{
#if CONFIG(EC_USE_LGMR)
		Return (MACS)
#else
		Return (EACS)
#endif
	}
}
