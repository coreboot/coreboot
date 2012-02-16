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

/* SMI Function Trap */
OperationRegion(SMI1, SystemMemory, 0xC0DEDEAD, 0x100)
Field(SMI1, AnyAcc, NoLock, Preserve)
{
	BCMD,  8,
	DID,  32,
	INFO, 1024
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
	TRAP(0xed)
	Sleep(1000)

	Store(0, \_SB.ACFG)

	// Are we going to S3?
	If (LEqual(Arg0, 3)) {
		Store (0x4c, BCMD)
	}

	// Are we going to S4?
	If (Lequal(Arg0, 4)) {
		TRAP(0xe7)
		TRAP(0xea)
	}

	// Are we going to S5?
	If (Lequal(Arg0, 5)) {
		Store (0x4b, BCMD)
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
		If (LGreater(\_PR.CPU0._PPC, 0)) {
			Subtract(\_PR.CPU0._PPC, 1, \_PR.CPU0._PPC)
			PNOT()
			Add(\_PR.CPU0._PPC, 1, \_PR.CPU0._PPC)
			PNOT()
		} Else {
			Add(\_PR.CPU0._PPC, 1, \_PR.CPU0._PPC)
			PNOT()
			Subtract(\_PR.CPU0._PPC, 1, \_PR.CPU0._PPC)
			PNOT()
		}
	}

	Return(Package(){0,0})
}

// Power notification

External (\_PR_.CPU0, DeviceObj)
External (\_PR_.CPU1, DeviceObj)
External (\_PR_.CPU0._PPC)
External (\_PR_.CPU1._PPC)
/* These come from the dynamically created CPU SSDT */
External(PDC0)
External(PDC1)

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
	Notify(\_SB.BAT0, 0x80) // Execute BAT0 _BST
	Notify(\_SB.BAT1, 0x80) // Execute BAT1 _BST
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
			TRAP(0x3d)
		}

		/* SMM power state and C4-on-C3 settings need to be updated */
		// TRAP(43)

		/* OS Init */
		TRAP(0x32)
	}
}

