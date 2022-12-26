/* SPDX-License-Identifier: GPL-2.0-only */

/* Read Privacy Screen Present */
Method (GPVD, 0, Serialized)
{
	Return (R (EPDT))
}

/* Read Privacy Screen Status */
Method (GPVX, 0, Serialized)
{
	If (R (EPST) == Zero) {
		Return (Zero)
	}

	Return (1)
}

/* Enable Privacy Screen */
Method (EPVX, 0, Serialized)
{
	W (EPCT, 1)
}

/* Disable Privacy Screen */
Method (DPVX, 0, Serialized)
{
	W (EPCT, Zero)
}
