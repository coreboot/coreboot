/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Patrick Rudolph <siro@das-labor.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#undef SUPERIO_DEV
#undef SUPERIO_PNP_BASE
#define SUPERIO_DEV		SIO0
#define SUPERIO_PNP_BASE	0x2e

#define SUPERIO_SHOW_SP2
#define SUPERIO_SHOW_KBC

#include <superio/nuvoton/npcd378/acpi/superio.asl>

Scope (\_GPE)
{
	Method (_L08, 0, NotSerialized)
	{
		\_SB.PCI0.LPCB.SIO0.SIOH ()
	}

	Method (_L0D, 0, NotSerialized)
	{
		Notify (\_SB.PCI0.EHC1, 0x02)
		Notify (\_SB.PCI0.EHC2, 0x02)
		//FIXME: Add GBE device
		//Notify (\_SB.PCI0.GBE, 0x02)
	}

	Method (_L09, 0, NotSerialized)
	{
		Notify (\_SB.PCI0.RP01, 0x02)
		Notify (\_SB.PCI0.RP02, 0x02)
		Notify (\_SB.PCI0.RP03, 0x02)
		Notify (\_SB.PCI0.RP04, 0x02)
		Notify (\_SB.PCI0.RP05, 0x02)
		Notify (\_SB.PCI0.RP06, 0x02)
		Notify (\_SB.PCI0.RP07, 0x02)
		Notify (\_SB.PCI0.RP08, 0x02)
		Notify (\_SB.PCI0.PEGP, 0x02)
	}
}
