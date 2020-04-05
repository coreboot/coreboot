/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <variant/onboard.h>

Scope (\_SB.GPNC)
{
	Method (_AEI, 0, Serialized)  // _AEI: ACPI Event Interrupts
	{
		Name (RBUF, ResourceTemplate ()
		{
			GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullDefault,,
				"\\_SB.GPNC") { BOARD_SCI_GPIO_INDEX }
		})
		Return (RBUF)
	}

	Method (_E0F, 0, NotSerialized)  // _Exx: Edge-Triggered GPE
	{
	}
}

/* Variant-specific I2C devices */
#include <variant/acpi/mainboard.asl>
