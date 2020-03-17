/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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

	Return (One)
}

/* Enable Privacy Screen */
Method (EPVX, 0, Serialized)
{
	W (EPCT, One)
}

/* Disable Privacy Screen */
Method (DPVX, 0, Serialized)
{
	W (EPCT, Zero)
}
