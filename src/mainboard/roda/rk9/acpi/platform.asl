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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

/* These come from the dynamically created CPU SSDT */
External(PDC0)
External(PDC1)

/* The APM port can be used for generating software SMIs */

OperationRegion (APMP, SystemIO, 0xb2, 2)
Field (APMP, ByteAcc, NoLock, Preserve)
{
	APMC, 8,	// APM command
	APMS, 8		// APM status
}

/* Port 80 POST */

OperationRegion (POST, SystemIO, 0x80, 1)
Field (POST, ByteAcc, Lock, Preserve)
{
	DBG0, 8
}

/* SMI I/O Trap */
Method(TRAP, 1, Serialized)
{
	Store (Arg0, SMIF)	// SMI Function
	Store (0, TRP0)		// Generate trap
	Return (SMIF)		// Return value of SMI handler
}

/* The _PIC method is called by the OS to choose between interrupt
 * routing via the i8259 interrupt controller or the APIC.
 *
 * _PIC is called with a parameter of 0 for i8259 configuration and
 * with a parameter of 1 for Local Apic/IOAPIC configuration.
 */

Method(_PIC, 1)
{
	// Remember the OS' IRQ routing choice.
	Store(Arg0, PICM)
}

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

	// TODO: Return Arg0 as second value if S-Arg0 was entered
	// before.

	Return(Package(){0,0})
}

// Power notification

External (\_PR_.CPU0, DeviceObj)
External (\_PR_.CPU1, DeviceObj)

Method (PNOT)
{
	If (MPEN) {
		If(And(PDC0, 0x08)) {
			Notify (\_PR_.CPU0, 0x80)	 // _PPC

			If (And(PDC0, 0x10)) {
				Sleep(100)
				Notify(\_PR_.CPU0, 0x81) // _CST
			}
		}

		If(And(PDC1, 0x08)) {
			Notify (\_PR_.CPU1, 0x80)	 // _PPC
			If (And(PDC1, 0x10)) {
				Sleep(100)
				Notify(\_PR_.CPU1, 0x81) // _CST
			}
		}

	} Else { // UP
		Notify (\_PR_.CPU0, 0x80)
		Sleep(0x64)
		Notify(\_PR_.CPU0, 0x81)
	}

	// Notify the Batteries
	Notify(BAT1, 0x80) // Execute BAT1 _BST
	Notify(BAT2, 0x80) // Execute BAT2 _BST
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

		/* Determine the Operating System and save the value in OSYS.
		 * We have to do this in order to be able to work around
		 * certain windows bugs.
		 *
		 *    OSYS value | Operating System
		 *    -----------+------------------
		 *       2000    | Windows 2000
		 *       2001    | Windows XP(+SP1)
		 *       2002    | Windows XP SP2
		 *       2006    | Windows Vista
		 *       ????    | Windows 7
		 */

		/* Let's assume we're running at least Windows 2000 */
		Store (2000, OSYS)

		If (CondRefOf(_OSI, Local0)) {
			/* Linux answers _OSI with "True" for a couple of
			 * Windows version queries. But unlike Windows it
			 * needs a Video repost, so let's determine whether
			 * we're running Linux.
			 */

			If (_OSI("Linux")) {
				Store (1, LINX)
			}

			If (_OSI("Windows 2001")) {
				Store (2001, OSYS)
			}

			If (_OSI("Windows 2001 SP1")) {
				Store (2001, OSYS)
			}

			If (_OSI("Windows 2001 SP2")) {
				Store (2002, OSYS)
			}

			If (_OSI("Windows 2006")) {
				Store (2006, OSYS)
			}
		}

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

