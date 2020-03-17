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

/* Call from \_SB._PTS() */
Method (PTS, 1, Serialized)
{
	Printf ("EC _PTS")
	W (FPTS, Arg0)
}

/* Call from \_SB._WAK() */
Method (WAK, 1, Serialized)
{
	Printf ("EC _WAK")
	W (FWAK, Arg0)
}
