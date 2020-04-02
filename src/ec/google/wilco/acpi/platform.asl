/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
