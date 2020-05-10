/* SPDX-License-Identifier: GPL-2.0-only */

// Scope is \_SB.PCI0.LPCB

Device (SIO) {
	Name (_HID, EisaId("PNP0A05"))
	Name (_UID, 0)

// Keyboard or AUX port (a.k.a Mouse)
#ifdef SIO_EC_ENABLE_PS2K
	#include <drivers/pc80/pc/ps2_controller.asl>
#endif
}
