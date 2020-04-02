/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/* Intel i82801I HDA */

// Intel High Definition Audio (Azalia) 0:1b.0

Device (HDEF)
{
	Name (_ADR, 0x001b0000)

	// Power Resources for Wake
	Name (_PRW, Package(){
		5,  // Bit 5 of GPE
		4   // Can wake from S4 state.
	})
}
