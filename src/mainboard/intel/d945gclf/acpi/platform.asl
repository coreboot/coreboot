/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* The _PTS method (Prepare To Sleep) is called before the OS is
 * entering a sleep state. The sleep state number is passed in Arg0
 */

Method(_PTS,1)
{
	// Call a trap so SMI can prepare for Sleep as well.
	// TRAP(0x55)
}

/* The _WAK method is called on system wakeup */

Method(_WAK,1)
{
	// CPU specific part

	// Notify PCI Express slots in case a card
	// was inserted while a sleep state was active.

	// Are we going to S3?
	If (LEqual(Arg0, 3)) {
		// ..
	}

	// Are we going to S4?
	If (LEqual(Arg0, 4)) {
		// ..
	}

	// TODO: Windows XP SP2 P-State restore

	Return(Package(){0,0})
}
