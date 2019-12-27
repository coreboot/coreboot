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

Method(GOS, 0)
{
	/* Determine the Operating System and save the value in OSYS.
	 * We have to do this in order to be able to work around
	 * certain windows bugs.
	 *
	 *    OSYS value | Operating System
	 *    -----------+------------------
	 *       2000    | Windows 2000
	 *       2001    | Windows XP
	 *       2001    | Windows XP SP1
	 *       2001    | Windows Server 2003
	 *       2001    | Windows Server 2003 SP1
	 *       2002    | Windows XP SP2
	 *       2006    | Windows Vista
	 *       2006    | Windows Vista SP1
	 *       2006    | Windows Server 2008
	 *       2009    | Windows 7
	 *       2012    | Windows 8
	 */

	/* Let's assume we're running at least Windows 2000 */
	Store (2000, OSYS)

	If (CondRefOf(_OSI)) {
		If (_OSI("Windows 2001")) {
			Store (2001, OSYS)
		}

		If (_OSI("Windows 2001 SP1")) {
			Store (2001, OSYS)
		}

		If (_OSI("Windows 2001.1")) {
			Store (2001, OSYS)
		}

		If (_OSI("Windows 2001.1 SP1")) {
			Store (2001, OSYS)
		}

		If (_OSI("Windows 2001 SP2")) {
			Store (2002, OSYS)
		}

		If (_OSI("Windows 2006")) {
			Store (2006, OSYS)
		}

		If (_OSI("Windows 2006 SP1")) {
			Store (2006, OSYS)
		}

		If (_OSI("Windows 2006.1")) {
			Store (2006, OSYS)
		}

		If (_OSI("Windows 2009")) {
			Store (2009, OSYS)
		}

		If (_OSI("Windows 2012")) {
			Store (2012, OSYS)
		}
	}
}
