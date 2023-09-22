/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/xhci.h>

/*
 * USB Port Wake Enable (UPWE) on usb attach/detach
 * Arg0 - Port Number
 * Arg1 - Port 1 Status and control offset
 * Arg2 - xHCI Memory-mapped address
 */
Method (UPWE, 3, Serialized)
{
	Local0 = Arg1 + ((Arg0 - 1) * 0x10)

	/* Map ((XMEM << 16) + Local0 in PSCR */
	OperationRegion (PSCR, SystemMemory, (Arg2 << 16) + Local0, 0x10)
	Field (PSCR, DWordAcc, NoLock, Preserve)
	{
		PSCT, 32,
	}
	Local0 = PSCT
	Local0 &= PORTSCN_BITS_OFF_MASK
	Local0 |= PORTSCN_WAKE_ON_BOTH_CONNECT_DISCONNECT_ENABLE
	PSCT = Local0
}

/*
 * USB Wake Enable Setup (UWES)
 * Arg0 - Port enable bitmap
 * Arg1 - Port 1 Status and control offset
 * Arg2 - xHCI Memory-mapped address
 */
Method (UWES, 3, Serialized)
{
	Local0 = Arg0

	While (1) {
		FindSetRightBit (Local0, Local1)
		If (Local1 == 0) {
			Break
		}
		UPWE (Local1, Arg1, Arg2)
		/*
		 * Clear the lowest set bit in Local0 since it was
		 * processed.
		 */
		Local0 &= (Local0 - 1)
	}
}
