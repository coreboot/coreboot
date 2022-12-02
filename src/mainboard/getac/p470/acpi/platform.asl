/* SPDX-License-Identifier: GPL-2.0-only */

/* The _PTS method (Prepare To Sleep) is called before the OS is
 * entering a sleep state. The sleep state number is passed in Arg0
 */

Method(_PTS,1)
{
	TRAP(0xed)
	Sleep(1000)

	\_SB.ACFG = 0

	// Are we going to S4?
	If (Arg0 == 4) {
		TRAP(0xe7)
		TRAP(0xea)
	}

	// Are we going to S5?
	If (Arg0 == 5) {
		TRAP(0xde)
	}

	// The 2.6.12.5 ACPI engine seems to optimize the
	// If(Arg0 == 5) path away. This keeps it from doing so:
	TRAP(Arg0)
	DBG0 = Arg0
	// End of ugly OS bug workaround
}

/* The _WAK method is called on system wakeup */

Method(_WAK,1)
{
	// Enable GPS
	GP11 = 1 // GPSE

	// Wake from S3 or S4?
	If ((Arg0 == 0x03) || (Arg0 == 0x04)) {
		If (CFGD & 0x01000000) {
			If ((CFGD & 0xF0) && (OSYS == 2001)) {
				TRAP(0x3d)
			}
		}
	}

	// Notify PCI Express slots in case a card
	// was inserted while a sleep state was active.

	If (RP1D == 0) {
		Notify(\_SB.PCI0.RP01, 0)
	}

	If (RP3D == 0) {
		Notify(\_SB.PCI0.RP03, 0)
	}

	If (RP4D == 0) {
		Notify(\_SB.PCI0.RP04, 0)
	}

	// Are we coming from S3?
	If (Arg0 == 3) {
		TRAP(0xeb)
		TRAP(0x46)
	}

	// Are we coming from S4?
	If (Arg0 == 4) {
		Notify(SLPB, 0x02)
		If (DTSE) {
			TRAP(0x47)
		}
	}

	// Windows XP SP2 P-State restore
	If ((OSYS == 2002) && (CFGD & 0x01)) {
		If (\_SB.CP00._PPC > 0) {
			\_SB.CP00._PPC -= 1
			PNOT()
			\_SB.CP00._PPC += 1
			PNOT()
		} Else {
			\_SB.CP00._PPC += 1
			PNOT()
			\_SB.CP00._PPC -= 1
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
		If ((OSYS == 2001) && MPEN) {
			TRAP(0x3d)
		}

		/* OS Init */
		TRAP(0x32)
	}
}
