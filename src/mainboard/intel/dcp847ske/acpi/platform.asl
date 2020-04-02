/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/*
 * The _PTS method (Prepare To Sleep) is called before the OS is
 * entering a sleep state. The sleep state number is passed in Arg0
 */
Method(_WAK,1)
{
	Return(Package(){0,0})
}

/* The _WAK method is called on system wakeup */
Method(_PTS,1)
{
}
