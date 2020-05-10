/* SPDX-License-Identifier: GPL-2.0-only */

Device (PEPD)
{
	Name (_HID, "INT33A1" /* Intel Power Engine */)
	Name (_CID, EisaId ("PNP0D80") /* System Power Management Controller */)
	Name (_UID, One)
}
