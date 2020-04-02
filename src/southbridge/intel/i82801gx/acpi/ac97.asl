/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/* Intel i82801G AC'97 Audio and Modem */

// Intel AC'97 Audio 0:1e.2

Device (AUD0)
{
	Name (_ADR, 0x001e0002)
}

// Intel AC'97 Modem 0:1e.3

Device (MODM)
{
	Name (_ADR, 0x001e0003)

	Name (_PRW, Package(){ 5, 4 })
}
