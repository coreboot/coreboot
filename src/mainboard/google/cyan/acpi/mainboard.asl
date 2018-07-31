/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
 * Copyright (C) 2015 Intel Corp.
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
