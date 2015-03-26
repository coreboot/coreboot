/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
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

/* System Bus */

Scope(\_SB)
{
	/* This method is placed on the top level, so we can make sure it's the
	 * first executed _INI method.
	 */
	Method(_INI, 0)
	{
		/* The DTS data in NVS is probably not up to date.
		 * Update temperature values and make sure AP thermal
		 * interrupts can happen
		 */

		// TRAP(71) // TODO

		\GOS()

		/* And the OS workarounds start right after we know what we're
		 * running: Windows XP SP1 needs to have C-State coordination
		 * enabled in SMM.
		 */
		If (LAnd(LEqual(OSYS, 2001), MPEN)) {
			// TRAP(61) // TODO
		}

		/* SMM power state and C4-on-C3 settings need to be updated */
		// TRAP(43) // TODO
	}
}
