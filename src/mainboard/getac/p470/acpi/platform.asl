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
 */

/* The _PTS method (Prepare To Sleep) is called before the OS is
 * entering a sleep state. The sleep state number is passed in Arg0
 */

Method(_PTS,1)
{
	TRAP(0xed)
	Sleep(1000)

	Store(0, \_SB.ACFG)

	// Are we going to S4?
	If (Lequal(Arg0, 4)) {
		TRAP(0xe7)
		TRAP(0xea)
	}

	// Are we going to S5?
	If (Lequal(Arg0, 5)) {
		TRAP(0xde)
	}

	// The 2.6.12.5 ACPI engine seems to optimize the
	// If(LEqual(Arg0, 5)) path away. This keeps it from doing so:
	TRAP(Arg0)
	Store(Arg0, DBG0)
	// End of ugly OS bug workaround
}

/* The _WAK method is called on system wakeup */

Method(_WAK,1)
{
	// Enable GPS
	Store (1, GP11) // GPSE

	// Wake from S3 or S4?
	If (LOr(LEqual(Arg0, 3), LEqual(Arg0, 4))) {
		If (And(CFGD, 0x01000000)) {
			If (LAnd(And(CFGD, 0xf0), LEqual(OSYS, 2001))) {
				TRAP(0x3d)
			}
		}
	}

	// Notify PCI Express slots in case a card
	// was inserted while a sleep state was active.

	If (LEqual(RP1D, 0)) {
		Notify(\_SB.PCI0.RP01, 0)
	}

	If (LEqual(RP3D, 0)) {
		Notify(\_SB.PCI0.RP03, 0)
	}

	If (LEqual(RP4D, 0)) {
		Notify(\_SB.PCI0.RP04, 0)
	}

	// Are we coming from S3?
	If (LEqual(Arg0, 3)) {
		TRAP(0xeb)
		TRAP(0x46)
	}

	// Are we coming from S4?
	If (LEqual(Arg0, 4)) {
		Notify(SLPB, 0x02)
		If (DTSE) {
			TRAP(0x47)
		}
	}

	// Windows XP SP2 P-State restore
	If (LAnd(LEqual(OSYS, 2002), And(CFGD, 1))) {
		If (LGreater(\_PR.CP00._PPC, 0)) {
			Subtract(\_PR.CP00._PPC, 1, \_PR.CP00._PPC)
			PNOT()
			Add(\_PR.CP00._PPC, 1, \_PR.CP00._PPC)
			PNOT()
		} Else {
			Add(\_PR.CP00._PPC, 1, \_PR.CP00._PPC)
			PNOT()
			Subtract(\_PR.CP00._PPC, 1, \_PR.CP00._PPC)
			PNOT()
		}
	}

	Return(Package(){0,0})
}

// Hardcoded for now..
Name (CFGD, 0x113B69F1)

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
		If (DTSE) {
			TRAP(0x47)
		}

		\GOS()

		/* And the OS workarounds start right after we know what we're
		 * running: Windows XP SP1 needs to have C-State coordination
		 * enabled in SMM.
		 */
		If (LAnd(LEqual(OSYS, 2001), MPEN)) {
			TRAP(0x3d)
		}

		/* SMM power state and C4-on-C3 settings need to be updated */
		// TRAP(43)

		/* OS Init */
		TRAP(0x32)
	}
}
