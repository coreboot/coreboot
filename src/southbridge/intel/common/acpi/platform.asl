/* SPDX-License-Identifier: GPL-2.0-only */

/* The APM port can be used for generating software SMIs */

OperationRegion (APMP, SystemIO, 0xb2, 2)
Field (APMP, ByteAcc, NoLock, Preserve)
{
	APMC, 8,	// APM command
	APMS, 8		// APM status
}

#include <arch/x86/acpi/post.asl>

Method(GOS, 0)
{
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
	OSYS = 2000

	If (CondRefOf(_OSI)) {
		If (_OSI("Windows 2001")) {
			OSYS = 2001
		}

		If (_OSI("Windows 2001 SP1")) {
			OSYS = 2001
		}

		If (_OSI("Windows 2001 SP2")) {
			OSYS = 2002
		}

		If (_OSI("Windows 2006")) {
			OSYS = 2006
		}
	}
}

/* Arg0 is state of HPET hardware enablement. */
Method (HPTS, 1)
{
	/* HPET hardware disabled. */
	If (!Arg0) {
		Return (0x0)
	}

	/* Ancient versions of Windows don't want to see the HPET. */
	If (OSYS < 2001) {
		Return (0xb)
	}

	Return (0xf)
}
