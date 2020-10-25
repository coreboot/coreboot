/* SPDX-License-Identifier: GPL-2.0-only */

/* Intel PCH HDA */

// Intel High Definition Audio (Azalia) 0:1b.0

Device (HDEF)
{
	Name (_ADR, 0x001b0000)

	Name (PRWH, Package (){ 0x0d, 3 }) // LPT-H
	Name (PRWL, Package (){ 0x6d, 3 }) // LPT-LP

	Method (_PRW, 0) { // Power Resources for Wake
		If (\ISLP ()) {
			Return (PRWL)
		} Else {
			Return (PRWH)
		}
	}
}
